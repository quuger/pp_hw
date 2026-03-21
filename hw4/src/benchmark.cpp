#include "benchmark.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include "fine_grained.hpp"
#include "grained.hpp"
#include "lazy.hpp"
#include "optimistic.hpp"

BenchmarkRunner::BenchmarkRunner(size_t threads, size_t ops_per_thread)
    : num_threads(threads), ops_per_thread(ops_per_thread) {
    generate_operations();
}

void BenchmarkRunner::generate_operations() {
    operations.resize(num_threads);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> operation_dist(0, 99);
    std::uniform_int_distribution<> value_dist(0, 10000);

    for (size_t t = 0; t < num_threads; ++t) {
        operations[t].reserve(ops_per_thread);

        for (size_t i = 0; i < ops_per_thread; ++i) {
            operations[t].emplace_back(
                (operation_dist(gen) < add_percent ? Operation::Add
                                                   : Operation::Contains),
                value_dist(gen)
            );
        }
    }
}

BenchmarkResult BenchmarkRunner::run_single(ISet &set) {
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            for (auto [op, value] : operations[t]) {
                if (op == Operation::Add) {
                    set.add(value);
                } else {
                    set.contains(value);
                }
            }
        });
    }

    for (auto &th : threads) {
        th.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    double seconds = std::chrono::duration<double>(end - start).count();
    double ops_per_sec = static_cast<double>(ops_per_thread) / seconds;

    return {
        .name = set.get_name(), .seconds = seconds, .ops_per_sec = ops_per_sec
    };
}

std::vector<BenchmarkResult> BenchmarkRunner::run_all() {
    std::vector<BenchmarkResult> results;
    auto grained = GrainedSet();
    auto fine_grained = FineGrainedSet();
    auto optimistic = OptimisticSet();
    auto lazy = LazySet();

    results.push_back(run_single(std::ref(grained)));
    results.push_back(run_single(std::ref(fine_grained)));
    results.push_back(run_single(std::ref(optimistic)));
    results.push_back(run_single(std::ref(lazy)));

    return results;

    std::cout << run_single(std::ref(grained));
    std::cout << run_single(std::ref(fine_grained));
    std::cout << run_single(std::ref(optimistic));
    std::cout << run_single(std::ref(lazy));
}
