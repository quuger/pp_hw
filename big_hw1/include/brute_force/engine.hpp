#pragma once

#include <atomic>
#include <future>
#include <optional>
#include "brute_force/thread_pool.hpp"
#include "models/key.hpp"
#include "models/search_range.hpp"

namespace brute_force_node::brute_force {

class Engine {
public:
    Engine(size_t threads);

    std::optional<uint64_t> search(Key key, SearchRange range);

    void cancel() noexcept;

private:
    ThreadPool pool_;
    size_t thread_count_;
    std::atomic<bool> cancel_requested_{false};
    std::vector<std::future<void>> futures_;
    std::atomic<bool> found_{false};
    std::atomic<uint64_t> found_value_{0};
};

}  // namespace brute_force_node::brute_force
