#include <gtest/gtest.h>

#include <fcntl.h>  // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>  // For mode constants
#include <unistd.h>    // For ftruncate, close

#include "PostingList.hpp"
#include "Util.hpp"
#include "WordLocation.hpp"

TEST(BasicPostingList, TestAddWord) {
    PostingList pl("test");
    PostEntry w1 = {0, wordlocation_t::title};
    PostEntry w2 = {1, wordlocation_t::body};
    PostEntry w3 = {2, wordlocation_t::bold};
    pl.AddWord("doc1", w1);
    pl.AddWord("doc1", w2);
    pl.AddWord("doc2", w3);

    // Access posts using GetPosts()
    auto posts = pl.GetPosts();

    // First post
    EXPECT_EQ(posts[0].GetDocumentName(), "doc1");
    auto wordEntries1 =
        posts[0].GetEntries();  // Get entries for the first post
    EXPECT_EQ(wordEntries1[0].GetDelta(), w1.GetDelta());
    EXPECT_EQ(wordEntries1[0].GetLocationFound(), w1.GetLocationFound());
    EXPECT_EQ(wordEntries1[1].GetDelta(), w2.GetDelta());
    EXPECT_EQ(wordEntries1[1].GetLocationFound(), w2.GetLocationFound());

    // Second post
    EXPECT_EQ(posts[1].GetDocumentName(), "doc2");
    auto wordEntries2 =
        posts[1].GetEntries();  // Get entries for the second post
    EXPECT_EQ(wordEntries2[0].GetDelta(), w3.GetDelta());
    EXPECT_EQ(wordEntries2[0].GetLocationFound(), w3.GetLocationFound());
}

void test_serializiation() {
    int fd;
    const size_t REGION_SIZE = 4096;
    void* base_region =
        create_mmap_region(fd, REGION_SIZE, "test_posting_list");

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

    postingList->AddWord(cnn_doc, word_occurrence_1);
    postingList->AddWord(cnn_doc, word_occurrence_2);
    postingList->AddWord(cnn_doc, word_occurrence_3);

    postingList->AddWord(fox_doc, word_occurrence_4);
    postingList->AddWord(fox_doc, word_occurrence_5);
    postingList->AddWord(fox_doc, word_occurrence_6);
    postingList->AddWord(fox_doc, word_occurrence_7);

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
    auto [base_region, size] = read_mmap_region(fd, "test_posting_list");
    size_t offset = 0;

    PostingList postingList =
        PostingList::Deserialize(static_cast<char*>(base_region), offset);

    if (postingList.GetWord() == "cat") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    if (postingList.GetPosts().size() == 2) {  // one for cnn and one for fox
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    std::vector<Post> catPostingList = postingList.GetPosts();
    Post cnnPost = catPostingList[0];
    if (cnnPost.GetDocumentName() == "cnn/index.html") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    auto entries = cnnPost.GetEntries();

    PostEntry cnn_word_occurrence_1 = entries[0];
    if (cnn_word_occurrence_1.GetDelta() == 5 &&
        cnn_word_occurrence_1.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry cnn_word_occurrence_2 = entries[1];
    if (cnn_word_occurrence_2.GetDelta() == 10 &&
        cnn_word_occurrence_2.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry cnn_word_occurrence_3 = entries[2];
    if (cnn_word_occurrence_3.GetDelta() == 17 &&
        cnn_word_occurrence_3.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    Post foxPost = catPostingList[1];
    if (foxPost.GetDocumentName() == "fox/index.html") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    entries = foxPost.GetEntries();

    PostEntry fox_word_occurrence_1 = entries[0];
    if (fox_word_occurrence_1.GetDelta() == 20 &&
        fox_word_occurrence_1.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_2 = entries[1];
    if (fox_word_occurrence_2.GetDelta() == 25 &&
        fox_word_occurrence_2.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_3 = entries[2];
    if (fox_word_occurrence_3.GetDelta() == 29 &&
        fox_word_occurrence_3.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_4 = entries[3];
    if (fox_word_occurrence_4.GetDelta() == 40 &&
        fox_word_occurrence_4.GetLocationFound() == wordlocation_t::body) {
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
