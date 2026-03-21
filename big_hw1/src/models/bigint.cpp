#include "models/bigint.hpp"
#include <algorithm>
#include <ranges>
#include <sstream>

namespace models {

namespace {
int index_of(char c) {
    int result = ALPHABET.find(c);
    if (result == -1) {
        throw UnknownSymbolError(c);
    }
    return result;
}
}  // namespace

UnknownSymbolError::UnknownSymbolError(char symbol)
    : std::runtime_error((std::stringstream{} << "Invalid character " << symbol
                                              << " found")
                             .str()) {
}

void Bigint::remove_leading_zeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

void Bigint::eliminate_digits_overflows() {
    digits.push_back(0);
    for (std::size_t i = 0; i < digits.size() - 1; ++i) {
        if (digits[i] < 0) {
            --digits[i + 1];
            digits[i] += BASE;
        }
        digits[i + 1] += digits[i] / BASE;
        digits[i] %= BASE;
    }
    remove_leading_zeros();
}

Bigint::Bigint() : digits{0} {
}

Bigint::Bigint(uint64_t number) {
    while (number > 0) {
        digits.push_back(static_cast<int>(number % BASE));
        number /= BASE;
    }
    digits.push_back(0);
    remove_leading_zeros();
}

Bigint::Bigint(const std::string &number) {
    if (number.empty()) {
        throw std::runtime_error("Cannot create Bigint from empty string");
    }
    for (auto c : std::ranges::reverse_view(number)) {
        digits.push_back(index_of(c));
    }
}

Bigint::operator uint64_t() const {
    uint64_t result = 0;
    for (int digit : std::ranges::reverse_view(digits)) {
        result *= BASE;
        result += digit;
    }
    return result;
}

bool operator==(const Bigint &lhs, const Bigint &rhs) {
    return lhs.digits == rhs.digits;
}

bool operator<(const Bigint &lhs, const Bigint &rhs) {
    if (lhs.digits.size() != rhs.digits.size()) {
        return lhs.digits.size() < rhs.digits.size();
    }
    std::size_t i = lhs.digits.size() - 1;
    while (i > 0 && lhs.digits[i] == rhs.digits[i]) {
        --i;
    }
    return lhs.digits[i] < rhs.digits[i];
}

bool operator<=(const Bigint &lhs, const Bigint &rhs) {
    return lhs < rhs || lhs == rhs;
}

Bigint &Bigint::operator+=(const Bigint &other) {
    while (digits.size() < other.digits.size()) {
        digits.push_back(0);
    }
    digits.push_back(0);
    for (std::size_t i = 0; i < other.digits.size(); ++i) {
        digits[i] += other.digits[i];
    }
    eliminate_digits_overflows();
    remove_leading_zeros();
    return *this;
}

Bigint &Bigint::operator-=(const Bigint &other) {
    for (std::size_t i = 0; i < other.digits.size(); ++i) {
        digits[i] -= other.digits[i];
        if (digits[i] < 0) {
            --digits[i + 1];
            digits[i] += BASE;
        }
    }
    eliminate_digits_overflows();
    remove_leading_zeros();
    return *this;
}

Bigint operator-(const Bigint &lhs, const Bigint &rhs) {
    return Bigint(lhs) -= rhs;
}

Bigint operator+(const Bigint &lhs, const Bigint &rhs) {
    return Bigint(lhs) += rhs;
}

Bigint &Bigint::operator++() {
    return *this += 1;
}

Bigint &Bigint::operator*=(const Bigint &other) {
    std::vector<int> result(digits.size() + other.digits.size(), 0);

    for (std::size_t i = 0; i < digits.size(); ++i) {
        for (std::size_t j = 0; j < other.digits.size(); ++j) {
            long long cur = result[i + j] +
                            static_cast<long long>(digits[i]) * other.digits[j];
            result[i + j] = static_cast<int>(cur % BASE);
            result[i + j + 1] += static_cast<int>(cur / BASE);
        }
    }

    digits = std::move(result);
    eliminate_digits_overflows();
    remove_leading_zeros();
    return *this;
}

Bigint operator*(const Bigint &lhs, const Bigint &rhs) {
    return Bigint(lhs) *= rhs;
}

Bigint &Bigint::operator/=(const Bigint &other) {
    const Bigint &divisor = other;
    Bigint dividend = *this;
    Bigint result;
    result.digits.assign(dividend.digits.size(), 0);

    Bigint current;

    for (std::size_t i = dividend.digits.size(); i-- > 0;) {
        current.digits.insert(current.digits.begin(), dividend.digits[i]);
        current.remove_leading_zeros();

        int x = 0;
        int left = 0;
        int right = BASE - 1;

        while (left <= right) {
            int mid = (left + right) / 2;
            Bigint t = divisor;
            t *= Bigint(mid);

            if (!(current < t)) {
                x = mid;
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        result.digits[i] = x;
        current -= divisor * Bigint(x);
    }

    result.remove_leading_zeros();
    *this = result;
    return *this;
}

Bigint operator/(const Bigint &lhs, const Bigint &rhs) {
    return Bigint(lhs) /= rhs;
}

std::string Bigint::to_string() const {
    std::string result;
    for (auto digit : digits) {
        result += ALPHABET[digit];
    }
    std::ranges::reverse(result.begin(), result.end());
    return result;
}

}  // namespace models
