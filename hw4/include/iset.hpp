#pragma once
#include <string>

struct ISet {
    virtual bool contains(int value) = 0;
    virtual void add(int value) = 0;
    virtual void remove(int value) = 0;
    virtual std::string get_name() = 0;
    virtual ~ISet() = default;

protected:
    constexpr static int HEAD_VALUE = std::numeric_limits<int>::min();
    constexpr static int TAIL_VALUE = std::numeric_limits<int>::max();
};
