#include <gtest/gtest.h>

#include <fcntl.h>  // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/stat.h>  // For mode constants
#include <unistd.h>    // For ftruncate, close

#include "PostingList.hpp"
#include "Util.hpp"
#include "WordLocation.hpp"

std::vector<uint8_t> encode(uint32_t value) {
    std::vector<uint8_t> bytes;
    do {
        uint8_t byte = value & 0x7F;  // Get the lowest 7 bits
        value >>= 7;
        if (value != 0) {
            bytes.push_back(byte);    // More bytes to come
        } else {
            byte |= 0x80;             // Set MSB = 1 to mark end
            bytes.push_back(byte);
            break;
        }
    } while (true);

    return bytes;
}

uint32_t decode(const uint8_t* base, size_t& offset) {
    uint32_t result = 0;
    int shift = 0;
    while (true) {
        uint8_t byte = *(base + offset);
        offset++;

        result |= (byte & 0x7F) << shift;

        if (byte & 0x80) break;  // MSB = 1 means done
        shift += 7;
    }
    return result;
}

TEST(BasicCompression, EncodeDecodeVB) {
    std::vector<uint32_t> values = {1, 127, 128, 255, 1024, 16384, 1 << 24};
    for (uint32_t val : values) {
        std::vector<uint8_t> encoded = encode(val);
        size_t offset = 0;
        uint32_t decoded = decode(encoded.data(), offset);

        std::cout << "Original: " << val << " | Decoded: " << decoded << "\n";
        EXPECT_EQ(decoded, val);
        EXPECT_EQ(offset, encoded.size());
    }
}

TEST(BasicCompression, CompressedVsNaiveSizeBenefit) {
    // test that using VB compression along with relative deltas actually does indeed give size benefits
    std::vector<uint32_t> abs_positions = {4, 5, 10, 18, 300, 900, 6000};
    std::vector<uint32_t> deltas;
    deltas.push_back(abs_positions[0]);
    for (size_t i = 1; i < abs_positions.size(); ++i) {
        deltas.push_back(abs_positions[i] - abs_positions[i - 1]);
    }

    // Naive method: 4 bytes per int
    size_t naive_bytes = abs_positions.size() * sizeof(uint32_t);

    // VB-compressed size
    size_t compressed_bytes = 0;
    for (uint32_t d : deltas) {
        compressed_bytes += encode(d).size();
    }

    std::cout << "Compressed takes " << compressed_bytes << " bytes" << std::endl;
    std::cout << "Naive takes " << naive_bytes << " bytes" << std::endl;

    EXPECT_LT(compressed_bytes, naive_bytes);
}

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
    PostEntry word_occurrence_1 = {123456, wordlocation_t::body};
    PostEntry word_occurrence_2 = {123457, wordlocation_t::title};
    PostEntry word_occurrence_3 = {123458, wordlocation_t::body};

    // and let's say there is another document called fox/index.html
    std::string fox_doc = "fox/index.html";
    PostEntry word_occurrence_4 = {200000, wordlocation_t::body};
    PostEntry word_occurrence_5 = {200005, wordlocation_t::title};
    PostEntry word_occurrence_6 = {200010, wordlocation_t::title};
    PostEntry word_occurrence_7 = {200015, wordlocation_t::body};

    postingList->AddWord(cnn_doc, word_occurrence_1);
    postingList->AddWord(cnn_doc, word_occurrence_2);
    postingList->AddWord(cnn_doc, word_occurrence_3);

    postingList->AddWord(fox_doc, word_occurrence_4);
    postingList->AddWord(fox_doc, word_occurrence_5);
    postingList->AddWord(fox_doc, word_occurrence_6);
    postingList->AddWord(fox_doc, word_occurrence_7);

    size_t offset = 0;
    PostingList::NewSerialize(static_cast<char*>(base_region), offset,
                           *postingList);

    std::cout << "Compressed PostingList serialized to mmap.\n";
    std::cout << "This many bytes were used for COMPRESSED RELATIVE DELTAS: " << offset << std::endl;

    size_t new_offset = offset;
    PostingList::OldSerialize(static_cast<char*>(base_region), new_offset, *postingList);

    std::cout << "Naive PostingList serialized to mmap.\n";
    std::cout << "This many bytes were used for NAIVE: " << new_offset - offset << std::endl;

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
        PostingList::NewDeserialize(static_cast<char*>(base_region), offset);

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
    if (cnn_word_occurrence_1.GetDelta() == 123456 &&
        cnn_word_occurrence_1.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry cnn_word_occurrence_2 = entries[1];
    if (cnn_word_occurrence_2.GetDelta() == 123457 &&
        cnn_word_occurrence_2.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry cnn_word_occurrence_3 = entries[2];
    if (cnn_word_occurrence_3.GetDelta() == 123458 &&
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
    if (fox_word_occurrence_1.GetDelta() == 200000 &&
        fox_word_occurrence_1.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_2 = entries[1];
    if (fox_word_occurrence_2.GetDelta() == 200005 &&
        fox_word_occurrence_2.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_3 = entries[2];
    if (fox_word_occurrence_3.GetDelta() == 200010 &&
        fox_word_occurrence_3.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    PostEntry fox_word_occurrence_4 = entries[3];
    if (fox_word_occurrence_4.GetDelta() == 200015 &&
        fox_word_occurrence_4.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    postingList =
        PostingList::OldDeserialize(static_cast<char*>(base_region), offset);

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

    catPostingList = postingList.GetPosts();
    cnnPost = catPostingList[0];
    if (cnnPost.GetDocumentName() == "cnn/index.html") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    entries = cnnPost.GetEntries();

    cnn_word_occurrence_1 = entries[0];
    if (cnn_word_occurrence_1.GetDelta() == 123456 &&
        cnn_word_occurrence_1.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    cnn_word_occurrence_2 = entries[1];
    if (cnn_word_occurrence_2.GetDelta() == 123457 &&
        cnn_word_occurrence_2.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    cnn_word_occurrence_3 = entries[2];
    if (cnn_word_occurrence_3.GetDelta() == 123458 &&
        cnn_word_occurrence_3.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    foxPost = catPostingList[1];
    if (foxPost.GetDocumentName() == "fox/index.html") {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    entries = foxPost.GetEntries();

    fox_word_occurrence_1 = entries[0];
    if (fox_word_occurrence_1.GetDelta() == 200000 &&
        fox_word_occurrence_1.GetLocationFound() == wordlocation_t::body) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    fox_word_occurrence_2 = entries[1];
    if (fox_word_occurrence_2.GetDelta() == 200005 &&
        fox_word_occurrence_2.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    fox_word_occurrence_3 = entries[2];
    if (fox_word_occurrence_3.GetDelta() == 200010 &&
        fox_word_occurrence_3.GetLocationFound() == wordlocation_t::title) {
        std::cout << "Passed!" << std::endl;
    } else {
        std::cout << "Failed!" << std::endl;
        exit(1);
    }

    fox_word_occurrence_4 = entries[3];
    if (fox_word_occurrence_4.GetDelta() == 200015 &&
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
