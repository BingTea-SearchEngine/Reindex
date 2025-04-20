#include "IndexServer.hpp"

IndexServer::IndexServer(int port, int maxClients, std::string indexaPath, std::string htmlPath,
                         MasterChunk master)
    : _server(Server(port, maxClients)), _htmlDir(htmlPath), _master(master) {
    _primaryIndexChunk = _master.GetIndexChunk(0);
    _primaryMetadataChunk = _master.GetMetadataChunk(0);
}

std::string IndexServer::getSnippet(metadata_t docMetadata, uint32_t matchOffset, int delta) {
    std::string filePath = _htmlDir + std::to_string(docMetadata.docNum) + ".parsed";
    std::ifstream file(filePath);
    if (!file) {
        return "";
    }

    int docRelativeMatchOffset = matchOffset - docMetadata.docStartOffset;
    assert(docRelativeMatchOffset >= 0);

    int snippetStartOffset = std::min(docRelativeMatchOffset - delta, 0);

    std::string snippet;
    std::string line;
    std::getline(file, line); // URL and Doc num line
    std::getline(file, line); // get <title> tag
    std::getline(file, line); // get titles
    
    std::istringstream titleIss(line);
    std::string word;
    uint32_t offset = 0;
    bool snippetFound = false;
    while (titleIss >> word) {
        if (offset == snippetStartOffset) {
            snippetFound = true;
            break;
        }
        ++offset;
    }

    return snippet;
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
            spdlog::info("Sending back response to {}:{}", m.senderIp, m.senderPort);
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

    cout << _primaryIndexChunk.GetAllPostingLists().size() << endl;
    Parser parser(msg.query, _primaryIndexChunk.GetAllPostingLists());
    Expression* expr = parser.Parse();
    // Evaluate query (Call ISRs)
    std::cout << expr->Eval() << std::endl;
    // std::cout << expr->GetString() << std::endl;
    // Rank?

    std::vector<doc_t> documents;
    documents.push_back(doc_t{"https://wwww.google.com", 5, 1231, 4, 0.4, 0.5, "This is google.com"});
    documents.push_back(doc_t{"https://www.twitter.com", 5, 1231, 5, 0.4, 0.5, "This is twitter.com"});
    documents.push_back(doc_t{"https://www.nytimes.com", 5, 1231, 6, 0.4, 0.5, "this is nytimes.com"});
    documents.push_back(
        doc_t{"https://www.washingtonpost.com", 5, 1231, 7, 0.4, 0.5, "This is washintongpost.com"});
    documents.push_back(doc_t{"https://www.ft.com", 5, 1231, 5, 0.4, 0.5, "This is ft.com"});

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

    program.add_argument("-h", "--htmlinput")
        .required()
        .help("Directory html files are located");

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
    std::string htmlPath = program.get<std::string>("-h");

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

    IndexServer indexServer(port, maxClients, indexPath, htmlPath, master);
    spdlog::info("======= Index Server Started =======");
    indexServer.Start();
}
