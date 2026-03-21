#include <iostream>
#include "benchmark.hpp"

void run(int threads, int ops_per_thread) {
    long long run_count = 1'000'000LL / threads / ops_per_thread;

    std::vector<BenchmarkResult> average_results =
        BenchmarkRunner(threads, ops_per_thread).run_all();

    for (int i = 0; i < run_count; ++i) {
        auto results = BenchmarkRunner(threads, ops_per_thread).run_all();
        for (int j = 0; j < average_results.size(); ++j) {
            average_results[j].seconds += results[j].seconds;
            average_results[j].ops_per_sec += results[j].ops_per_sec;
        }
    }

    for (auto &average_result : average_results) {
        average_result.seconds /= static_cast<double>(run_count) + 1;
        average_result.ops_per_sec /= static_cast<double>(run_count) + 1;
    }

    std::cout << "\n===== run_count = " << run_count + 1
              << ", thread = " << threads
              << ", ops_per_thread = " << ops_per_thread << " =====\n";
    for (const auto &result : average_results) {
        std::cout << result;
    }
}

int main() {
    std::vector<std::pair<int, int>> combs = {
        {8, 1'000'000}, {4, 1'000'000}, {4, 1'000},    {4, 10'000},
        {4, 100'000},   {4, 1'000'000}, {8, 1'000},    {8, 10'000},
        {8, 100'000},   {8, 1'000'000}, {16, 1'000},   {16, 10'000},
        {16, 100'000},  {32, 1'000},    {32, 10'000},  {32, 100'000},
        {64, 1'000},    {64, 10'000},   {64, 100'000},
    };

    for (auto [threads, ops_per_thread] : combs) {
        run(threads, ops_per_thread);
    }
}
