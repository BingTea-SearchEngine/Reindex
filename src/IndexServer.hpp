#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "Expression.hpp"
#include "GatewayServer.hpp"
#include "IndexChunk.hpp"
#include "MasterChunk.hpp"
#include "Parser.hpp"
#include "IndexInterface.hpp"

class IndexServer {
   public:
    IndexServer(int port, int maxClients, std::string indexPath,
                MasterChunk master);

    void Start();

   private:
    std::string _handleSearch(std::string query);

    Server _server;

    MasterChunk _master;
    IndexChunk _primaryIndexChunk;
};
