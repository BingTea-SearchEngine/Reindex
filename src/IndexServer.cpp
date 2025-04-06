#include "IndexServer.hpp"

IndexServer::IndexServer(int port, int maxClients)
    : _server(Server(port, maxClients)) {

}

void IndexServer::Start() {
    while (true) {
        std::vector<Message> messages = _server.GetMessagesBlocking();
        for (const auto& m : messages) {
            spdlog::info("Request from {}:{}", m.senderIp, m.senderPort);
            spdlog::info(m.msg);
        }
    }
}

int main(int argc, char** argv) {
    argparse::ArgumentParser program("indexserver");
    program.add_argument("-p", "--port")
        .default_value(8002)
        .help("Port to run server on")
        .scan<'i', int>();

    program.add_argument("-m", "--maxclients")
        .default_value(10)
        .help("Max number of clients")
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    int port = program.get<int>("-p");
    int maxClients = program.get<int>("-m");

    spdlog::info("Port {}", port);
    spdlog::info("Max clients {}", maxClients);

    IndexServer indexServer(port, maxClients);
    spdlog::info("======= Index Server Started =======");
    indexServer.Start();
}
