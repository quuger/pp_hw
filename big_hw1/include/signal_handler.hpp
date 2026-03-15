#pragma once

#include <atomic>

namespace brute_force_node::signal_handler {

std::atomic<bool> &get();
std::atomic<int> &get_signal_code();

void register_socket(int fd);

void init();

}  // namespace brute_force_node::signal_handler
