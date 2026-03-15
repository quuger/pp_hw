#include "brute_force/thread_pool.hpp"

namespace brute_force_node::brute_force {

ThreadPool::ThreadPool(size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this](std::stop_token stoken) {
            worker_loop(stoken);
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock lock(mutex);
        stopping = true;
    }

    cv.notify_all();

    for (auto &t : workers) {
        t.request_stop();
    }
}

void ThreadPool::enqueue(Task task) noexcept {
    {
        std::unique_lock lock(mutex);
        tasks.push(std::move(task));
    }

    cv.notify_one();
}

void ThreadPool::worker_loop(std::stop_token stoken) {
    while (true) {
        Task task;

        {
            std::unique_lock lock(mutex);

            cv.wait(lock, [&] {
                return stopping || !tasks.empty() || stoken.stop_requested();
            });

            if (stopping || stoken.stop_requested()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        task(stoken);
    }
}

}  // namespace brute_force_node::brute_force
