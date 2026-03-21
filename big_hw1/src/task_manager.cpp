#include "task_manager.hpp"
#include <unistd.h>
#include <iostream>
#include <thread>
#include "models/bigint.hpp"
#include "models/protocol.hpp"
#include "models/search_range.hpp"
#include "signal_handler.hpp"

namespace brute_force_node {

namespace {

void interruptible_sleep(std::chrono::seconds duration) {
    std::mutex m;
    std::unique_lock lock(m);

    signal_handler::get_sleep_cond_var().wait_for(lock, duration, [] {
        return brute_force_node::signal_handler::get().test();
    });
}

}  // namespace

TaskManager::TaskManager(const std::string &host, int port, uint64_t hashrate)
    : hashrate_(hashrate),
      tcp_client_(host, port),
      engine_(std::thread::hardware_concurrency()) {
    tcp_client_.send_message(build_hello(hashrate_));
}

void TaskManager::do_scan() {
    while (!signal_handler::get().test()) {
        std::string msg = tcp_client_.receive_message();
        try {
            MessageType type = parse_type(msg);

            switch (type) {
                case MessageType::TASK: {
                    SearchTask task = parse_task(msg);
                    process_task(std::move(task));
                    break;
                }
                case MessageType::WAIT: {
                    auto seconds = parse_wait(msg);
                    interruptible_sleep(seconds);
                    break;
                }
                case MessageType::DONE:
                    return;
                default:
                    tcp_client_.send_message(build_uknown());
            }
        } catch (std::ios_base::failure &) {
            tcp_client_.send_message(build_ill_formed());
        } catch (models::UnknownSymbolError &e) {
            tcp_client_.send_message(build_ill_formed(e.what()));
        }
    }
}

void TaskManager::process_task(SearchTask task) {
    std::optional<std::string> found =
        engine_.search(task.hash, {std::move(task.begin), std::move(task.end)});

    if (found.has_value()) {
        tcp_client_.send_message(build_found(*found));
    } else {
        tcp_client_.send_message(build_not_found());
    }
}

}  // namespace brute_force_node
