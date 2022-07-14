#include "proxy.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/connect.hpp"

#include <functional>
#include <iostream>
#include <memory>

namespace spiff {
    minecraft_proxy::minecraft_proxy(unsigned short listen_port, int protocol_version) : 
        m_proxy_server(m_io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), listen_port)),
        m_protocol_version(protocol_version) {}

    void minecraft_proxy::start(std::string_view server_endpoint) {
        const auto colon_pos = server_endpoint.find(':', 0);
        const auto& server_address = colon_pos != std::string_view::npos ? server_endpoint.substr(0, colon_pos) : server_endpoint;
        const auto server_port = colon_pos != std::string_view::npos ? server_endpoint.substr(colon_pos + 1) : "25565";

        asio::ip::tcp::resolver resolver{m_io};
        asio::async_connect(m_server_socket, resolver.resolve(server_address, server_port), std::bind(&minecraft_proxy::handle_connect, this, std::placeholders::_1, std::placeholders::_2));
        m_io.run();
    }

    void minecraft_proxy::handle_connect(const asio::error_code& ec, const asio::ip::tcp::endpoint& endpoint) { 
        if (!ec) {
            std::cout << "Proxy connection made to " << endpoint << '\n';
            std::cout << "Listening on " << m_proxy_server.local_endpoint() << "..." << '\n';
            start_accept();
        }
        else {
            throw std::runtime_error{"Failed to resolve and connect to server: " + ec.message()};
        }
    }

    void minecraft_proxy::start_accept() {
        m_proxy_server.async_accept(std::bind(&minecraft_proxy::handle_accept, this, std::placeholders::_1, std::placeholders::_2));
    }

    void minecraft_proxy::handle_accept(const asio::error_code& ec, asio::ip::tcp::socket peer) {
        if (!ec) {
            //TODO
            std::cout << "Connection: " << peer.remote_endpoint() << " <-> Proxy <-> " << m_server_socket.remote_endpoint() << '\n';
            std::shared_ptr<minecraft_proxy_connection> conn = minecraft_proxy_connection{std::move(peer)}.shared_from_this();
            conn->start();
            start_accept();
        }
        else {
            std::cout << "Accepting new connection failed: " << ec.message() << '\n';
        }
    }

    minecraft_proxy_connection::minecraft_proxy_connection(asio::ip::tcp::socket&& socket) : m_socket(std::move(socket)) {}

    void minecraft_proxy_connection::start() {

    }
}