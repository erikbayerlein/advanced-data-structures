#pragma once

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

    const SimpleTabulation& hash_;
    unsigned bits_;
    std::optional<std::uint32_t> minimum_;
    std::uint32_t maximum_ = 0;
    bool base_has_both_keys_ = false;
    std::unique_ptr<VanEmdeBoas> summary_;
    ClusterTable<VanEmdeBoas> clusters_;
};
