#include <gtest/gtest.h>

#include "IndexChunk.hpp"
#include "Util.hpp"
#include "WordLocation.hpp"

TEST(BasicIndexChunk, SerialiezDeserialize) {
    IndexChunk chunk;
    std::vector<word_t> d1 = {word_t{"a", 0, wordlocation_t::title}};
    std::vector<word_t> d2 = {word_t{"b", 1, wordlocation_t::title}};
    std::vector<word_t> d3 = {word_t{"c", 2, wordlocation_t::title}};

    chunk.AddDocument("doc1", d1);
    chunk.AddDocument("doc2", d2);
    chunk.AddDocument("doc3", d3);

    std::string filePath = "test_master_chunk";
    int fd = -1;
    void* base_region = create_mmap_region(fd, 4098, filePath);

    size_t offset = 0;
    IndexChunk::Serialize(static_cast<char*>(base_region), offset, chunk);
    munmap(base_region, 4098);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);

    int fd2 = -1;
    void* buf2 = read_mmap_region(fd2, 4098, filePath);
    offset = 0;
    IndexChunk chunk2 =
        IndexChunk::Deserailize(static_cast<char*>(buf2), offset);
    munmap(buf2, 4098);
    close(fd2);

    EXPECT_EQ(chunk.GetDocuments(), chunk2.GetDocuments());

    // Check for "a"
    std::vector<Post> chunk1PostForA = chunk.GetPostingList("a").GetPosts();
    std::vector<Post> chunk2PostForA = chunk2.GetPostingList("a").GetPosts();
    EXPECT_EQ(chunk1PostForA.size(), chunk2PostForA.size());
    for (size_t i = 0; i < chunk1PostForA.size(); ++i) {
        std::vector<PostEntry> chunk1PostEntries =
            chunk1PostForA[i].GetEntries();
        std::vector<PostEntry> chunk2PostEntries =
            chunk2PostForA[i].GetEntries();
        EXPECT_EQ(chunk1PostEntries.size(), chunk2PostEntries.size());
        for (size_t j = 0; j < chunk1PostEntries.size(); ++j) {
            PostEntry chunk1PE = chunk1PostEntries[i];
            PostEntry chunk2PE = chunk2PostEntries[i];
            EXPECT_EQ(chunk1PE.GetDelta(), chunk2PE.GetDelta());
            EXPECT_EQ(chunk1PE.GetLocationFound(), chunk2PE.GetLocationFound());
        }
    }

    // Check for "b"
    std::vector<Post> chunk1PostForB = chunk.GetPostingList("b").GetPosts();
    std::vector<Post> chunk2PostForB = chunk2.GetPostingList("b").GetPosts();
    EXPECT_EQ(chunk1PostForB.size(), chunk2PostForB.size());
    for (size_t i = 0; i < chunk1PostForB.size(); ++i) {
        std::vector<PostEntry> chunk1PostEntries =
            chunk1PostForA[i].GetEntries();
        std::vector<PostEntry> chunk2PostEntries =
            chunk2PostForA[i].GetEntries();
        EXPECT_EQ(chunk1PostEntries.size(), chunk2PostEntries.size());
        for (size_t j = 0; j < chunk1PostEntries.size(); ++j) {
            PostEntry chunk1PE = chunk1PostEntries[i];
            PostEntry chunk2PE = chunk2PostEntries[i];
            EXPECT_EQ(chunk1PE.GetDelta(), chunk2PE.GetDelta());
            EXPECT_EQ(chunk1PE.GetLocationFound(), chunk2PE.GetLocationFound());
        }
    }

    // Check for "c"
    std::vector<Post> chunk1PostForC = chunk.GetPostingList("c").GetPosts();
    std::vector<Post> chunk2PostForC = chunk2.GetPostingList("c").GetPosts();
    EXPECT_EQ(chunk1PostForC.size(), chunk2PostForC.size());
    for (size_t i = 0; i < chunk1PostForC.size(); ++i) {
        std::vector<PostEntry> chunk1PostEntries =
            chunk1PostForA[i].GetEntries();
        std::vector<PostEntry> chunk2PostEntries =
            chunk2PostForA[i].GetEntries();
        EXPECT_EQ(chunk1PostEntries.size(), chunk2PostEntries.size());
        for (size_t j = 0; j < chunk1PostEntries.size(); ++j) {
            PostEntry chunk1PE = chunk1PostEntries[i];
            PostEntry chunk2PE = chunk2PostEntries[i];
            EXPECT_EQ(chunk1PE.GetDelta(), chunk2PE.GetDelta());
            EXPECT_EQ(chunk1PE.GetLocationFound(), chunk2PE.GetLocationFound());
        }
    }
}
