#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "GatewayServer.hpp"
#include "IndexChunk.hpp"
#include "IndexInterface.hpp"
#include "MasterChunk.hpp"
#include "Parser.hpp"
#include "Expression.hpp"

class IndexServer {
   public:
    IndexServer(int port, int maxClients, std::string indexPath,
                MasterChunk master);

    void Start();

   private:
    IndexMessage _handleSearch(IndexMessage msg);

    Server _server;

    MasterChunk _master;

    IndexChunk _primaryIndexChunk;
    MetadataChunk _primaryMetadataChunk;
};
