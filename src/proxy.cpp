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
        m_proxy_server.async_accept([this] (const asio::error_code& ec, asio::ip::tcp::socket peer) {
            handle_accept(ec, std::move(peer));
        });
    }

    void minecraft_proxy::handle_accept(const asio::error_code& ec, asio::ip::tcp::socket peer) {
        if (!ec) {
            auto conn = std::make_shared<minecraft_proxy_connection>(m_io, std::move(peer), m_server_endpoint);
            conn->start();
        }

        start_accept();
    }

    minecraft_proxy_connection::minecraft_proxy_connection(asio::io_context& io, asio::ip::tcp::socket proxy_to_client_socket, asio::ip::tcp::endpoint server_endpoint) 
        : m_proxy_to_client_socket(std::move(proxy_to_client_socket)),
          m_proxy_to_server_socket(io),
          m_server_endpoint(server_endpoint) {}

    void minecraft_proxy_connection::start() {
        m_proxy_to_server_socket.async_connect(m_server_endpoint, [self = shared_from_this()](const asio::error_code& ec) {
            self->handle_connect(ec);
        });
    }

    void minecraft_proxy_connection::close() {
        m_proxy_to_client_socket.close();
        m_proxy_to_server_socket.close();
    }

    void minecraft_proxy_connection::read_from_client() {
        auto buf = std::make_shared<std::vector<std::byte>>(m_buffer_size);
        m_proxy_to_client_socket.async_read_some(asio::buffer(*buf), [self = shared_from_this(), buf](const asio::error_code& ec, size_t bytes_transferred) {
            self->m_client_read_buffer.insert(self->m_client_read_buffer.end(), buf->begin(), buf->begin() + bytes_transferred);
            self->handle_read_from_client(ec, bytes_transferred);
        });
    }

    void minecraft_proxy_connection::read_from_server() {
        auto buf = std::make_shared<std::vector<std::byte>>(m_buffer_size);
        m_proxy_to_server_socket.async_read_some(asio::buffer(*buf), [self = shared_from_this(), buf](const asio::error_code& ec, size_t bytes_transferred) {
            self->m_server_read_buffer.insert(self->m_server_read_buffer.end(), buf->begin(), buf->begin() + bytes_transferred);
            self->handle_read_from_server(ec, bytes_transferred);
        });
    }

    void minecraft_proxy_connection::handle_connect(const asio::error_code& ec) {
        if (!ec) {
            std::cout << m_proxy_to_client_socket.remote_endpoint() << " <-> " << m_proxy_to_client_socket.local_endpoint() << " (Proxy) <-> " << m_proxy_to_server_socket.remote_endpoint() << " (Server)\n"; 
            read_from_client();
            read_from_server();
        }
        else {
            std::cout << "Couldn't connect proxy connection to server: " << ec.message() << '\n';
            close();
        }
    }


    void minecraft_proxy_connection::handle_read_from_client(const asio::error_code& ec, size_t) {
        if (!ec) {
            minecraft_varint packet_length;
            try {
                packet_length = minecraft_varint{m_client_read_buffer};
            }
            catch (const std::runtime_error&) {
                read_from_client();
            }

            const size_t total_packet_length = packet_length.bytes().size() + packet_length.value();
            if (m_client_read_buffer.size() < total_packet_length) {
                read_from_client();
            }

            asio::async_write(m_proxy_to_server_socket, asio::buffer(m_client_read_buffer, total_packet_length), [self = shared_from_this(), total_packet_length](const asio::error_code& ec, size_t) {
                self->read_from_server();
                self->read_from_client();
                asio::dynamic_buffer(self->m_client_read_buffer).consume(total_packet_length);
            });
        }
        else {
            std::cout << "Failed to read from client: " << ec.message() << '\n';
            close();
        }
    }

    void minecraft_proxy_connection::handle_read_from_server(const asio::error_code& ec, size_t) {
        if (!ec) {
            minecraft_varint packet_length;
            try {
                packet_length = minecraft_varint{m_server_read_buffer};
            }
            catch (const std::runtime_error&) {
                read_from_server();
            }

            const size_t total_packet_length = packet_length.bytes().size() + packet_length.value();
            if (total_packet_length < m_server_read_buffer.size()) {
                read_from_server();
            }
            
            asio::async_write(m_proxy_to_client_socket, asio::buffer(m_server_read_buffer, total_packet_length), [self = shared_from_this(), total_packet_length](const asio::error_code& ec, size_t) {
                self->read_from_client();
                self->read_from_server();
                asio::dynamic_buffer(self->m_server_read_buffer).consume(total_packet_length);
            });
        }
        else {
            std::cout << "Failed to read from server: " << ec.message() << '\n';
            close();
        }
    }
}