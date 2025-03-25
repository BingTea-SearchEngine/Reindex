#include "MasterChunk.hpp"

MasterChunk::MasterChunk(std::string outputDir, size_t chunkSize) : _outputDir(outputDir), _chunkSize(chunkSize)  {}

size_t MasterChunk::Serialize(const char* buffer, const MasterChunk& master) {
    return 100;
}

MasterChunk MasterChunk::Deserailize(char* buffer) {
    return MasterChunk("", 500);
}

std::vector<std::string> MasterChunk::GetChunkList() {
    return _indexChunks;
}

int MasterChunk::GetNumDocuments() {
    return _numDocuments;
}

void MasterChunk::addDocument(std::string doc, std::vector<word_t> words) {
    _numDocuments++;
    _currIndexChunk.AddDocument(doc, words);

    // Check if index will become too big
    // If too big write to disk and reinitialize _currIndexChunk
    if (_currIndexChunk.GetBytesRequired() > _chunkSize) {
        std::string chunkFilePath = _outputDir + "/" + std::to_string(_indexChunks.size());
        _indexChunks.push_back(chunkFilePath);
        // Allocate mmap buffer and serialize Index chunk
        // IndexChunk::Serialize(buf, _currIndexChunk);
        _currIndexChunk = IndexChunk();
    }
    _currIndexChunk.addDocument(doc, words);
}
    