#include "models/protocol.hpp"
#include <ios>
#include <map>
#include <sstream>
#include "models/bigint.hpp"

namespace brute_force_node {

namespace {
const std::map<std::string, MessageType> MESSAGE_TYPE_TO_ENUM = {
    {"TASK", MessageType::TASK},
    {"WAIT", MessageType::WAIT},
    {"DONE", MessageType::DONE}
};
}

MessageType parse_type(const std::string &msg) noexcept {
    std::stringstream ss(msg);
    ss.exceptions(std::ios_base::failbit | std::ios_base::badbit);

    std::string cmd;
    ss >> cmd;

    if (auto it = MESSAGE_TYPE_TO_ENUM.find(cmd);
        it != MESSAGE_TYPE_TO_ENUM.end()) {
        return it->second;
    }

    return MessageType::UNKNOWN;
}

SearchTask parse_task(const std::string &msg) {
    std::stringstream ss(msg);
    ss.exceptions(std::ios_base::failbit | std::ios_base::badbit);

    std::string cmd;
    std::string hash;
    std::string begin;
    std::string end;

    ss >> cmd >> hash >> begin >> end;

    return {
        .hash = hash, .begin = models::Bigint(begin), .end = models::Bigint(end)
    };
}

std::chrono::seconds parse_wait(const std::string &msg) {
    std::stringstream ss(msg);
    ss.exceptions(std::ios_base::failbit | std::ios_base::badbit);

    std::string cmd;
    int sec{};

    ss >> cmd >> sec;

    return std::chrono::seconds(sec);
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

std::string build_uknown() {
    return "UNKNOWN_COMMAND";
}

std::string build_ill_formed(const std::string &error) {
    constexpr static std::string prefix = "COMMAND_IS_ILL_FORMED";
    if (error.empty()) {
        return prefix;
    }
    return prefix + ": " + error;
}

}  // namespace brute_force_node
