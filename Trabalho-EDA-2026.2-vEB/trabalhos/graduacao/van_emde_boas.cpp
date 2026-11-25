#include "van_emde_boas.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {

void add_occurrences(std::uint64_t& count, std::uint64_t added) {
    if (added > std::numeric_limits<std::uint64_t>::max() - count) {
        throw std::overflow_error("limite de ocorrências excedido");
    }
    count += added;
}

}  // namespace

VanEmdeBoas::VanEmdeBoas(const SimpleTabulation& hash, unsigned bits)
    : hash_(hash), bits_(bits), clusters_(hash) {
    assert(bits_ >= 1 && bits_ <= 32);
}

void VanEmdeBoas::insert(std::uint32_t key) {
    insert(Record{key, 1});
}

void VanEmdeBoas::insert(Record record) {
    assert(bits_ == 32 || (record.key >> bits_) == 0);
    if (bits_ == 1) {
        add_occurrences(base_counts_[record.key], record.count);
        maximum_ = std::max(maximum_, record.key);
        return;
    }

    if (!minimum_) {
        minimum_ = record;
        maximum_ = record.key;
        return;
    }
    if (record.key == minimum_->key) {
        add_occurrences(minimum_->count, record.count);
        return;
    }

    const bool replaces_minimum = record.key < minimum_->key;
    Record routed = replaces_minimum ? *minimum_ : record;
    const unsigned low_bits = bits_ / 2;
    const std::uint32_t index = routed.key >> low_bits;
    routed.key &= (std::uint32_t{1} << low_bits) - 1;
    if (auto* cluster = clusters_.find(index)) {
        cluster->insert(routed);
    } else {
        auto new_cluster = std::make_unique<VanEmdeBoas>(hash_, low_bits);
        new_cluster->insert(routed);
        auto new_summary = summary_ ? nullptr : std::make_unique<VanEmdeBoas>(hash_, bits_ - low_bits);

        // Depois de atualizar o resumo, registrar o filho não pode exigir uma alocação.
        clusters_.reserve_for_insert();
        auto& summary = summary_ ? *summary_ : *new_summary;
        summary.insert(index);
        clusters_.insert(index, std::move(new_cluster));
        if (new_summary) summary_ = std::move(new_summary);
    }

    if (replaces_minimum) minimum_ = record;
    maximum_ = std::max(maximum_, record.key);
}

void VanEmdeBoas::for_each(const std::function<void(std::uint32_t)>& visit) const {
    for_each_record([&](Record record) {
        for (; record.count != 0; --record.count) visit(record.key);
    });
}

void VanEmdeBoas::for_each_record(const std::function<void(Record)>& visit) const {
    if (bits_ == 1) {
        for (std::uint32_t key = 0; key < base_counts_.size(); ++key) {
            if (base_counts_[key] != 0) visit(Record{key, base_counts_[key]});
        }
        return;
    }
    if (!minimum_) return;
    visit(*minimum_);
    if (!summary_) return;

    const unsigned low_bits = bits_ / 2;
    summary_->for_each_record([&](Record index) {
        assert(index.count == 1);
        const auto* cluster = clusters_.find(index.key);
        assert(cluster);
        const std::uint32_t prefix = index.key << low_bits;
        cluster->for_each_record([&](Record record) {
            record.key |= prefix;
            visit(record);
        });
    });
}
