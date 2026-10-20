#include "persistent_list.hpp"

#include <utility>

PersistentList::PersistentList() : heads_{nullptr} {}

PersistentList::Node* PersistentList::make_node(std::int64_t key, Node* next) {
    auto owned = std::make_unique<Node>();
    owned->key = key;
    owned->base_next = next;
    Node* const raw = owned.get();
    nodes_.push_back(std::move(owned));
    return raw;
}

PersistentList::Node* PersistentList::next_at(const Node* node,
                                               std::size_t version) const {
    Node* result = node->base_next;
    std::size_t selected_version = 0;
    for (std::size_t index = 0; index < node->modification_count; ++index) {
        const Modification& modification = node->modifications[index];
        if (modification.version <= version &&
            modification.version >= selected_version) {
            selected_version = modification.version;
            result = modification.next;
        }
    }
    return result;
}

void PersistentList::write_next(Node* node, Node* target,
                                std::size_t version, Node*& head) {
    if (node->modification_count < node->modifications.size()) {
        node->modifications[node->modification_count++] = {version, target};
        if (target != nullptr) {
            target->predecessor = node;
        }
        return;
    }

    Node* const predecessor = node->predecessor;
    Node* const copy = make_node(node->key, target);
    if (target != nullptr) {
        target->predecessor = copy;
    }
    if (predecessor == nullptr) {
        copy->predecessor = nullptr;
        head = copy;
    } else {
        copy->predecessor = predecessor;
        write_next(predecessor, copy, version, head);
    }
}

void PersistentList::include(std::int64_t key) {
    const std::size_t previous_version = heads_.size() - 1;
    const std::size_t new_version = heads_.size();
    Node* head = heads_.back();
    Node* predecessor = nullptr;
    Node* current = head;
    while (current != nullptr && current->key < key) {
        predecessor = current;
        current = next_at(current, previous_version);
    }

    Node* const inserted = make_node(key, current);
    if (current != nullptr) {
        current->predecessor = inserted;
    }
    if (predecessor == nullptr) {
        head = inserted;
    } else {
        inserted->predecessor = predecessor;
        write_next(predecessor, inserted, new_version, head);
    }
    heads_.push_back(head);
}

std::size_t PersistentList::resolve_version(
    std::int64_t requested_version) const noexcept {
    if (requested_version >= 0 &&
        static_cast<std::uint64_t>(requested_version) < heads_.size()) {
        return static_cast<std::size_t>(requested_version);
    }
    return heads_.size() - 1;
}

void PersistentList::print(std::int64_t requested_version,
                           std::ostream& output) const {
    const std::size_t version = resolve_version(requested_version);
    Node* current = heads_[version];
    bool first = true;
    while (current != nullptr) {
        if (!first) {
            output << ", ";
        }
        output << current->key;
        first = false;
        current = next_at(current, version);
    }
    output << '\n';
}

std::size_t PersistentList::version_count() const noexcept {
    return heads_.size();
}

std::size_t PersistentList::node_count() const noexcept {
    return nodes_.size();
}

bool PersistentList::validate_current_links() const {
    const std::size_t version = heads_.size() - 1;
    Node* predecessor = nullptr;
    Node* current = heads_[version];
    std::size_t visited = 0;
    while (current != nullptr) {
        if (++visited > nodes_.size() || current->predecessor != predecessor ||
            (predecessor != nullptr && predecessor->key > current->key) ||
            current->modification_count > current->modifications.size()) {
            return false;
        }
        predecessor = current;
        current = next_at(current, version);
    }
    return true;
}
