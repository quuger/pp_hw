#include "signal_handler.hpp"
#include <sys/socket.h>
#include <csignal>
#include <iostream>

namespace {
std::atomic<int> &socket() {
    static std::atomic<int> socket;
    return socket;
}

void handler(int signal) {
    brute_force_node::signal_handler::get_signal_code().store(signal);

    brute_force_node::signal_handler::get().store(true);

    int fd = socket().load();

    if (fd != -1) {
        shutdown(fd, SHUT_RDWR);
    }
}

}  // namespace

namespace brute_force_node::signal_handler {

std::atomic<bool> &get() {
    static std::atomic<bool> stop;
    return stop;
}

std::atomic<int> &get_signal_code() {
    static std::atomic<int> code;
    return code;
}

void register_socket(int fd) {
    socket().store(fd);
}

void init() {
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGQUIT, handler);
}

}  // namespace brute_force_node::signal_handler
