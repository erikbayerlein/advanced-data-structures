#include "van_emde_boas.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

namespace {

void check_order(const VanEmdeBoas& tree, const std::vector<std::uint32_t>& expected) {
    std::vector<std::uint32_t> actual;
    tree.for_each([&](std::uint32_t key) { actual.push_back(key); });
    assert(actual == expected);
}

void check_collisions_and_growth(const SimpleTabulation& hash) {
    ClusterTable<int> table(hash);
    assert(table.capacity() == 0 && !table.find(0));

    // As quatro primeiras entradas começam no slot 7 e ocupam 7, 0, 1 e 2.
    std::vector<std::uint32_t> colliders;
    for (std::uint32_t key = 0; colliders.size() < 6; ++key) {
        if ((hash(key) & 7u) == 7u) colliders.push_back(key);
    }

    std::vector<const int*> addresses;
    for (int i = 0; i < 5; ++i) {
        auto child = std::make_unique<int>(i);
        addresses.push_back(child.get());
        table.insert(colliders[i], std::move(child));
        assert(table.size() == addresses.size());
        assert(table.capacity() == (i < 4 ? 8u : 16u));

        const auto& view = table;
        for (int j = 0; j <= i; ++j) {
            const int* found = view.find(colliders[j]);
            assert(found == addresses[j] && *found == j);
        }
        assert(!view.find(colliders[i + 1]));
    }
}

}  // namespace

struct VanEmdeBoasTest {
    static void check_layout(const SimpleTabulation& hash) {
        VanEmdeBoas tree(hash);
        assert(!tree.minimum_ && !tree.summary_ && tree.clusters_.capacity() == 0);
        check_order(tree, {});

        constexpr auto largest = std::numeric_limits<std::uint32_t>::max();
        tree.insert(largest);
        assert(tree.minimum_ == largest && tree.maximum_ == largest);
        assert(!tree.summary_ && tree.clusters_.capacity() == 0);
        check_order(tree, {largest});

        for (const auto key : {65536u, 65535u, 0u, 2147483648u}) tree.insert(key);
        check_order(tree, {0, 65535, 65536, 2147483648u, largest});
        assert(tree.minimum_ == 0 && tree.maximum_ == largest);
        assert(tree.clusters_.size() == 4);
        assert(tree.summary_ && &tree.summary_->hash_ == &hash);
        check_order(*tree.summary_, {0, 1, 32768, 65535});
        for (const auto index : {0u, 1u, 32768u, 65535u}) {
            const auto* child = tree.clusters_.find(index);
            assert(child && child->minimum_ && &child->hash_ == &hash);
            assert(!child->summary_ && child->clusters_.capacity() == 0);
        }
        assert(!tree.clusters_.find(2));
        check_order(*tree.clusters_.find(0), {65535});
    }
};

int main() {
    const SimpleTabulation hash(42);
    check_collisions_and_growth(hash);
    VanEmdeBoasTest::check_layout(hash);

    VanEmdeBoas leaf(hash, 1);
    leaf.insert(1);
    check_order(leaf, {1});
    leaf.insert(0);
    check_order(leaf, {0, 1});

    VanEmdeBoas odd_width(hash, 3);
    for (const auto key : {7u, 4u, 2u, 5u, 1u, 0u, 6u, 3u}) odd_width.insert(key);
    check_order(odd_width, {0, 1, 2, 3, 4, 5, 6, 7});

    std::cout << "Tabela de clusters e representação vEB verificadas\n";
}
