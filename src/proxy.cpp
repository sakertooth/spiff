#include "proxy.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/connect.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"

#include <functional>
#include <iostream>
#include <memory>

namespace spiff {
    minecraft_proxy::minecraft_proxy(unsigned short listen_port, int protocol_version) : 
        m_proxy_server(m_io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), listen_port)),
        m_protocol_version(protocol_version) {}

    void minecraft_proxy::start(std::string_view server_endpoint) {
        const auto colon_pos = server_endpoint.find(':', 0);
        const auto server_address = colon_pos != std::string_view::npos ? server_endpoint.substr(0, colon_pos) : server_endpoint;
        const auto server_port = colon_pos != std::string_view::npos ? server_endpoint.substr(colon_pos + 1) : "25565";

        asio::ip::tcp::resolver resolver{m_io};
        auto resolved = resolver.resolve(server_address, server_port);

        if (resolved.size() == 0) {
            std::cerr << "Failed to resolve hostname: " << server_endpoint << '\n';
            return;
        }

        m_server_endpoint = *resolved.begin();

        std::cout << "Resolved hostname to " << m_server_endpoint << '\n';
        std::cout << "Listening on " << m_proxy_server.local_endpoint() << "...\n";
        start_accept();

        m_io.run();
    }

    void minecraft_proxy::start_accept() {
        m_proxy_server.async_accept(std::bind(&minecraft_proxy::handle_accept, this, std::placeholders::_1, std::placeholders::_2));
    }

    void minecraft_proxy::handle_accept(const asio::error_code& ec, asio::ip::tcp::socket peer) {
        if (!ec) {
            std::cout << "Connection made to proxy: " << peer.remote_endpoint() << '\n';
            auto conn = std::make_shared<minecraft_proxy_connection>(m_io, std::move(peer), m_server_endpoint);
            conn->start();
        }

        start_accept();
    }

    minecraft_proxy_connection::minecraft_proxy_connection(asio::io_context& io, asio::ip::tcp::socket&& client_to_proxy_socket, asio::ip::tcp::endpoint server_endpoint) 
        : m_client_to_proxy_socket(std::move(client_to_proxy_socket)),
          m_proxy_to_server_socket(io),
          m_server_endpoint(server_endpoint) {}

    void minecraft_proxy_connection::start() {
        m_proxy_to_server_socket.async_connect(m_server_endpoint, std::bind(&minecraft_proxy_connection::handle_connect, shared_from_this(), std::placeholders::_1));
    }

    void minecraft_proxy_connection::handle_connect(const asio::error_code& ec) {
        if (!ec) {
            start_transmission();
        }
    }

    void minecraft_proxy_connection::start_transmission() {
        
    }

}