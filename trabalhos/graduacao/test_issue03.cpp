#include "persistent_list.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

std::string expected_print(const std::vector<std::int64_t>& values) {
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

int main() {
    PersistentList list;
    list.remove(10);
    require(list.version_count() == 2 && list.node_count() == 0 &&
                printed(list, 1) == "\n" && list.validate_current_links(),
            "REM do vazio deve criar versão vazia sem nós");

    for (std::int64_t key : {2, 4, 6, 8}) {
        list.include(key);
    }
    require(printed(list, 5) == "2, 4, 6, 8\n",
            "pré-condição de remoção incorreta");
    list.remove(2); // Cabeça.
    require(printed(list, 6) == "4, 6, 8\n" &&
                list.validate_current_links(),
            "remoção da cabeça incorreta");
    list.remove(6); // Meio.
    require(printed(list, 7) == "4, 8\n" &&
                list.validate_current_links(),
            "remoção do meio incorreta");
    list.remove(8); // Cauda.
    require(printed(list, 8) == "4\n" && list.validate_current_links(),
            "remoção da cauda incorreta");

    const std::size_t nodes_before_absent = list.node_count();
    list.remove(42);
    list.remove(-1);
    require(list.version_count() == 11 &&
                list.node_count() == nodes_before_absent &&
                printed(list, 9) == "4\n" && printed(list, 10) == "4\n",
            "REM ausente ou negativo não deve alterar conteúdo nem nós");
    list.remove(4);
    list.remove(4);
    require(list.version_count() == 13 && printed(list, 11) == "\n" &&
                printed(list, 12) == "\n" && list.validate_current_links(),
            "esvaziamento e nova remoção do vazio incorretos");
    require(printed(list, 5) == "2, 4, 6, 8\n" &&
                printed(list, 7) == "4, 8\n",
            "remoção alterou uma versão anterior");

    PersistentList duplicates;
    duplicates.include(5);
    duplicates.include(5);
    duplicates.include(5);
    duplicates.remove(5);
    duplicates.remove(5);
    duplicates.remove(5);
    require(printed(duplicates, 3) == "5, 5, 5\n" &&
                printed(duplicates, 4) == "5, 5\n" &&
                printed(duplicates, 5) == "5\n" &&
                printed(duplicates, 6) == "\n" &&
                duplicates.validate_current_links(),
            "REM deve retirar uma ocorrência por comando");

    PersistentList cascade;
    const std::vector<std::pair<bool, std::int64_t>> operations = {
        {true, 30}, {true, 20},  {true, 10}, {true, 20},
        {false, 20}, {true, 30}, {false, 30}};
    for (const auto& operation : operations) {
        if (operation.first) {
            cascade.include(operation.second);
        } else {
            cascade.remove(operation.second);
        }
        require(cascade.validate_current_links(),
                "predecessores incorretos antes da cascata");
    }
    const std::size_t before_cascade = cascade.node_count();
    cascade.include(25);
    require(cascade.node_count() >= before_cascade + 3 &&
                cascade.validate_current_links(),
            "cascata deveria copiar dois predecessores");
    require(printed(cascade, 3) == "10, 20, 30\n" &&
                printed(cascade, 5) == "10, 20, 30\n" &&
                printed(cascade, 7) == "10, 20, 30\n" &&
                printed(cascade, 8) == "10, 20, 25, 30\n",
            "cascata corrompeu uma versão histórica ou a corrente");

    PersistentList varied;
    std::vector<std::vector<std::int64_t>> snapshots(1);
    std::uint32_t state = 0xabcdef12;
    for (std::size_t step = 1; step <= 300; ++step) {
        state = state * 1664525u + 1013904223u;
        const std::int64_t key = step % 3 == 0 && step % 17 == 0
                                     ? -1
                                     : static_cast<std::int64_t>((state >> 16) % 101);
        snapshots.push_back(snapshots.back());
        auto& current = snapshots.back();
        if (step % 3 != 0) {
            varied.include(key);
            current.insert(std::lower_bound(current.begin(), current.end(), key),
                           key);
        } else {
            const std::size_t before = varied.node_count();
            const auto found = std::lower_bound(current.begin(), current.end(), key);
            if (found == current.end() || *found != key) {
                varied.remove(key);
                require(varied.node_count() == before,
                        "REM ausente alocou um nó");
            } else {
                current.erase(found);
                varied.remove(key);
            }
        }
        require(varied.version_count() == step + 1 &&
                    varied.validate_current_links() &&
                    printed(varied, static_cast<std::int64_t>(step)) ==
                        expected_print(current),
                "atualização variada quebrou versão corrente ou predecessor");
    }
    for (std::size_t version = 0; version < snapshots.size(); ++version) {
        require(printed(varied, static_cast<std::int64_t>(version)) ==
                    expected_print(snapshots[version]),
                "atualização variada corrompeu histórico");
    }

    std::cout << "Issue #3: remoção e histórico verificados\n";
}
