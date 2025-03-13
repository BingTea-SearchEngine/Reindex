#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>


#include "DocStream.hpp"
#include "MasterChunk.hpp"

int main(int argc, char** argv) {
    argparse::ArgumentParser program("reindex");
    program.add_argument("-i", "--input")
      .required()
      .help("Specify the inputt file.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string inputDir = program.get<std::string>("-i");
    spdlog::info("======= Reindex Started =======");
    spdlog::info("Input directory: {}", inputDir);

    // Initialize DocStream object
    DocStream docStream(inputDir);
    MasterChunk master;

    while (docStream.size() > 0) {
        std::pair<docname, words> nextDoc = docStream.nextFile();
        auto [document, words] = nextDoc;
        if (words.empty()) {
            spdlog::error("Error parsing {}", document);
            continue;
        }
        master.addDocument(document, words);
    }
}
