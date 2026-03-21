#include "signal_handler.hpp"
#include <sys/socket.h>
#include <condition_variable>
#include <csignal>

namespace {
std::atomic<int> &socket() {
    static std::atomic<int> socket;
    return socket;
}

void handler(int signal) {
    brute_force_node::signal_handler::get_signal_code().store(signal);

    brute_force_node::signal_handler::get().test_and_set();

    int fd = socket().load();

    if (fd != -1) {
        shutdown(fd, SHUT_RDWR);
    }

    brute_force_node::signal_handler::get_sleep_cond_var().notify_all();
}

}  // namespace

namespace brute_force_node::signal_handler {

std::atomic_flag &get() {
    static std::atomic_flag stop;
    return stop;
}

std::atomic<int> &get_signal_code() {
    static std::atomic<int> code;
    return code;
}

std::condition_variable &get_sleep_cond_var() {
    static std::condition_variable cond_var;
    return cond_var;
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
