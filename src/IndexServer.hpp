#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <argparse/argparse.hpp>
#include <fstream>
#include <sstream>
#include <memory>

#include "Expression.hpp"
#include "GatewayServer.hpp"
#include "IndexChunk.hpp"
#include "IndexInterface.hpp"
#include "MasterChunk.hpp"
#include "Parser.hpp"

struct search_result_t {
    uint32_t numWords;
    uint32_t numTitleWords;
    uint32_t numOutLinks;
    float pageRank;
    float cheiRank;
    uint32_t docNum;          // For snippet retrieval
    uint32_t docStartOffset;  // For snippet retrieval
};

class IndexServer {
   public:
    IndexServer(int port, int maxClients, std::string indexPath, std::string htmlPath,
                MasterChunk master);

    void Start();

   private:
    std::vector<std::pair<std::string, search_result_t>> findDocuments(std::string query,
                                                                  int matchCount);

    std::string getSnippet(metadata_t docMetadata, uint32_t matchOffset, int delta);

    IndexMessage _handleSearch(IndexMessage msg);

    Server _server;

    MasterChunk _master;

    std::unique_ptr<IndexChunk> _primaryIndexChunk;
    std::unique_ptr<IndexChunk> _secondaryIndexChunk;
    std::unique_ptr<MetadataChunk> _primaryMetadataChunk;
    std::unique_ptr<MetadataChunk> _secondaryMetadataChunk;

    std::string _htmlDir;
};
