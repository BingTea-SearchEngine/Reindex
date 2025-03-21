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
    fd = open("test_posting_list", O_CREAT | O_RDWR, 0666); // 0666 = rw
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
        throw std::runtime_error("Error: Failed to get file size for test_posting_list");
    }
    fileSize = fileStat.st_size;

    void* mappedRegion = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mappedRegion == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Error: Failed to memory-map file");
    }

    return mappedRegion;
}

void test_serializiation() {
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
}

void test_deserialization() {
    int fd;
    const size_t REGION_SIZE = 4096;
    void* base_region = read_mmap_region(fd, REGION_SIZE);
    size_t offset = 0;

    PostingList postingList = PostingList::Deserialize(static_cast<char*>(base_region), offset);

    if (postingList.getPosts().size() == 2) { // one for cnn and one for fox
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    Post cnnPost = postingList.getPost(0);
    if (cnnPost.document == "cnn/index.html") {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    words w = cnnPost.getEntries();

    word_t cnn_word_occurrence_1 = w[0];
    if (cnn_word_occurrence_1.word == "cat" && cnn_word_occurrence_1.offset == 5 && cnn_word_occurrence_1.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    word_t cnn_word_occurrence_2 = w[1];
    if (cnn_word_occurrence_2.word == "cat" && cnn_word_occurrence_2.offset == 10 && cnn_word_occurrence_2.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    word_t cnn_word_occurrence_3 = w[2];
    if (cnn_word_occurrence_3.word == "cat" && cnn_word_occurrence_3.offset == 17 && cnn_word_occurrence_3.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    Post foxPost = postingList.getPost(1);
    if (foxPost.document == "fox/index.html") {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    w = foxPost.getEntries();

    word_t fox_word_occurrence_1 = w[0];
    if (fox_word_occurrence_1.word == "cat" && fox_word_occurrence_1.offset == 20 && fox_word_occurrence_1.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    word_t fox_word_occurrence_2 = w[1];
    if (fox_word_occurrence_2.word == "cat" && fox_word_occurrence_2.offset == 25 && fox_word_occurrence_2.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    word_t fox_word_occurrence_3 = w[2];
    if (fox_word_occurrence_3.word == "cat" && fox_word_occurrence_3.offset == 29 && fox_word_occurrence_3.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    word_t fox_word_occurrence_4 = w[3];
    if (fox_word_occurrence_4.word == "cat" && fox_word_occurrence_4.offset == 40 && fox_word_occurrence_4.location == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    }
    else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    std::cout << "All tests passed!" << std::endl;
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

    // test_serializiation();
    test_deserialization();

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