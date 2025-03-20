#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>


#include <sys/mman.h>    // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>    // For mode constants
#include <fcntl.h>       // For O_CREAT, O_RDWR
#include <unistd.h>      // For ftruncate, close

#include "DocStream.hpp"
#include "MasterChunk.hpp"
#include "PostingList.hpp"
#include "Post.hpp"
#include "Types.hpp"

void* create_mmap_region(int& fd, size_t size) {
    fd = open("test_posting_list", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, size);
    return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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
    spdlog::info("======= Reindex Started =======");

    int fd;
    const size_t REGION_SIZE = 4096;
    void* base_region = create_mmap_region(fd, REGION_SIZE);

    PostingList* postingList = new PostingList("cat");

    // let's say there is a document named cnn/index.html
    std::string cnn_doc = "cnn/index.html";
    // and on this document, there are a couple occurrences of the word "cat"
    word_t word_occurrence_1 = {"cat", 5, wordlocation_t::body};
    word_t word_occurrence_2 = {"cat", 10, wordlocation_t::body};
    word_t word_occurrence_3 = {"cat", 17, wordlocation_t::body};

    // and let's say there is another document called fox/index.html
    std::string fox_doc = "fox/index.html";
    word_t word_occurrence_4 = {"cat", 20, wordlocation_t::body};
    word_t word_occurrence_5 = {"cat", 25, wordlocation_t::body};
    word_t word_occurrence_6 = {"cat", 29, wordlocation_t::body};
    word_t word_occurrence_7 = {"cat", 40, wordlocation_t::body};

    postingList->addWord(cnn_doc, word_occurrence_1);
    postingList->addWord(cnn_doc, word_occurrence_2);
    postingList->addWord(cnn_doc, word_occurrence_3);

    postingList->addWord(fox_doc, word_occurrence_4);
    postingList->addWord(fox_doc, word_occurrence_5);
    postingList->addWord(fox_doc, word_occurrence_6);
    postingList->addWord(fox_doc, word_occurrence_7);

    size_t offset = 0;
    PostingList::Serialize(static_cast<char*>(base_region), offset, *postingList);

    std::cout << "PostingList serialized to mmap.\n";

    munmap(base_region, REGION_SIZE);
    close(fd);
    delete postingList;
    return 0;
}

// spdlog::info("Input directory: {}", inputDir);

// Initialize DocStream object
// DocStream docStream(inputDir);
// MasterChunk master;

// while (docStream.size() > 0) {
//     std::pair<docname, words> nextDoc = docStream.nextFile();
//     auto [document, words] = nextDoc;
//     if (words.empty()) {
//         spdlog::error("Error parsing {}", document);
//         continue;
//     }
//     master.addDocument(document, words);
// }