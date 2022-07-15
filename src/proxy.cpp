#include "proxy.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/connect.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"
#include "varint.hpp"

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
            read_from_client();
        }
    }

    void minecraft_proxy_connection::read_from_client() {
        asio::async_read(m_client_to_proxy_socket, asio::dynamic_buffer(m_client_read_buffer), asio::transfer_at_least(1), std::bind(&minecraft_proxy_connection::handle_read_from_client, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void minecraft_proxy_connection::handle_read_from_client(const asio::error_code& ec, size_t bytes_transferred) {
        if (!ec) {
            minecraft_varint packet_length;
            try {
                packet_length = minecraft_varint{m_client_read_buffer};
            }
            catch (const std::runtime_error& ec) {
                read_from_client();
            }

            auto total_packet_length = packet_length.bytes().size() + packet_length.value();
            if (total_packet_length < m_client_read_buffer.size()) {
                asio::async_read(m_client_to_proxy_socket, asio::dynamic_buffer(m_client_read_buffer), asio::transfer_exactly(total_packet_length - m_client_read_buffer.size()), [self = shared_from_this()] (const asio::error_code& ec, size_t bytes_transferred) {
                    self->handle_read_from_client(ec, bytes_transferred);
                });
            }

            auto packet = asio::dynamic_buffer(m_client_read_buffer, total_packet_length);
            
            //TODO: Dissect and forward packet...
            asio::dynamic_buffer(m_client_read_buffer).consume(total_packet_length);

            if (m_client_read_buffer.size() == 0) {
                read_from_client();
            }
        }
    }

    void minecraft_proxy_connection::read_from_server() {
        
    }

    void minecraft_proxy_connection::handle_read_from_server(const asio::error_code& ec, size_t bytes_transferred) {

    }
}