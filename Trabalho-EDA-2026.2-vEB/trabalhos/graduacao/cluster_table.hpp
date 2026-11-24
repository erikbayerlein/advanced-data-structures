#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <utility>
#include <vector>

class SimpleTabulation {
public:
    explicit SimpleTabulation(std::uint64_t seed) {
        std::mt19937_64 random(seed);
        for (auto& table : tables_) {
            for (auto& value : table) value = random();
        }
    }

    std::uint64_t operator()(std::uint32_t key) const noexcept {
        std::uint64_t value = 0;
        for (const auto& table : tables_) {
            value ^= table[key & 0xffu];
            key >>= 8;
        }
        return value;
    }

private:
    std::array<std::array<std::uint64_t, 256>, 4> tables_;
};

template <typename Child>
class ClusterTable {
public:
    explicit ClusterTable(const SimpleTabulation& hash) : hash_(hash) {}

    const Child* find(std::uint32_t index) const noexcept {
        if (slots_.empty()) return nullptr;
        return slots_[find_slot(slots_, index)].child.get();
    }

    Child* find(std::uint32_t index) noexcept {
        return const_cast<Child*>(std::as_const(*this).find(index));
    }

    void reserve_for_insert() {
        if (size_ == slots_.size() / 2) grow();
    }

    void insert(std::uint32_t index, std::unique_ptr<Child> child) {
        reserve_for_insert();
        auto& slot = slots_[find_slot(slots_, index)];
        assert(child && !slot.child);
        slot = {index, std::move(child)};
        ++size_;
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return slots_.size(); }

private:
    struct Entry {
        std::uint32_t index = 0;
        std::unique_ptr<Child> child;
    };

    std::size_t find_slot(const std::vector<Entry>& slots, std::uint32_t index) const noexcept {
        const std::size_t mask = slots.size() - 1;
        std::size_t position = static_cast<std::size_t>(hash_(index)) & mask;
        while (slots[position].child && slots[position].index != index) {
            position = (position + 1) & mask;
        }
        return position;
    }

    void grow() {
        std::vector<Entry> enlarged(slots_.empty() ? 8 : 2 * slots_.size());
        for (auto& slot : slots_) {
            if (slot.child) enlarged[find_slot(enlarged, slot.index)] = std::move(slot);
        }
        slots_.swap(enlarged);
    }

    const SimpleTabulation& hash_;
    std::vector<Entry> slots_;
    std::size_t size_ = 0;
};
