#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stop_token>
#include <thread>
#include <vector>

namespace brute_force_node::brute_force {

class ThreadPool {
public:
    using Task = std::function<void(std::stop_token)>;

    explicit ThreadPool(size_t threads);
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ~ThreadPool();

    void enqueue(Task task) noexcept;

private:
    std::vector<std::jthread> workers;
    std::queue<Task> tasks;

    std::mutex mutex;
    std::condition_variable cv;

    bool stopping = false;

    void worker_loop(std::stop_token stoken);
};

}  // namespace brute_force_node::brute_force
