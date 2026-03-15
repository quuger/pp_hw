#include "models/search_range.hpp"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include "models/key.hpp"

namespace {
void ensure_borders(uint64_t begin, uint64_t end) {
    if (begin > end) {
        throw std::runtime_error((std::stringstream{}
                                  << "begin of the range is greater than end: "
                                  << begin << " > " << end)
                                     .str());
    }
}
}  // namespace

namespace brute_force_node {

SearchRange::SearchRange(const std::string &begin, const std::string &end)
    : begin_(Key(begin)), end_(Key(end)) {
    ensure_borders(begin_, end_);
}

SearchRange::SearchRange(uint64_t begin, uint64_t end)
    : begin_(begin), end_(end) {
    ensure_borders(begin_, end_);
}

uint64_t SearchRange::begin() const noexcept {
    return begin_;
}

uint64_t SearchRange::end() const noexcept {
    return end_;
}

uint64_t SearchRange::size() const noexcept {
    return end_ - begin_ + 1;
}

std::vector<SearchRange> SearchRange::split(uint64_t count) const {
    std::vector<SearchRange> result;

    uint64_t chunk = size() / count;
    if (chunk == 0) {
        chunk = 1;
        count = size();
    }

    result.reserve(count);
    for (int i = 0; i < count; i++) {
        uint64_t begin = begin_ + (i * chunk);
        uint64_t end = (i == count - 1) ? end_ : begin + chunk - 1;
        result.emplace_back(begin, end);
    }

    return result;
}

}  // namespace brute_force_node
