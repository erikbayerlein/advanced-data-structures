#include "persistent_list.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using Multiset = std::vector<std::int64_t>;

void require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

std::string printed(const PersistentList& list, std::int64_t version) {
    std::ostringstream output;
    list.print(version, output);
    return output.str();
}

std::string expected_print(const Multiset& values) {
    std::ostringstream output;
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            output << ", ";
        }
        output << values[index];
    }
    output << '\n';
    return output.str();
}

std::optional<std::int64_t> expected_successor(const Multiset& values,
                                               std::int64_t key) {
    const auto found = std::upper_bound(values.begin(), values.end(), key);
    if (found == values.end()) {
        return std::nullopt;
    }
    return *found;
}

// Oráculo usado apenas no teste: guarda uma cópia completa de cada versão.
class Oracle {
public:
    Oracle() : versions_(1) {}

    void include(std::int64_t key) {
        Multiset next = versions_.back();
        next.insert(std::upper_bound(next.begin(), next.end(), key), key);
        versions_.push_back(next);
    }

    void remove(std::int64_t key) {
        Multiset next = versions_.back();
        const auto found = std::lower_bound(next.begin(), next.end(), key);
        if (found != next.end() && *found == key) {
            next.erase(found);
        }
        versions_.push_back(next);
    }

    const std::vector<Multiset>& versions() const { return versions_; }

private:
    std::vector<Multiset> versions_;
};

void apply(PersistentList& list, Oracle& oracle, bool include,
           std::int64_t key) {
    if (include) {
        list.include(key);
        oracle.include(key);
        return;
    }
    list.remove(key);
    oracle.remove(key);
}

void check_version(const PersistentList& list, const Multiset& values,
                   std::size_t version, const std::string& label) {
    const std::string where = label + ", versão " + std::to_string(version);
    require(list.validate_version(version), where + ": invariante violada");
    require(printed(list, static_cast<std::int64_t>(version)) ==
                expected_print(values),
            where + ": IMP diverge do oráculo");
    const std::int64_t last = values.empty() ? 0 : values.back();
    for (std::int64_t key = -1; key <= last + 1; ++key) {
        require(list.successor(key, static_cast<std::int64_t>(version)) ==
                    expected_successor(values, key),
                where + ": SUC " + std::to_string(key) + " diverge");
    }
}

void check_all_versions(const PersistentList& list, const Oracle& oracle,
                        const std::string& label) {
    const std::vector<Multiset>& versions = oracle.versions();
    require(list.version_count() == versions.size(),
            label + ": quantidade de versões diverge");
    require(list.validate_current_links(),
            label + ": retornos da versão corrente inválidos");
    for (std::size_t version = 0; version < versions.size(); ++version) {
        check_version(list, versions[version], version, label);
    }
}

void test_third_write_copies_node() {
    PersistentList list;
    Oracle oracle;
    for (const std::int64_t key : {2, 5, 3}) {
        apply(list, oracle, true, key);
    }
    require(list.node_count() == 3 && list.modification_count() == 2,
            "o nó 2 deveria ter os dois registros ocupados");
    apply(list, oracle, true, 3);
    require(list.node_count() == 5 && list.modification_count() == 2,
            "a terceira escrita em 2.next deveria copiar o nó 2");
    check_all_versions(list, oracle, "terceira escrita");
}

void test_cascade_to_head() {
    PersistentList list;
    Oracle oracle;
    apply(list, oracle, true, 30);
    apply(list, oracle, true, 20);
    apply(list, oracle, true, 10);
    apply(list, oracle, true, 20);
    apply(list, oracle, false, 20);
    apply(list, oracle, true, 30);
    apply(list, oracle, false, 30);
    const std::size_t nodes_before = list.node_count();
    apply(list, oracle, true, 25);
    // Novo nó 25 + cópia de 20 + cópia de 10 (nova cabeça).
    require(list.node_count() == nodes_before + 3,
            "a inserção de 25 deveria copiar 20 e 10 em cascata");
    require(printed(list, 8) == "10, 20, 25, 30\n",
            "versão 8 da cascata incorreta");
    check_all_versions(list, oracle, "cascata até a cabeça");
}

void run_random_sequence(unsigned seed, std::size_t updates,
                         std::int64_t max_key, PersistentList& list,
                         Oracle& oracle) {
    std::mt19937 random(seed);
    std::uniform_int_distribution<std::int64_t> key(0, max_key);
    std::bernoulli_distribution include(0.6);
    for (std::size_t step = 0; step < updates; ++step) {
        apply(list, oracle, include(random), key(random));
    }
}

void test_random_sequences_against_oracle() {
    for (unsigned seed = 1; seed <= 200; ++seed) {
        PersistentList list;
        Oracle oracle;
        run_random_sequence(seed, 1 + seed % 60, 8, list, oracle);
        check_all_versions(list, oracle,
                           "semente " + std::to_string(seed));
    }
}

void test_thousand_updates() {
    PersistentList list;
    Oracle oracle;
    run_random_sequence(1000, 1000, 50, list, oracle);
    require(list.version_count() == 1001,
            "1000 atualizações devem produzir 1001 versões");
    check_all_versions(list, oracle, "1000 atualizações");
    require(printed(list, 0) == "\n", "a versão 0 deve continuar vazia");
}

// Cada atualização termina com no máximo uma escrita em registro livre, e um
// nó cheio só é copiado uma vez: registros <= v e cópias <= v/2.
void test_linear_space() {
    for (const std::size_t updates : {100, 500, 1000}) {
        PersistentList list;
        Oracle oracle;
        run_random_sequence(7, updates, 20, list, oracle);
        std::cout << "carga " << updates << ": versões "
                  << list.version_count() << ", nós " << list.node_count()
                  << ", registros " << list.modification_count() << '\n';
        require(list.modification_count() <= updates,
                "registros de modificação não são lineares em v");
        require(2 * list.node_count() <= 3 * updates,
                "nós físicos não são lineares em v");
    }
}

void test_queries_do_not_allocate() {
    PersistentList list;
    Oracle oracle;
    run_random_sequence(42, 1000, 50, list, oracle);
    const std::size_t versions = list.version_count();
    const std::size_t nodes = list.node_count();
    const std::size_t modifications = list.modification_count();
    std::mt19937 random(43);
    std::uniform_int_distribution<std::int64_t> version(-5, 1100);
    std::uniform_int_distribution<std::int64_t> key(-1, 52);
    std::ostringstream sink;
    for (std::size_t query = 0; query < 100000; ++query) {
        list.successor(key(random), version(random));
        if (query % 100 == 0) {
            list.print(version(random), sink);
        }
    }
    require(list.version_count() == versions && list.node_count() == nodes &&
                list.modification_count() == modifications,
            "consultas não devem criar versões, nós ou registros");
}

int main() {
    test_third_write_copies_node();
    test_cascade_to_head();
    test_random_sequences_against_oracle();
    test_thousand_updates();
    test_linear_space();
    test_queries_do_not_allocate();
    return 0;
}
