#include "van_emde_boas.hpp"

#include <algorithm>
#include <cassert>
#include <utility>

VanEmdeBoas::VanEmdeBoas(const SimpleTabulation& hash, unsigned bits)
    : hash_(hash), bits_(bits), clusters_(hash) {
    assert(bits_ >= 1 && bits_ <= 32);
}

void VanEmdeBoas::insert(std::uint32_t key) {
    assert(bits_ == 32 || (key >> bits_) == 0);
    if (!minimum_) {
        minimum_ = maximum_ = key;
        return;
    }
    if (key == *minimum_) return;

    if (bits_ == 1) {
        minimum_ = 0;
        maximum_ = 1;
        base_has_both_keys_ = true;
        return;
    }

    const unsigned low_bits = bits_ / 2;
    const std::uint32_t routed_key = std::max(key, *minimum_);
    const std::uint32_t index = routed_key >> low_bits;
    const std::uint32_t residue = routed_key & ((std::uint32_t{1} << low_bits) - 1);
    if (auto* cluster = clusters_.find(index)) {
        cluster->insert(residue);
    } else {
        auto new_cluster = std::make_unique<VanEmdeBoas>(hash_, low_bits);
        new_cluster->insert(residue);
        auto new_summary = summary_ ? nullptr : std::make_unique<VanEmdeBoas>(hash_, bits_ - low_bits);

        // Depois de atualizar o resumo, registrar o filho não pode exigir uma alocação.
        clusters_.reserve_for_insert();
        auto& summary = summary_ ? *summary_ : *new_summary;
        summary.insert(index);
        clusters_.insert(index, std::move(new_cluster));
        if (new_summary) summary_ = std::move(new_summary);
    }

    if (key < *minimum_) minimum_ = key;
    if (key > maximum_) maximum_ = key;
}

void VanEmdeBoas::for_each(const std::function<void(std::uint32_t)>& visit) const {
    if (!minimum_) return;
    visit(*minimum_);
    if (bits_ == 1) {
        if (base_has_both_keys_) visit(1);
        return;
    }
    if (!summary_) return;

    const unsigned low_bits = bits_ / 2;
    summary_->for_each([&](std::uint32_t index) {
        const auto* cluster = clusters_.find(index);
        assert(cluster);
        cluster->for_each([&](std::uint32_t residue) {
            visit((index << low_bits) | residue);
        });
    });
}
