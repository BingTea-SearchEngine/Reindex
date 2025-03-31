#include <gtest/gtest.h>
#include <filesystem>

#include "MasterChunk.hpp"
#include "WordLocation.hpp"
#include "Util.hpp"

TEST(BasicMasterChunk, SerialiezDeserialize) {
    // low chunk size so it creates a new chunk after every word
    std::filesystem::path cwd = std::filesystem::current_path();
    MasterChunk master(cwd.string()+"/temp", 1);
    std::vector<word_t> w;
    w.push_back(word_t{"w", 0, wordlocation_t::title});

    master.AddDocument("doc1", w);
    master.AddDocument("doc2", w);
    master.Flush();

    std::string filePath = "test_master_chunk";
    int fd = -1;
    void* base_region = create_mmap_region(fd, 4098, filePath);

    size_t offset = 0;
    MasterChunk::Serialize(static_cast<char*>(base_region), offset, master);
    munmap(base_region, 4098);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);

    int fd2 = -1;
    void* buf2 = read_mmap_region(fd2, 4098, filePath);
    offset = 0;
    MasterChunk master2 = MasterChunk::Deserailize(static_cast<char*>(buf2), offset);
    munmap(buf2, 4098);
    close(fd2);

    EXPECT_EQ(master.GetNumDocuments(), master2.GetNumDocuments());
    EXPECT_EQ(master.GetChunkList(), master2.GetChunkList());
}
