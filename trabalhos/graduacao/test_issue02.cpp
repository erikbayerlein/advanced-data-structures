#include "persistent_list.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
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
    require(list.version_count() == 1 && list.node_count() == 0,
            "a versão 0 deve ser vazia");
    require(printed(list, 0) == "\n", "IMP 0 deve produzir linha vazia");

    for (std::int64_t key : {2, 5, 3, 3}) {
        list.include(key);
        require(list.validate_current_links(),
                "predecessores da versão corrente inconsistentes");
    }
    require(list.version_count() == 5, "cada INC deve criar uma versão");
    require(list.node_count() == 5,
            "a terceira alteração em um elo cheio deve copiar o nó");
    const std::vector<std::string> expected = {
        "\n", "2\n", "2, 5\n", "2, 3, 5\n", "2, 3, 3, 5\n"};
    for (std::size_t version = 0; version < expected.size(); ++version) {
        require(printed(list, static_cast<std::int64_t>(version)) ==
                    expected[version],
                "versão histórica alterada ou impressa incorretamente");
    }
    require(printed(list, 99) == expected.back() &&
                printed(list, -1) == expected.back(),
            "versão inexistente deve usar a mais recente");
    require(list.version_count() == 5 && list.node_count() == 5,
            "IMP não pode criar nós ou versões");

    PersistentList varied;
    std::vector<std::vector<std::int64_t>> snapshots(1);
    std::uint32_t state = 0x12345678;
    bool saw_cascade = false;
    for (std::size_t step = 1; step <= 250; ++step) {
        state = state * 1664525u + 1013904223u;
        const std::int64_t key = static_cast<std::int64_t>((state >> 16) % 97);
        const std::size_t previous_node_count = varied.node_count();
        varied.include(key);
        saw_cascade |= varied.node_count() >= previous_node_count + 3;
        snapshots.push_back(snapshots.back());
        auto& current = snapshots.back();
        current.insert(std::lower_bound(current.begin(), current.end(), key),
                       key);
        require(varied.version_count() == step + 1,
                "contagem incorreta de versões");
        require(varied.validate_current_links(),
                "predecessores ou ordenação incorretos após inserção");
        require(printed(varied, static_cast<std::int64_t>(step)) ==
                    expected_print(current),
                "conteúdo da versão corrente não corresponde ao oráculo");
    }
    for (std::size_t version = 0; version < snapshots.size(); ++version) {
        require(printed(varied, static_cast<std::int64_t>(version)) ==
                    expected_print(snapshots[version]),
                "histórico não corresponde ao oráculo");
    }
    require(saw_cascade, "teste não exercitou cópia em cascata");

    varied.include(0);
    varied.include(32767);
    varied.include(65535);
    varied.include(std::numeric_limits<std::int64_t>::max());
    require(varied.validate_current_links(),
            "predecessores incorretos com valores limite");
    require(printed(varied, static_cast<std::int64_t>(varied.version_count() - 1))
                .find("32767, 65535, 9223372036854775807\n") !=
                std::string::npos,
            "valores limite foram perdidos ou reordenados");

    std::cout << "Issue #2: invariantes e histórico verificados\n";
}
