#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <unordered_set>
#include <vector>

#include "IndexChunk.hpp"
#include "WordLocation.hpp"

class MasterChunk {
   public:
    static void Serialize(const char* buf, const MasterChunk& masterChunk);

    static MasterChunk Deserailize(char* buffer);

    MasterChunk(std::string outputDir, size_t chunkSize);

    std::vector<std::string> GetChunkList();

    void addDocument(std::string doc, std::vector<word_t> words);

    int GetNumDocuments();

    void AddDocument(docname doc, words words);

private:
    std::vector<std::string> _indexChunks;
    std::string _outputDir;
    IndexChunk _currIndexChunk;
    size_t _chunkSize;
    int _numDocuments;
};
