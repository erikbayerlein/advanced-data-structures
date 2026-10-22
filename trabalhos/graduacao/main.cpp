#include <charconv>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "persistent_list.hpp"

enum class Operation { Include, Remove, Successor, Print };

struct Command {
    Operation operation;
    std::int64_t number = 0;
    std::int64_t version = 0;
};

bool parse_integer(const std::string& text, std::int64_t& value) {
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end;
}

bool parse_command(const std::string& line, Command& command) {
    std::istringstream input(line);
    std::vector<std::string> words;
    for (std::string word; input >> word;) {
        words.push_back(word);
    }

    if (words.size() == 2 && words[0] == "IMP" &&
        parse_integer(words[1], command.version)) {
        command.operation = Operation::Print;
        return true;
    }
    if (words.size() == 2 && words[0] == "INC" &&
        parse_integer(words[1], command.number) && command.number >= 0) {
        command.operation = Operation::Include;
        return true;
    }
    if (words.size() == 2 && words[0] == "REM" &&
        parse_integer(words[1], command.number)) {
        command.operation = Operation::Remove;
        return true;
    }
    if (words.size() == 3 && words[0] == "SUC" &&
        parse_integer(words[1], command.number) &&
        parse_integer(words[2], command.version)) {
        command.operation = Operation::Successor;
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc != 2 || argv[1][0] == '\0') {
        std::cerr << "uso: programa CAMINHO_DA_ENTRADA\n";
        return 2;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "não foi possível abrir a entrada: " << argv[1] << '\n';
        return 2;
    }

    PersistentList list;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        Command command{};
        if (!parse_command(line, command)) {
            std::cerr << "linha " << line_number << ": comando inválido\n";
            return 2;
        }
        if (command.operation == Operation::Include) {
            list.include(command.number);
            continue;
        }
        if (command.operation == Operation::Remove) {
            list.remove(command.number);
            continue;
        }
        std::cout << line << '\n';
        if (command.operation == Operation::Successor) {
            const std::optional<std::int64_t> result =
                list.successor(command.number, command.version);
            if (result.has_value()) {
                std::cout << *result << '\n';
            } else {
                std::cout << "inf\n";
            }
            continue;
        }
        list.print(command.version, std::cout);
    }

    if (!input.eof()) {
        std::cerr << "erro durante a leitura da entrada\n";
        return 2;
    }
    return 0;
}
