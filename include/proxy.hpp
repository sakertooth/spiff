#pragma once

#include <string_view>
#include "asio/awaitable.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/streambuf.hpp"
#include "asio/buffers_iterator.hpp"

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
        minecraft_proxy_connection(asio::io_context& io, asio::ip::tcp::socket client_to_proxy_socket, asio::ip::tcp::endpoint m_server_endpoint);
        
        void start();
        void close();

        void read_from_server();
        void read_from_client();

        void handle_connect(const asio::error_code& ec);
        void handle_read_from_client(const asio::error_code& ec, size_t bytes_transferred);
        void handle_read_from_server(const asio::error_code& ec, size_t bytes_transferred);
    private:
        asio::ip::tcp::socket m_proxy_to_client_socket;
        asio::ip::tcp::socket m_proxy_to_server_socket;
        asio::ip::tcp::endpoint m_server_endpoint;
        std::vector<std::byte> m_client_read_buffer;
        std::vector<std::byte> m_server_read_buffer;
        const size_t m_buffer_size = 1024;
    };
};