#include "van_emde_boas.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace {

void check_order(const VanEmdeBoas& tree, const std::vector<std::uint32_t>& expected) {
    std::vector<std::uint32_t> actual;
    tree.for_each([&](std::uint32_t key) { actual.push_back(key); });
    assert(actual == expected);
}

void check_overflow(VanEmdeBoas& tree, std::uint32_t key) {
    bool rejected = false;
    try {
        tree.insert(key);
    } catch (const std::overflow_error&) {
        rejected = true;
    }
    assert(rejected);
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
        assert(tree.minimum_ && tree.minimum_->key == largest && tree.maximum_ == largest);
        assert(!tree.summary_ && tree.clusters_.capacity() == 0);
        check_order(tree, {largest});

        for (const auto key : {65536u, 65535u, 0u, 2147483648u}) tree.insert(key);
        check_order(tree, {0, 65535, 65536, 2147483648u, largest});
        assert(tree.minimum_ && tree.minimum_->key == 0 && tree.maximum_ == largest);
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

    static void check_multiplicities(const SimpleTabulation& hash) {
        constexpr auto largest = std::numeric_limits<std::uint32_t>::max();
        VanEmdeBoas tree(hash);
        for (int i = 0; i < 10000; ++i) tree.insert(9);
        assert(tree.minimum_->count == 10000 && tree.maximum_ == 9);
        assert(!tree.summary_ && tree.clusters_.capacity() == 0);
        check_order(tree, std::vector<std::uint32_t>(10000, 9));

        for (const auto key : {1u, 65536u, 131072u, largest}) tree.insert(key);
        const auto* summary = tree.summary_.get();
        const auto* cluster = tree.clusters_.find(0);
        assert(cluster && cluster->minimum_->count == 10000);
        const auto capacity = tree.clusters_.capacity();
        assert(tree.clusters_.size() == 4 && capacity == 8);

        tree.insert(1);
        tree.insert(9);
        tree.insert(largest);
        assert(tree.clusters_.size() == 4 && tree.clusters_.capacity() == capacity);
        assert(tree.summary_.get() == summary && tree.clusters_.find(0) == cluster);
        assert(!cluster->summary_ && cluster->clusters_.capacity() == 0);
        check_order(*summary, {0, 1, 2, 65535});
        assert(tree.maximum_ == largest);

        std::vector<std::uint32_t> expected{1, 1};
        expected.insert(expected.end(), 10001, 9);
        expected.insert(expected.end(), {65536, 131072, largest, largest});
        check_order(tree, expected);
    }

    static void check_count_limits(const SimpleTabulation& hash) {
        constexpr auto limit = std::numeric_limits<std::uint64_t>::max();
        VanEmdeBoas tree(hash);
        tree.insert(9);
        tree.minimum_->count = limit - 1;
        tree.insert(9);
        check_overflow(tree, 9);
        assert(tree.minimum_->count == limit && !tree.summary_);

        tree.insert(1);
        const auto* cluster = tree.clusters_.find(0);
        assert(cluster && cluster->minimum_->count == limit);
        check_overflow(tree, 9);
        assert(cluster->minimum_->count == limit && tree.minimum_->count == 1);
        check_order(*tree.summary_, {0});

        VanEmdeBoas leaf(hash, 1);
        leaf.insert(0);
        leaf.insert(1);
        leaf.base_counts_[1] = limit - 1;
        leaf.insert(1);
        check_overflow(leaf, 1);
        assert(leaf.base_counts_[0] == 1 && leaf.base_counts_[1] == limit);
    }
};

int main() {
    const SimpleTabulation hash(42);
    check_collisions_and_growth(hash);
    VanEmdeBoasTest::check_layout(hash);
    VanEmdeBoasTest::check_multiplicities(hash);
    VanEmdeBoasTest::check_count_limits(hash);

    VanEmdeBoas leaf(hash, 1);
    leaf.insert(1);
    check_order(leaf, {1});
    leaf.insert(0);
    leaf.insert(1);
    leaf.insert(0);
    leaf.insert(0);
    check_order(leaf, {0, 0, 0, 1, 1});

    VanEmdeBoas odd_width(hash, 3);
    for (const auto key : {7u, 4u, 2u, 5u, 1u, 0u, 6u, 3u}) odd_width.insert(key);
    check_order(odd_width, {0, 1, 2, 3, 4, 5, 6, 7});

    std::cout << "Tabela de clusters e representação vEB verificadas\n";
}
