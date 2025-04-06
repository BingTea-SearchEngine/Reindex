#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "GatewayServer.hpp"

class IndexServer {
   public:
    IndexServer(int port, int maxClients);

    void Start();

   private:
    Server _server;
};
