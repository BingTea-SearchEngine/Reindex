#pragma once

#include <unordered_set>

#include "Types.hpp"
#include "IndexChunk.hpp"

class MasterChunk {
public:
    static void Serialize(const char* buf, const MasterChunk& masterChunk);

    static MasterChunk Deserailize(const char* buf);

    MasterChunk();

    void addDocument(std::string doc, std::vector<postentry_t> words);

    size_t getBytesRequired();

private:
    std::unordered_set<std::string> _indexChunks;
    IndexChunk _currIndexChunk;
    int _numDocuments;
    size_t _bytesRequired;
    size_t _totalIndexSize;
};
