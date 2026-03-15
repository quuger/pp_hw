#include "tcp_client.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <signal_handler.hpp>
#include <stdexcept>

namespace brute_force_node {

TcpClient::TcpClient(const std::string &host, int port)
    : socket_fd_(socket(AF_INET, SOCK_STREAM, 0)), host_(host), port_(port) {
    connect_to_server(host, port);
    signal_handler::register_socket(socket_fd_);
}

void TcpClient::connect_to_server(const std::string &host, int port) const {
    if (socket_fd_ < 0) {
        throw std::runtime_error("Socket creation error");
    }

    sockaddr_in server = {};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &server.sin_addr) <= 0) {
        throw std::runtime_error(
            "Invalid address or address not supported: " + host
        );
    }

    if (connect(
            socket_fd_, reinterpret_cast<sockaddr *>(&server), sizeof(server)
        ) < 0) {
        std::string error = std::strerror(errno);
        throw std::runtime_error("Connection error: " + error);
    }
}

void TcpClient::send_message(const std::string &msg) const {
    if (signal_handler::get().load()) {
        throw std::runtime_error(
            "Shutdown in progress, so unable to send message \"" + msg + "\""
        );
    }

    std::string m = msg + "\n";
    send(socket_fd_, m.c_str(), m.size(), 0);
}

std::string TcpClient::receive_message() const {
    if (signal_handler::get().load()) {
        throw std::runtime_error(
            "Shutdown in progress, so unable to recieve message"
        );
    }

    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        return "";
    }

    return std::string(buffer, bytes);
}

TcpClient::~TcpClient() {
    close(socket_fd_);
}

}  // namespace brute_force_node
