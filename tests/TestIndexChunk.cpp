#include <gtest/gtest.h>

#include "IndexChunk.hpp"
#include "WordLocation.hpp"
#include "Util.hpp"

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
    IndexChunk chunk2 = IndexChunk::Deserailize(static_cast<char*>(buf2), offset);
    munmap(buf2, 4098);
    close(fd2);

    EXPECT_EQ(chunk.GetDocuments(), chunk2.GetDocuments());
    // TODO: Check for individual words after functionis implemented
}
