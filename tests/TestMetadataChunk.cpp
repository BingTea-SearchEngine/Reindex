#include <gtest/gtest.h>

#include "MetadataChunk.hpp"
#include "WordLocation.hpp"
#include "Util.hpp"

TEST(BasicIndexChunk, SerializeDeserialize) {
    MetadataChunk chunk;
    
    // size_t numWords;
    // size_t numTitleWords;
    // float pageRank;
    // float cheiRank;
    // size_t numOutLinks;
    // std::vector<std::string> outLinks;
    
    metadata_t d1{1, 2, 0.1, 0.2, 1, std::vector<std::string>{"a"}};
    metadata_t d2{3, 4, 0.3, 0.4, 2, std::vector<std::string>{"b", "c"}};
    metadata_t d3{5, 6, 0.5, 0.6, 3, std::vector<std::string>{"d", "e", "f"}};

    chunk.AddDocument("doc1", d1);
    chunk.AddDocument("doc2", d2);
    chunk.AddDocument("doc3", d3);

    std::string filePath = "test_metadata_chunk";
    int fd = -1;
    void* base_region = create_mmap_region(fd, 4098, filePath);

    size_t offset = 0;
    MetadataChunk::Serialize(static_cast<char*>(base_region), offset, chunk);
    munmap(base_region, 4098);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);

    int fd2 = -1;
    void* buf2 = read_mmap_region(fd2, 4098, filePath);
    offset = 0;
    MetadataChunk chunk2 = MetadataChunk::Deserailize(static_cast<char*>(buf2), offset);
    munmap(buf2, 4098);
    close(fd2);

    EXPECT_EQ(chunk.GetDocuments(), chunk2.GetDocuments());

    for(const auto &doc : chunk.GetDocuments()) {
        EXPECT_TRUE(chunk.GetMetadata(doc) == chunk2.GetMetadata(doc));
    }
}
