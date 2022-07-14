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
        void handle_accept(const asio::error_code& ec, asio::ip::tcp::socket peer);

    private:
        asio::io_context m_io;
        asio::ip::tcp::acceptor m_proxy_server;
        asio::ip::tcp::endpoint m_server_endpoint;
        int m_protocol_version;
    };

    class minecraft_proxy_connection : public std::enable_shared_from_this<minecraft_proxy_connection> {
    public:
        minecraft_proxy_connection(asio::io_context& io, asio::ip::tcp::socket&& client_to_proxy_socket, asio::ip::tcp::endpoint m_server_endpoint);
        void start();
        void handle_connect(const asio::error_code& ec);
        void handle_read(asio::error_code& ec, size_t bytes_transferred);
        void start_transmission();
    private:
        asio::ip::tcp::socket m_client_to_proxy_socket;
        asio::ip::tcp::socket m_proxy_to_server_socket;
        asio::ip::tcp::endpoint m_server_endpoint;
        asio::mutable_buffer m_read_buffer;
    };
};