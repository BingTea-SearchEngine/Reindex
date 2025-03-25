#include <gtest/gtest.h>

#include <fcntl.h>  // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>  // For mode constants
#include <unistd.h>    // For ftruncate, close

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

TEST(BasicPostingList, TestConstructor) {
    PostingList pl("hello");
    EXPECT_EQ(pl.word, "hello");
    EXPECT_EQ(pl.GetOverheadBytes(), 14);
    EXPECT_EQ(pl.getWord(), "hello");
}

TEST(BasicPostingList, TestAddWord) {
    PostingList pl("test");
    PostEntry w1 = {0, wordlocation_t::title};
    PostEntry w2 = {1, wordlocation_t::body};
    PostEntry w3 = {2, wordlocation_t::bold};
    pl.addWord("doc1", w1);
    pl.addWord("doc1", w2);
    pl.addWord("doc2", w3);

    // Access posts using getPosts()
    auto posts = pl.getPosts();

    // First post
    EXPECT_EQ(posts[0].getDocumentName(), "doc1");
    auto wordEntries1 =
        posts[0].getEntries();  // Get entries for the first post
    EXPECT_EQ(wordEntries1[0].getDelta(), w1.getDelta());
    EXPECT_EQ(wordEntries1[0].getLocationFound(), w1.getLocationFound());
    EXPECT_EQ(wordEntries1[1].getDelta(), w2.getDelta());
    EXPECT_EQ(wordEntries1[1].getLocationFound(), w2.getLocationFound());

    // Second post
    EXPECT_EQ(posts[1].getDocumentName(), "doc2");
    auto wordEntries2 =
        posts[1].getEntries();  // Get entries for the second post
    EXPECT_EQ(wordEntries2[0].getDelta(), w3.getDelta());
    EXPECT_EQ(wordEntries2[0].getLocationFound(), w3.getLocationFound());
}

void test_serializiation() {
    int fd;
    const size_t REGION_SIZE = 4096;
    void* base_region = create_mmap_region(fd, REGION_SIZE);

    PostingList* postingList = new PostingList("cat");

    // let's say there is a document named cnn/index.html
    std::string cnn_doc = "cnn/index.html";
    // and on this document, there are a couple occurrences of the word "cat" in the body
    PostEntry word_occurrence_1 = {5, wordlocation_t::body};
    PostEntry word_occurrence_2 = {10, wordlocation_t::title};
    PostEntry word_occurrence_3 = {17, wordlocation_t::body};

    // and let's say there is another document called fox/index.html
    std::string fox_doc = "fox/index.html";
    PostEntry word_occurrence_4 = {20, wordlocation_t::body};
    PostEntry word_occurrence_5 = {25, wordlocation_t::title};
    PostEntry word_occurrence_6 = {29, wordlocation_t::title};
    PostEntry word_occurrence_7 = {40, wordlocation_t::body};

    postingList->addWord(cnn_doc, word_occurrence_1);
    postingList->addWord(cnn_doc, word_occurrence_2);
    postingList->addWord(cnn_doc, word_occurrence_3);

    postingList->addWord(fox_doc, word_occurrence_4);
    postingList->addWord(fox_doc, word_occurrence_5);
    postingList->addWord(fox_doc, word_occurrence_6);
    postingList->addWord(fox_doc, word_occurrence_7);

    size_t offset = 0;
    PostingList::Serialize(static_cast<char*>(base_region), offset,
                           *postingList);

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

    PostingList postingList =
        PostingList::Deserialize(static_cast<char*>(base_region), offset);

    if (postingList.getWord() == "cat") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    if (postingList.getPosts().size() == 2) {  // one for cnn and one for fox
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    std::vector<Post> catPostingList = postingList.getPosts();
    Post cnnPost = catPostingList[0];
    if (cnnPost.getDocumentName() == "cnn/index.html") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    auto entries = cnnPost.getEntries();

    PostEntry cnn_word_occurrence_1 = entries[0];
    if (cnn_word_occurrence_1.getDelta() == 5 &&
        cnn_word_occurrence_1.getLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry cnn_word_occurrence_2 = entries[1];
    if (cnn_word_occurrence_2.getDelta() == 10 &&
        cnn_word_occurrence_2.getLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry cnn_word_occurrence_3 = entries[2];
    if (cnn_word_occurrence_3.getDelta() == 17 &&
        cnn_word_occurrence_3.getLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    Post foxPost = catPostingList[1];
    if (foxPost.getDocumentName() == "fox/index.html") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    entries = foxPost.getEntries();

    PostEntry fox_word_occurrence_1 = entries[0];
    if (fox_word_occurrence_1.getDelta() == 20 &&
        fox_word_occurrence_1.getLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_2 = entries[1];
    if (fox_word_occurrence_2.getDelta() == 25 &&
        fox_word_occurrence_2.getLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_3 = entries[2];
    if (fox_word_occurrence_3.getDelta() == 29 &&
        fox_word_occurrence_3.getLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_4 = entries[3];
    if (fox_word_occurrence_4.getDelta() == 40 &&
        fox_word_occurrence_4.getLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    std::cout << "All tests passed!" << std::endl;
}

TEST(BasicPostingList, SerializeDeserialize) {
    test_serializiation();
    test_deserialization();
}
