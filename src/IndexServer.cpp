#include "IndexServer.hpp"
#include <chrono>

IndexServer::IndexServer(int port, int maxClients, std::string indexaPath, std::string htmlPath,
                         int matchCount, int waitTime, int numChunksLoaded, MasterChunk master)
    : _server(Server(port, maxClients)),
      _htmlDir(htmlPath),
      _matchCount(matchCount),
      _waitTimeMS(waitTime),
      _numChunksLoaded(numChunksLoaded),
      _master(std::move(master)) {
    _numChunksLoaded = std::min(_numChunksLoaded, (int)_master.GetChunkList().size());
    for (int i = 0; i < _numChunksLoaded; ++i) {
        _indexChunks.push_back(std::move(_master.GetIndexChunk(i)));
        _metadataChunks.push_back(std::move(_master.GetMetadataChunk(i)));
    }
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

    auto start = std::chrono::steady_clock::now();

    search_results docs = findDocuments(msg.query);

    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    spdlog::info("Got {} results in {}", docs.size(), time);

    // Some kind of ranking
    rank(docs);

    std::vector<doc_t> results;
    for (size_t i = 0; i < docs.size(); ++i) {
        search_result_t result = docs[i];
        auto [title, snippet] = getTitleAndSnippet(result, 10);
        results.push_back(doc_t{result.url, result.numWords, result.numTitleWords,
                                result.numOutLinks, result.numTitleMatch, result.numBodyMatch,
                                result.pageRank, result.cheiRank, result.community,
                                result.communityCount, result.rankingScore, snippet, title});
    }

    return IndexMessage{IndexMessageType::DOCUMENTS, "", results};
}

search_results IndexServer::findDocuments(std::string query) {
    search_results docs;
    size_t chunkIndex = 0;
    size_t numChunks = _master.NumChunks();

    auto startTime = std::chrono::steady_clock::now();
    std::vector<std::thread> threads;
    std::vector<search_results> initialSearchResults(_numChunksLoaded);
    for (; chunkIndex < _numChunksLoaded; ++chunkIndex) {
        threads.emplace_back(&IndexServer::searchChunk, this, query, chunkIndex, _matchCount,
                             &initialSearchResults[chunkIndex]);
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
        docs.insert(docs.end(), initialSearchResults[i].begin(), initialSearchResults[i].end());
    }
    auto endTime = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    if (docs.size() >= _matchCount || time >= _waitTimeMS) {
        return docs;
    }

    while (docs.size() < _matchCount && chunkIndex < numChunks) {
        auto endTime = std::chrono::steady_clock::now();
        auto time =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        spdlog::info("Time (ms): {}", time);
        if (time > _waitTimeMS) {
            break;
        }

        search_results chunkResult;
        searchChunk(query, chunkIndex, _matchCount - docs.size(), &chunkResult);

        docs.insert(docs.end(), chunkResult.begin(), chunkResult.end());
        chunkIndex++;
    }
    return docs;
}

void IndexServer::searchChunk(std::string query, size_t chunkIndex, int matchCount,
                              search_results* results) {
    spdlog::info("Searching chunk {}", chunkIndex);
    const IndexChunk* indexChunk;
    const MetadataChunk* metadataChunk;

    std::unique_ptr<const IndexChunk> tempIndexChunk;
    std::unique_ptr<const MetadataChunk> tempMetadataChunk;

    if (chunkIndex < _indexChunks.size()) {
        indexChunk = _indexChunks[chunkIndex].get();
        metadataChunk = _metadataChunks[chunkIndex].get();
    } else {
        tempIndexChunk = _master.GetIndexChunk(chunkIndex);
        tempMetadataChunk = _master.GetMetadataChunk(chunkIndex);
        indexChunk = tempIndexChunk.get();
        metadataChunk = tempMetadataChunk.get();
    }

    Parser parser(query, &indexChunk->GetAllPostingLists());
    Expression* expr = parser.Parse();
    if (!expr) {
        return;
    }
    auto ISR = expr->Eval();
    if (!ISR) {
        return;
    }
    ISR->Next();

    auto startTime = std::chrono::steady_clock::now();
    while (ISR->GetCurrentPostEntry() != std::nullopt) {
        if (results->size() == matchCount) {
            break;
        }
        uint32_t docId = ISR->GetDocumentID();
        uint32_t absolute_location = ISR->GetStartLocation();
        std::string docName = indexChunk->GetDocName(docId);
        metadata_t data = metadataChunk->GetMetadata(docName);

        int numTitleOccurences = 0;
        int numBodyOccurences = 0;
        while (ISR->GetCurrentPostEntry() != std::nullopt) {
            auto endTime = std::chrono::steady_clock::now();
            auto time =
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
            if (time >= _waitTimeMS) {
                break;
            }
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
            ISR->Next();
        }
        search_result_t docData(docName, data.numWords, data.numTitleWords, data.numOutLinks,
                                data.pageRank, data.cheiRank, numTitleOccurences, numBodyOccurences,
                                data.docNum, data.docStartOffset, absolute_location);
        auto endTime = std::chrono::steady_clock::now();
        auto time =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        if (time >= _waitTimeMS) {
            break;
        }
        results->push_back(docData);
    }
    delete ISR;
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
        if (!valid(word))
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
        if (!valid(word))
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

    program.add_argument("-n", "--num_chunks_inmem")
        .default_value(1)
        .help("Number of chunks in memory")
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
    int numChunksLoaded = program.get<int>("-n");

    spdlog::info("Port {}", port);
    spdlog::info("Max clients {}", maxClients);
    spdlog::info("Index Path {}", indexPath);
    spdlog::info("Match count {}", matchCount);
    spdlog::info("Wait time in ms {}", waitTimeMS);
    spdlog::info("Num chunks loaded {}", numChunksLoaded);

    int fd = -1;
    auto [buf, size] = read_mmap_region(fd, indexPath + "masterchunk");
    size_t offset = 0;
    MasterChunk master = MasterChunk::Deserailize(static_cast<char*>(buf), offset);
    munmap(buf, size);
    close(fd);
    spdlog::info("Num chunks in index {}", master.GetChunkList().size());

    IndexServer indexServer(port, maxClients, indexPath, htmlPath, matchCount, waitTimeMS,
                            numChunksLoaded, std::move(master));
    spdlog::info("======= Index Server Started =======");
    indexServer.Start();
}
