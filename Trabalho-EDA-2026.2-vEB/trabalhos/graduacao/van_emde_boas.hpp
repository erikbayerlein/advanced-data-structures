#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

#include "cluster_table.hpp"

class VanEmdeBoas {
public:
    explicit VanEmdeBoas(const SimpleTabulation& hash, unsigned bits = 32);

    void insert(std::uint32_t key);
    void for_each(const std::function<void(std::uint32_t)>& visit) const;

private:
    friend struct VanEmdeBoasTest;

    struct Record {
        std::uint32_t key;
        std::uint64_t count;
    };

    void insert(Record record);
    void for_each_record(const std::function<void(Record)>& visit) const;

    const SimpleTabulation& hash_;
    unsigned bits_;
    std::optional<Record> minimum_;
    std::uint32_t maximum_ = 0;
    std::array<std::uint64_t, 2> base_counts_{};
    std::unique_ptr<VanEmdeBoas> summary_;
    ClusterTable<VanEmdeBoas> clusters_;
};
