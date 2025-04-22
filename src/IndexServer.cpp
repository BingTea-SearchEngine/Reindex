#include "IndexServer.hpp"

IndexServer::IndexServer(int port, int maxClients, std::string indexaPath, std::string htmlPath,
                         int matchCount, int waitTime, MasterChunk master)
    : _server(Server(port, maxClients)),
      _htmlDir(htmlPath),
      _matchCount(matchCount),
      _waitTimeMS(waitTime),
      _master(master) {
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
    search_results docs = findDocuments(msg.query, _matchCount, _waitTimeMS);
    spdlog::info("Got {} results", docs.size());

    // Some kind of ranking
    rank(docs);

    std::vector<doc_t> results;
    for (size_t i = 0; i < std::min(docs.size(), (size_t)10); ++i) {
        search_result_t result = docs[i];
        cout << result << endl;
        auto [title, snippet] = getTitleAndSnippet(result, 10);
        results.push_back(doc_t{result.url, result.numWords, result.numTitleWords,
                                result.numOutLinks, result.numTitleMatch, result.numBodyMatch,
                                result.pageRank, result.cheiRank, result.rankingScore, snippet,
                                title});
    }

    return IndexMessage{IndexMessageType::DOCUMENTS, "", results};
}

search_results IndexServer::findDocuments(std::string query, int matchCount, int timeUpperLimitMs) {
    search_results docs;
    const IndexChunk* currIndexChunk = nullptr;
    const MetadataChunk* currMetadataChunk = nullptr;

    std::unique_ptr<IndexChunk> tempIndexChunk;
    std::unique_ptr<MetadataChunk> tempMetadataChunk;

    size_t chunkIndex = 0;
    size_t numChunks = _master.NumChunks();

    auto startTime = std::chrono::steady_clock::now();
    while (docs.size() < matchCount && chunkIndex < numChunks) {
        spdlog::info("Checking chunk index {}", chunkIndex);
        if (chunkIndex == 0) {
            currIndexChunk = _primaryIndexChunk.get();
            currMetadataChunk = _primaryMetadataChunk.get();
        } else {
            spdlog::info("Getting non primary index chunk {}", chunkIndex);
            tempIndexChunk = std::move(_master.GetIndexChunk(chunkIndex));
            tempMetadataChunk = std::move(_master.GetMetadataChunk(chunkIndex));

            currIndexChunk = tempIndexChunk.get();
            currMetadataChunk = tempMetadataChunk.get();
        }

        auto endTime = std::chrono::steady_clock::now();
        double time =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        if (time > timeUpperLimitMs) {
            break;
        }

        Parser parser(query, &currIndexChunk->GetAllPostingLists());
        Expression* expr = parser.Parse();
        if (!expr) {
            return {};
        }
        auto ISR = expr->Eval();

        endTime = std::chrono::steady_clock::now();
        time =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        if (time > timeUpperLimitMs) {
            break;
        }

        if (!ISR) {
            return {};
        }
        ISR->Next();

        while (ISR->GetCurrentPostEntry() != std::nullopt) {
            if (docs.size() == matchCount)
                break;
            auto endTime = std::chrono::steady_clock::now();
            double time =
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            if (time > timeUpperLimitMs) {
                break;
            }

            uint32_t docId = ISR->GetDocumentID();
            uint32_t absolute_location = ISR->GetStartLocation();
            std::string docName = currIndexChunk->GetDocName(docId);
            metadata_t data = currMetadataChunk->GetMetadata(docName);

            int numTitleOccurences = 0;
            int numBodyOccurences = 0;
            while (ISR->GetCurrentPostEntry() != std::nullopt) {
                std::optional<PostEntry> entry = ISR->GetCurrentPostEntry();
                if (ISR->GetDocumentID() == docId) {
                    if (entry->GetLocationFound() == wordlocation_t::title) {
                        numTitleOccurences++;
                    } else {
                        numBodyOccurences++;
                    }
                } else {
                    break;  // we did Next() and that caused us to move on to a brand new document
                }
                auto endTime = std::chrono::steady_clock::now();
                double time =
                    std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
                        .count();
                if (time > timeUpperLimitMs) {
                    break;
                }
                ISR->Next();
            }
            search_result_t docData(docName, data.numWords, data.numTitleWords, data.numOutLinks,
                                    numTitleOccurences, numBodyOccurences, data.pageRank,
                                    data.cheiRank, data.docNum, data.docStartOffset,
                                    absolute_location);
            docs.push_back(docData);
            endTime = std::chrono::steady_clock::now();
            time =
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
                    .count();
        }
        delete ISR;
        chunkIndex++;
    }
    return docs;
}

