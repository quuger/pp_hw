#pragma once

#include <atomic>
#include <future>
#include <optional>
#include "brute_force/thread_pool.hpp"
#include "models/bigint.hpp"
#include "models/search_range.hpp"

namespace brute_force_node::brute_force {

class Engine {
public:
    Engine(size_t threads);

    std::optional<std::string>
    search(const std::string &key, SearchRange range);

    void cancel() noexcept;

private:
    ThreadPool pool_;
    size_t thread_count_;
    std::atomic<bool> cancel_requested_{false};
    std::vector<std::future<void>> futures_;
    std::atomic<bool> found_{false};
    models::Bigint found_value_;

    void reset();
};

}  // namespace brute_force_node::brute_force
