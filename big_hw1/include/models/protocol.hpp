#pragma once
#include <string>

namespace brute_force_node {

enum class MessageType { TASK, WAIT, DONE, UNKNOWN };

struct TaskMessage {
    std::string hash;
    std::string begin;
    std::string end;
};

MessageType parse_type(const std::string &msg) noexcept;

TaskMessage parse_task(const std::string &msg);

int parse_wait(const std::string &msg);

std::string build_hello(uint64_t hashrate);

std::string build_found(const std::string &key);

std::string build_not_found();

}  // namespace brute_force_node
