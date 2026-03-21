#include "models/search_range.hpp"
#include <cstdint>
#include <sstream>
#include <vector>

namespace {
using models::Bigint;

void ensure_borders(const Bigint &begin, const Bigint &end) {
    if (end < begin) {
        throw std::runtime_error((std::stringstream{}
                                  << "begin of the range is greater than end: "
                                  << begin.to_string() << " > "
                                  << end.to_string())
                                     .str());
    }
}
}  // namespace

namespace brute_force_node {

SearchRange::SearchRange(const std::string &begin, const std::string &end)
    : begin_(Bigint(begin)), end_(Bigint(end)) {
    ensure_borders(begin_, end_);
}

SearchRange::SearchRange(Bigint begin, Bigint end)
    : begin_(std::move(begin)), end_(std::move(end)) {
    ensure_borders(begin_, end_);
}

Bigint SearchRange::begin() const noexcept {
    return begin_;
}

Bigint SearchRange::end() const noexcept {
    return end_;
}

Bigint SearchRange::size() const noexcept {
    return end_ - begin_ + Bigint(1U);
}

std::vector<SearchRange> SearchRange::split(Bigint count) const {
    std::vector<SearchRange> result;

    Bigint chunk = size() / count;
    if (chunk == 0) {
        chunk = 1;
        count = size();
    }

    result.reserve(static_cast<uint64_t>(count));
    for (int i = 0; i < count; i++) {
        Bigint begin = begin_ + (i * chunk);
        Bigint end = (i == count - 1) ? end_ : begin + chunk - 1;
        result.emplace_back(begin, end);
    }
    return result;
}

}  // namespace brute_force_node
