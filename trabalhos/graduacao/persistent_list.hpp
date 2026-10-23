#ifndef PERSISTENT_LIST_HPP
#define PERSISTENT_LIST_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <vector>

class PersistentList {
public:
    PersistentList();

    void include(std::int64_t key);
    void remove(std::int64_t key);
    void print(std::int64_t requested_version, std::ostream& output) const;
    // Menor chave estritamente maior que `key` na versao pedida (ou na mais
    // recente se a versao nao existir). std::nullopt representa `inf`.
    std::optional<std::int64_t> successor(std::int64_t key,
                                          std::int64_t requested_version) const;

    std::size_t version_count() const noexcept;
    std::size_t node_count() const noexcept;
    bool validate_current_links() const;

private:
    struct Node;

    struct Modification {
        std::size_t version = 0;
        Node* next = nullptr;
    };

    struct Node {
        std::int64_t key = 0;
        Node* base_next = nullptr;
        std::array<Modification, 2> modifications{};
        std::size_t modification_count = 0;
        Node* predecessor = nullptr; // Only valid in the latest version.
    };

    Node* make_node(std::int64_t key, Node* next);
    Node* next_at(const Node* node, std::size_t version) const;
    void write_next(Node* node, Node* target, std::size_t version, Node*& head);
    std::size_t resolve_version(std::int64_t requested_version) const noexcept;

    std::vector<std::unique_ptr<Node>> nodes_;
    std::vector<Node*> heads_;
};

#endif
