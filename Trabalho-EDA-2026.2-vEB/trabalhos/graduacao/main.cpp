#include <charconv>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "van_emde_boas.hpp"

namespace {

enum class Operation { Include, Remove, Successor, Predecessor, Print };

struct Command {
    Operation operation;
    std::uint32_t key = 0;
};

std::optional<Command> parse_command(const std::string& line) {
    std::istringstream input(line);
    std::string name, number, extra;
    input >> name;

    if (name == "IMP") {
        if (input >> extra) return std::nullopt;
        return Command{Operation::Print};
    }

    Operation operation;
    if (name == "INC") {
        operation = Operation::Include;
    } else if (name == "REM") {
        operation = Operation::Remove;
    } else if (name == "SUC") {
        operation = Operation::Successor;
    } else if (name == "PRE") {
        operation = Operation::Predecessor;
    } else {
        return std::nullopt;
    }

    if (!(input >> number) || (input >> extra)) return std::nullopt;

    std::uint32_t key;
    const char* end = number.data() + number.size();
    const auto result = std::from_chars(number.data(), end, key, 10);
    if (result.ec != std::errc{} || result.ptr != end) return std::nullopt;
    return Command{operation, key};
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 2 || argv[1][0] == '\0') {
        std::cerr << "informe o caminho do arquivo de entrada\n";
        return 2;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "não foi possível abrir o arquivo de entrada " << argv[1] << '\n';
        return 2;
    }

    const SimpleTabulation hash(std::random_device{}());
    VanEmdeBoas tree(hash);
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        const auto command = parse_command(line);
        if (!command) {
            std::cerr << "comando inválido na linha " << line_number << '\n';
            return 2;
        }

        switch (command->operation) {
            case Operation::Print: {
                std::cout << line << '\n';
                bool first = true;
                tree.for_each([&](std::uint32_t key) {
                    if (!first) std::cout << ", ";
                    std::cout << key;
                    first = false;
                });
                std::cout << '\n';
                break;
            }
            case Operation::Include:
                try {
                    tree.insert(command->key);
                } catch (const std::overflow_error& error) {
                    std::cerr << error.what() << " na linha " << line_number << '\n';
                    return 2;
                }
                break;
            case Operation::Remove:
            case Operation::Successor:
            case Operation::Predecessor:
                std::cerr << "operação ainda indisponível na linha " << line_number << '\n';
                return 2;
        }
    }

    if (input.bad() || !input.eof()) {
        std::cerr << "erro durante a leitura do arquivo de entrada\n";
        return 2;
    }
}
