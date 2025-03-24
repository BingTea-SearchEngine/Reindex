#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>

#include "Types.hpp"
#include "IndexChunk.hpp"

class MasterChunk {
public:
    static size_t Serialize(const char* buffer, const MasterChunk& master);

    static MasterChunk Deserailize(char* buffer);

    MasterChunk(std::string outputDir, size_t chunkSize);

    std::vector<std::string> GetChunkList();

    int GetNumDocuments();

    void AddDocument(docname doc, words words);

private:
    std::vector<std::string> _indexChunks;
    std::string _outputDir;
    IndexChunk _currIndexChunk;
    size_t _chunkSize;
    int _numDocuments;
};
