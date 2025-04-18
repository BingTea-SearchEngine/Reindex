#include "IndexServer.hpp"

IndexServer::IndexServer(int port, int maxClients, std::string indexaPath,
                         MasterChunk master)
    : _server(Server(port, maxClients)), _master(master) {
    _primaryIndexChunk = _master.GetIndexChunk(0);
    _primaryMetadataChunk = _master.GetMetadataChunk(0);
}

void IndexServer::Start() {
    while (true) {
        std::vector<Message> messages = _server.GetMessagesBlocking();
        for (const auto& m : messages) {
            spdlog::info("Request from {}:{}", m.senderIp, m.senderPort);

            IndexMessage queryMsg = IndexInterface::Decode(m.msg);

            Message msg;
            msg.receiverSock = m.senderSock;
            msg.msg = IndexInterface::Encode(_handleSearch(queryMsg));
            _server.SendMessage(msg);
        }
    }
}

IndexMessage IndexServer::_handleSearch(IndexMessage msg) {
    if (msg.type != IndexMessageType::QUERY) {
        return IndexMessage{IndexMessageType::DOCUMENTS, "", {}};
    }
    spdlog::info("Query: {}", msg.query);

    // Parser query
    Parser parser(msg.query);
    Expression* expr = parser.Parse();
    // Evaluate query (Call ISRs)
    std::cout << expr->Eval() << std::endl;
    // Rank?

    std::vector<doc_t> documents;
    documents.push_back(doc_t{"https://wwww.google.com", 5, 1231, 0.4, 0.5,
                              std::vector<std::string>{"b.com", "c.com"}});
    documents.push_back(doc_t{"https://www.twitter.com", 5, 1231, 0.4, 0.5,
                              std::vector<std::string>{"b.com", "c.com"}});
    documents.push_back(doc_t{"https://www.nytimes.com", 5, 1231, 0.4, 0.5,
                              std::vector<std::string>{"b.com", "c.com"}});
    documents.push_back(doc_t{"https://www.washingtonpost.com", 5, 1231, 0.4,
                              0.5, std::vector<std::string>{"b.com", "c.com"}});
    documents.push_back(doc_t{"https://www.ft.com", 5, 1231, 0.4, 0.5,
                              std::vector<std::string>{"b.com", "c.com"}});

    return IndexMessage{IndexMessageType::DOCUMENTS, "", documents};
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

    program.add_argument("-i", "--input")
        .required()
        .help("Directory index files are located");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    int port = program.get<int>("-p");
    int maxClients = program.get<int>("-m");
    std::string indexPath = program.get<std::string>("-i");

    spdlog::info("Port {}", port);
    spdlog::info("Max clients {}", maxClients);
    spdlog::info("Index Path {}", indexPath);

    int fd = -1;
    auto [buf, size] = read_mmap_region(fd, indexPath + "masterchunk");
    size_t offset = 0;
    MasterChunk master =
        MasterChunk::Deserailize(static_cast<char*>(buf), offset);
    munmap(buf, size);
    close(fd);

    IndexServer indexServer(port, maxClients, indexPath, master);
    spdlog::info("======= Index Server Started =======");
    indexServer.Start();
}
