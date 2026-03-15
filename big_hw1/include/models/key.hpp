#pragma once

#include <cstdint>
#include <string>

namespace brute_force_node {

struct Key {
public:
    Key(const std::string &key) noexcept;
    Key(uint64_t key) noexcept;

    explicit operator std::string() const;
    explicit operator uint64_t() const noexcept;

    friend bool operator==(const Key &key, uint64_t other) noexcept;
    friend bool operator==(uint64_t lhs, const Key &rhs) noexcept;

private:
    uint64_t data_{};
};

}  // namespace brute_force_node
