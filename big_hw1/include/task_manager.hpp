#pragma once

#include <cstdint>
#include "brute_force/engine.hpp"
#include "models/protocol.hpp"
#include "tcp_client.hpp"

namespace brute_force_node {

class TaskManager {
public:
    TaskManager(const std::string &host, int port, uint64_t hashrate);

    void do_scan();

private:
    void process_task(SearchTask task);

    TcpClient tcp_client_;
    brute_force::Engine engine_;
    uint64_t hashrate_;
};

}  // namespace brute_force_node
