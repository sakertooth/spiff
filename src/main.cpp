#include <iostream>
#include "argparse/argparse.hpp"
#include "proxy.hpp"

int main(int argc, char **argv) {
    argparse::ArgumentParser parser{"Spiff", "1.0.0"};
    
    parser.add_argument("-s", "--server")
        .help("The Minecraft server to connect to")
        .default_value(std::string{"127.0.0.1:25565"})
        .required();

    parser.add_argument("-p", "--port")
        .help("The port the proxy should listen on")
        .default_value(25566)
        .required()
        .scan<'i', int>();

    parser.add_argument("-pv", "--protocol-version")
        .help("The protocol version to use")
        .required()
        .scan<'i', int>();

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << '\n';
        std::cerr << parser;
        std::exit(1);
    }

    const auto& server_endpoint = parser.get<std::string>("--server");
    const unsigned short listen_port = parser.get<int>("--port");
    const auto& protocol_version = parser.get<int>("--protocol-version");

    spiff::minecraft_proxy proxy {listen_port, protocol_version};
    proxy.start(server_endpoint);
}
