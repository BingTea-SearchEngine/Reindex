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
      .help("Specify the input directory.");

    program.add_argument("-o", "--output")
      .required()
      .help("Specify the output directory.");

    program.add_argument("-s", "--size")
        .default_value(500)
        .help("Specify the size of an index chunk in MB")
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string inputDir = program.get<std::string>("-i");
    std::string outputDir = program.get<std::string>("-o");
    int chunkSizeMB = program.get<int>("-s");
    size_t chunkSizeBytes = chunkSizeMB * 1000000;
    spdlog::info("======= Reindex Started =======");
    spdlog::info("Input directory: {}", inputDir);
    spdlog::info("Output directory: {}", outputDir);
    spdlog::info("Chunk size: {} MB", chunkSizeMB);
    spdlog::info("Chunk size: {}", chunkSizeBytes);

    DocStream docStream(inputDir);
    MasterChunk master(outputDir, chunkSizeBytes);

    while (docStream.size() > 0) {
        std::pair<docname, words> nextDoc = docStream.nextFile();
        auto [document, words] = nextDoc;
        if (words.empty()) {
            spdlog::error("Error parsing {}", document);
            continue;
        }
        master.AddDocument(document, words);
    }

    // mmap and serialize Master Chunk
    std::string filePath = outputDir + "/master_chunk";
    size_t masterSize = 1024 * 5; // 5 KB for now

    int fd = open(filePath.c_str(), O_CREAT | O_RDWR);
    // Ensure file is at least masterSize
    if (ftruncate(fd, masterSize) == -1) {
        perror("Error extending file size");
        close(fd);
        return 1;
    }
    // Memory map 
    char* buf = (char *)mmap(nullptr, masterSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        perror("Error mmap");
        close(fd);
        return 1;
    }
    // Serailize master
    size_t memUsed = MasterChunk::Serialize(buf, master);
    // Flush changes to file
    if (msync(buf, masterSize, MS_SYNC) == -1) {
        perror("Error syncing file");
    }
    // Truncate file to bytes actually used
    if (ftruncate(fd, memUsed) == -1) {
        perror("Error truncating file");
    }
    // Unmap and close file
    if (munmap(buf, memUsed) == -1) {
        perror("Error unmapping memory");
    }
    close(fd);
}
