#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace models {
const std::string ALPHABET =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int BASE = 62;

struct UnknownSymbolError : std::runtime_error {
    UnknownSymbolError(char symbol);
};

class Bigint {
private:
    std::vector<int> digits;  // from least significant to the highest digits

    void remove_leading_zeros();

    void eliminate_digits_overflows();

public:
    Bigint();
    Bigint(uint64_t number);
    explicit Bigint(const std::string &number);
    Bigint(const Bigint &number) = default;
    Bigint(Bigint &&number) = default;
    Bigint &operator=(const Bigint &number) = default;
    Bigint &operator=(Bigint &&number) = default;
    ~Bigint() = default;

    explicit operator uint64_t() const;

    friend bool operator==(const Bigint &lhs, const Bigint &rhs);
    friend bool operator<(const Bigint &lhs, const Bigint &rhs);
    friend bool operator<=(const Bigint &lhs, const Bigint &rhs);

    Bigint &operator+=(const Bigint &other);
    Bigint &operator-=(const Bigint &other);
    Bigint &operator*=(const Bigint &other);
    Bigint &operator/=(const Bigint &other);

    friend Bigint operator-(const Bigint &lhs, const Bigint &rhs);
    friend Bigint operator+(const Bigint &lhs, const Bigint &rhs);
    friend Bigint operator*(const Bigint &lhs, const Bigint &rhs);
    friend Bigint operator/(const Bigint &lhs, const Bigint &rhs);

    Bigint &operator++();

    [[nodiscard]] std::string to_string() const;
};

}  // namespace models