void IndexServer::rank(search_results& input) {
    std::sort(input.begin(), input.end(), [](const search_result_t& a, const search_result_t& b) {
        return a.rankingScore > b.rankingScore;
    });
}

std::pair<std::string, std::string> IndexServer::getTitleAndSnippet(search_result_t docMetadata,
                                                                    int delta) {
    std::string filePath = _htmlDir + std::to_string(docMetadata.docNum) + ".parsed";
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error openning " << filePath << endl;
        return {"", ""};
    }

    int docRelativeMatchOffset = docMetadata.matchAbsoluteOffset - docMetadata.docStartOffset;
    if (docRelativeMatchOffset < 0) {
        return {"", ""};
    }

    int snippetStartOffset = std::max(docRelativeMatchOffset - delta, 0);

    std::string title;
    std::string snippet;

    std::string line;
    std::getline(file, line);  // URL and Doc num line
    std::getline(file, line);  // get <title> tag
    std::getline(file, line);  // get titles

    std::istringstream titleIss(line);
    title = line;
    std::string word;
    uint32_t offset = 0;
    bool snippetFound = false;
    while (titleIss >> word) {
        word = strip_utf8_spaces(word);
        if (!is_ascii(word))
            continue;

        if (offset == snippetStartOffset) {
            snippet += word;
            snippet += " ";
            snippetFound = true;
            offset++;
            break;
        }
        ++offset;
    }

    if (snippetFound) {
        while (titleIss >> word) {
            snippet += word;
            snippet += " ";
            if (offset == docRelativeMatchOffset + delta) {
                break;
            }
            ++offset;
        }
        return {title, snippet};
    }

    std::getline(file, line);  // get </title>
    std::getline(file, line);  // get <words>
    std::getline(file, line);  // get words
    std::istringstream wordIss(line);
    while (wordIss >> word) {
        word = strip_utf8_spaces(word);
        if (!is_ascii(word))
            continue;

        if (offset == snippetStartOffset) {
            snippet += word;
            snippet += " ";
            snippetFound = true;
            offset++;
            break;
        }
        ++offset;
    }

    if (snippetFound) {
        while (wordIss >> word) {
            if (offset == docRelativeMatchOffset + delta) {
                break;
            }
            snippet += word;
            snippet += " ";
            ++offset;
        }
    }

    return {title, snippet};
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

    program.add_argument("-i", "--input").required().help("Directory index files are located");

    program.add_argument("-h", "--htmlinput").required().help("Directory html files are located");

    program.add_argument("-c", "--count")
        .default_value(100)
        .help("Number of mathes to look for before stopping index search")
        .scan<'i', int>();

    program.add_argument("-t", "--time")
        .default_value(1000)
        .help("Max time on how long to search index for ")
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
    std::string indexPath = program.get<std::string>("-i");
    std::string htmlPath = program.get<std::string>("-h");
    int matchCount = program.get<int>("-c");
    int waitTimeMS = program.get<int>("-t");

    spdlog::info("Port {}", port);
    spdlog::info("Max clients {}", maxClients);
    spdlog::info("Index Path {}", indexPath);
    spdlog::info("Match count {}", matchCount);
    spdlog::info("Wait time in ms {}", waitTimeMS);

    int fd = -1;
    auto [buf, size] = read_mmap_region(fd, indexPath + "masterchunk");
    size_t offset = 0;
    MasterChunk master = MasterChunk::Deserailize(static_cast<char*>(buf), offset);
    munmap(buf, size);
    close(fd);

    IndexServer indexServer(port, maxClients, indexPath, htmlPath, matchCount, waitTimeMS, master);
    spdlog::info("======= Index Server Started =======");
    indexServer.Start();
}
