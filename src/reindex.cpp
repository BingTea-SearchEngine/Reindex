#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>

#include <fcntl.h>  // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>  // For mode constants
#include <unistd.h>    // For ftruncate, close

#include "DocStream.hpp"
#include "MasterChunk.hpp"
#include "Post.hpp"
#include "PostingList.hpp"
#include "WordLocation.hpp"

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
        .help("Specify the input file.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    std::string inputDir = program.get<std::string>("-i");
    spdlog::info("Input directory: {}", inputDir);
    spdlog::info("======= Reindex Started =======");
    // Initialize DocStream object
    DocStream docStream(inputDir);
    MasterChunk master;

    while (docStream.size() > 0) {
        std::pair<std::string, std::vector<word_t>> nextDoc = docStream.nextFile();
        auto [document, words] = nextDoc;
        if (words.empty()) {
            spdlog::error("Error parsing {}", document);
            continue;
        }
        master.addDocument(document, words);
    }

    return 0;
}


