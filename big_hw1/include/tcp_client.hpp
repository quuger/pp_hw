#pragma once
#include <string>

namespace brute_force_node {

class TcpClient {
public:
    TcpClient(const std::string &host, int port);
    ~TcpClient();
    TcpClient &operator=(const TcpClient &) = delete;
    TcpClient &operator=(TcpClient &&) = delete;
    TcpClient(const TcpClient &) = delete;
    TcpClient(TcpClient &&) = delete;

    void send_message(const std::string &msg) const;

    [[nodiscard]] std::string receive_message() const;

private:
    void connect_to_server(const std::string &host, int port) const;

    int socket_fd_;
    const std::string &host_;
    const int port_;
};

}  // namespace brute_force_node
