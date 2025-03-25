#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include "DocStream.hpp"
#include "MasterChunk.hpp"
#include "WordLocation.hpp"
#include "Util.hpp"

void* create_mmap_region(int& fd, size_t size) {
    fd = open("test_posting_list", O_CREAT | O_RDWR, 0666);  // 0666 = rw
    ftruncate(fd, size);
    return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void* read_mmap_region(int& fd, size_t size) {
    fd = open("test_posting_list", O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Failed to open file");
    }

    struct stat fileStat;
    size_t fileSize;
    if (fstat(fd, &fileStat) == -1) {
        close(fd);
        throw std::runtime_error(
            "Error: Failed to get file size for test_posting_list");
    }
    fileSize = fileStat.st_size;

    void* mappedRegion = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mappedRegion == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Error: Failed to memory-map file");
    }

    return mappedRegion;
}

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
    spdlog::info("Input directory: {}", inputDir);
    spdlog::info("Output directory: {}", outputDir);
    spdlog::info("Chunk size: {} MB", chunkSizeMB);
    spdlog::info("Chunk size: {}", chunkSizeBytes);
    spdlog::info("======= Reindex Started =======");

    DocStream docStream(inputDir);
    MasterChunk master(outputDir, chunkSizeBytes);

    while (docStream.size() > 0) {
        std::pair<std::string, std::vector<word_t>> nextDoc = docStream.nextFile();
        auto [document, words] = nextDoc;
        if (words.empty()) {
            spdlog::error("Error parsing {}", document);
            continue;
        }
        master.AddDocument(document, words);
    }
    master.Flush();

    // Open file and mmap
    std::string masterchunkOutputFile = outputDir + "/masterchunk";
    int fd = -1;
    void* base_region = create_mmap_region(fd, 4098, masterchunkOutputFile);

    //Serialize master chunk
    size_t offset = 0;
    MasterChunk::Serialize(static_cast<char*>(base_region), offset, master);

    // Un memory map and truncate file
    munmap(base_region, 4098);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);
}

