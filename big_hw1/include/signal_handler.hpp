#pragma once

#include <atomic>
#include <condition_variable>

namespace brute_force_node::signal_handler {

std::atomic_flag &get();
std::atomic<int> &get_signal_code();
std::condition_variable &get_sleep_cond_var();

void register_socket(int fd);

void init();

}  // namespace brute_force_node::signal_handler
