#include <gtest/gtest.h>
#include <filesystem>

#include "MasterChunk.hpp"
#include "Util.hpp"
#include "WordLocation.hpp"

TEST(BasicMasterChunk, SerialiezDeserialize) {
    // low chunk size so it creates a new chunk after every word
    std::filesystem::path cwd = std::filesystem::current_path();
    MasterChunk master(cwd.string(), 1);
    std::vector<word_t> w;
    w.push_back(word_t{"w", 0, wordlocation_t::title});

    metadata_t m;

    std::cout << "Before adding documents" << std::endl;

    master.AddDocument("doc1", w, m);
    master.AddDocument("doc2", w, m);
    master.Flush();

    std::cout << "flushed everything" << std::endl;

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

    std::cout << "serialized" << std::endl;

    int fd2 = -1;
    auto [buf2, size] = read_mmap_region(fd2, filePath);
    offset = 0;
    MasterChunk master2 =
        MasterChunk::Deserailize(static_cast<char*>(buf2), offset);
    munmap(buf2, 4098);
    close(fd2);
    //
    // std::cout << "deserialized" << std::endl;
    //
    // EXPECT_EQ(master.GetNumDocuments(), master2.GetNumDocuments());
    // EXPECT_EQ(master.GetChunkList(), master2.GetChunkList());
    // EXPECT_EQ(master.GetMetadataChunkList(), master2.GetMetadataChunkList());
}
