#pragma once

#include <string_view>
#include "asio/awaitable.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

namespace spiff {
    class minecraft_proxy {
    public:
        minecraft_proxy(unsigned short listen_port, int protocol_version);

        void start(std::string_view server_endpoint);
        void start_accept();
        void handle_connect(const asio::error_code& ec, const asio::ip::tcp::endpoint& endpoint);
        void handle_accept(const asio::error_code& ec, asio::ip::tcp::socket peer);

    private:
        asio::io_context m_io;
        asio::ip::tcp::socket m_server_socket {m_io};
        asio::ip::tcp::acceptor m_proxy_server;
        int m_protocol_version;
    };

    class minecraft_proxy_connection : public std::enable_shared_from_this<minecraft_proxy_connection> {
    public:
        minecraft_proxy_connection(asio::ip::tcp::socket&& socket);
        void start();
    private:
        asio::ip::tcp::socket m_socket;
    };
};