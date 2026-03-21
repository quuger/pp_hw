#pragma once
#include <string>
#include <vector>
#include "models/bigint.hpp"

namespace brute_force_node {

class SearchRange {
public:
    SearchRange(const std::string &begin, const std::string &end);
    SearchRange(models::Bigint begin, models::Bigint end);
    [[nodiscard]] models::Bigint size() const noexcept;
    [[nodiscard]] models::Bigint begin() const noexcept;
    [[nodiscard]] models::Bigint end() const noexcept;
    [[nodiscard]] std::vector<SearchRange> split(models::Bigint count) const;

private:
    models::Bigint begin_;
    models::Bigint end_;
};

}  // namespace brute_force_node
