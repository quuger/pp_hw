#pragma once
#include <chrono>
#include <string>
#include "models/bigint.hpp"

namespace brute_force_node {

enum class MessageType { TASK, WAIT, DONE, UNKNOWN };

struct SearchTask {
    std::string hash;
    models::Bigint begin;
    models::Bigint end;
};

MessageType parse_type(const std::string &msg) noexcept;

SearchTask parse_task(const std::string &msg);

std::chrono::seconds parse_wait(const std::string &msg);

std::string build_hello(uint64_t hashrate);

std::string build_found(const std::string &key);

std::string build_not_found();

std::string build_uknown();

std::string build_ill_formed(const std::string &error = "");

}  // namespace brute_force_node
