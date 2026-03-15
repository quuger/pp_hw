#include "models/protocol.hpp"
#include <sstream>

namespace brute_force_node {

MessageType parse_type(const std::string &msg) noexcept {
    if (msg.starts_with("TASK")) {
        return MessageType::TASK;
    }
    if (msg.starts_with("WAIT")) {
        return MessageType::WAIT;
    }
    if (msg.starts_with("DONE")) {
        return MessageType::DONE;
    }

    return MessageType::UNKNOWN;
}

TaskMessage parse_task(const std::string &msg) {
    TaskMessage task;

    std::stringstream ss(msg);

    std::string cmd;

    ss >> cmd >> task.hash >> task.begin >> task.end;

    return task;
}

int parse_wait(const std::string &msg) {
    std::stringstream ss(msg);

    std::string cmd;
    int sec{};

    ss >> cmd >> sec;

    return sec;
}

std::string build_hello(uint64_t hashrate) {
    return "HELLO " + std::to_string(hashrate);
}

std::string build_found(const std::string &key) {
    return "FOUND " + key;
}

std::string build_not_found() {
    return "NOT_FOUND";
}

}  // namespace brute_force_node
