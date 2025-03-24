#include <gtest/gtest.h>

#include "MasterChunk.hpp"
#include "Types.hpp"

TEST(BasicMasterChunk, SerialiezDeserialize) {
    // low chunk size so it creates a new chunk after every word
    MasterChunk master("tmp", 1);
    words w;
    w.push_back(word_t{"w", 0, wordlocation_t::title});

    master.AddDocument("doc1", w);
    master.AddDocument("doc2", w);

    std::string filePath = "master_chunk";
    size_t masterSize = 1024 * 5; // 5 KB for now
    int fd = open(filePath.c_str(), O_CREAT | O_RDWR);
    ftruncate(fd, masterSize);
    char* buf = (char *)mmap(nullptr, masterSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    size_t memUsed = MasterChunk::Serialize(buf, master);

    msync(buf, masterSize, MS_SYNC);
    ftruncate(fd, memUsed);
    munmap(buf, memUsed);
    close(fd);

    fd = open(filePath.c_str(), O_RDWR);
    buf = (char *)mmap(nullptr, masterSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    MasterChunk master2 = MasterChunk::Deserailize(buf);

    EXPECT_EQ(master.GetNumDocuments(), master2.GetNumDocuments());
    EXPECT_EQ(master.GetChunkList(), master2.GetChunkList());
}
