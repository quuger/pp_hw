#pragma once
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "iset.hpp"

enum class Operation { Add, Contains };

struct BenchmarkResult {
    std::string name;
    double seconds;
    double ops_per_sec;

    friend std::ostream &
    operator<<(std::ostream &os, const BenchmarkResult &r) {
        const int NAME_WIDTH = 20;
        const int TIME_WIDTH = 12;
        const int OPS_WIDTH = 10;

        os << std::left << std::setw(NAME_WIDTH) << r.name
           << " | time: " << std::right << std::setw(TIME_WIDTH) << std::fixed
           << std::setprecision(7) << r.seconds << " sec"
           << " | ops/sec: " << std::right << std::setw(OPS_WIDTH) << std::fixed
           << std::setprecision(1) << r.ops_per_sec << "\n";

        return os;
    }
};

class BenchmarkRunner {
public:
    BenchmarkRunner(size_t threads, size_t ops_per_thread);

    std::vector<BenchmarkResult> run_all();

private:
    size_t num_threads;
    size_t ops_per_thread;
    int add_percent = 20;

    std::vector<std::vector<std::pair<Operation, int>>> operations;

    void generate_operations();

    BenchmarkResult run_single(ISet &set);
};