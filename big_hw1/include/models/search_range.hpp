#pragma once
#include <string>
#include <vector>

namespace brute_force_node {

class SearchRange {
public:
    SearchRange(const std::string &begin, const std::string &end);
    SearchRange(uint64_t begin, uint64_t end);
    [[nodiscard]] uint64_t size() const noexcept;
    [[nodiscard]] uint64_t begin() const noexcept;
    [[nodiscard]] uint64_t end() const noexcept;
    [[nodiscard]] std::vector<SearchRange> split(uint64_t count) const;

private:
    uint64_t begin_;
    uint64_t end_;
};

}  // namespace brute_force_node
