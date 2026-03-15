#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "signal_handler.hpp"
#include "task_manager.hpp"

namespace {
uint64_t read_hashrate(const std::string &filename) {
    uint64_t hashrate{};
    std::ifstream file(filename, std::ios_base::binary | std::ios_base::in);

    if (!file) {
        throw std::runtime_error("Cannot open hashrate file " + filename);
    }
    file.read(reinterpret_cast<char *>(&hashrate), sizeof hashrate);

    return hashrate;
}
}  // namespace

int main(int argc, char **argv) {
    brute_force_node::signal_handler::init();

    if (argc != 4) {
        std::cerr << "Usage: ./app <host> <port> <hashrate_file>\n";
        return 1;
    }

    std::string host = argv[1];
    int port = std::stoi(argv[2]);

    int result = 0;

    try {
        uint64_t hashrate = read_hashrate(argv[3]);
        brute_force_node::TaskManager manager(host, port, hashrate);
        manager.do_scan();
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
        result = 1;
    }

    if (brute_force_node::signal_handler::get().load()) {
        result = 128 + brute_force_node::signal_handler::get_signal_code();
    }

    return result;
}
