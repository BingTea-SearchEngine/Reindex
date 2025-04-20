#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Expression.hpp"
#include "GatewayServer.hpp"
#include "IndexChunk.hpp"
#include "IndexInterface.hpp"
#include "MasterChunk.hpp"
#include "Parser.hpp"

class IndexServer {
   public:
    IndexServer(int port, int maxClients, std::string indexPath, std::string htmlPath,
                MasterChunk master);

    void Start();

   private:
    std::string getSnippet(metadata_t docMetadata, uint32_t matchOffset, int delta);

    IndexMessage _handleSearch(IndexMessage msg);

    Server _server;

    MasterChunk _master;

    IndexChunk _primaryIndexChunk;
    MetadataChunk _primaryMetadataChunk;

    std::string _htmlDir;
};
