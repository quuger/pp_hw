#include "task_manager.hpp"
#include <unistd.h>
#include <chrono>
#include <cstdint>
#include <thread>
#include "models/key.hpp"
#include "signal_handler.hpp"

namespace brute_force_node {

TaskManager::TaskManager(const std::string &host, int port, uint64_t hashrate)
    : hashrate_(hashrate),
      tcp_client_(host, port),
      engine_(std::thread::hardware_concurrency()) {
    tcp_client_.send_message(build_hello(hashrate_));
}

void TaskManager::do_scan() {
    while (!signal_handler::get().load()) {
        std::string msg = tcp_client_.receive_message();

        MessageType type = parse_type(msg);

        switch (type) {
            case MessageType::TASK: {
                TaskMessage task = parse_task(msg);
                process_task(task);
                break;
            }
            case MessageType::WAIT: {
                int seconds = parse_wait(msg);
                // TODO: wake up after SIGNAL
                std::this_thread::sleep_for(std::chrono::seconds(seconds));
                break;
            }
            case MessageType::DONE:
                return;
            default:
        }
    }
}

void TaskManager::process_task(const TaskMessage &task) {
    std::optional<uint64_t> found =
        engine_.search(task.hash, {task.begin, task.end});

    if (found.has_value()) {
        tcp_client_.send_message(build_found(std::string(Key(*found))));
    } else {
        tcp_client_.send_message(build_not_found());
    }
}

}  // namespace brute_force_node
