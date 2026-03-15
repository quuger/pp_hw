#include "models/key.hpp"

namespace {

const std::string ALPHABET =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

const int BASE = 62;

std::size_t index_of(char c) {
    return ALPHABET.find(c);
}

}  // namespace

namespace brute_force_node {

Key::Key(const std::string &key) noexcept {
    for (char c : key) {
        data_ *= BASE;
        data_ += index_of(c);
    }
}

Key::Key(uint64_t key) noexcept : data_(key) {
}

Key::operator std::string() const {
    if (data_ == 0) {
        return "0";
    }

    std::string res;
    uint64_t value = data_;

    while (value > 0) {
        res = ALPHABET[value % BASE] + res;
        value /= BASE;
    }

    return res;
}

Key::operator uint64_t() const noexcept {
    return data_;
}

bool operator==(const Key &key, uint64_t other) noexcept {
    return key.data_ == other;
}

bool operator==(uint64_t lhs, const Key &rhs) noexcept {
    return rhs == lhs;
}

}  // namespace brute_force_node
