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


