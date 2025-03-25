#include "MasterChunk.hpp"

MasterChunk::MasterChunk() {}

MasterChunk::MasterChunk(std::string outputDir, size_t chunkSize) : _outputDir(outputDir), _chunkSize(chunkSize)  {
    _numDocuments = 0;
}

void MasterChunk::Serialize(char* baseRegion, size_t& offset, MasterChunk& master) {
    assert(offset == 0);
    // Serialize number of documents
    std::memcpy(baseRegion+offset, &master._numDocuments, sizeof(master._numDocuments));
    offset+=sizeof(master._numDocuments);

    // Serialize threshold chunk size
    std::memcpy(baseRegion+offset, &master._chunkSize, sizeof(master._chunkSize));
    offset+=sizeof(master._chunkSize);

    // Serialize output directory
    size_t outputDirectorySize = master._outputDir.size() + 1;
    std::memcpy(baseRegion+offset, master._outputDir.c_str(), outputDirectorySize);
    offset+=outputDirectorySize;

    // Serialize size of chunk list
    size_t numChunks = master._indexChunks.size();
    std::memcpy(baseRegion+offset, &numChunks, sizeof(numChunks));
    offset+=sizeof(numChunks);

    // Serialize chunk list
    for (std::string& chunkName : master._indexChunks) {
        size_t chunkNameSize = chunkName.size()+1;
        std::memcpy(baseRegion+offset, chunkName.c_str(), chunkNameSize);
        offset+=chunkNameSize;
    }
}

MasterChunk MasterChunk::Deserailize(char* baseRegion, size_t& offset) {
    assert(offset==0);
    MasterChunk master;
    // Read number of documents
    std::memcpy(&master._numDocuments, baseRegion+offset, sizeof(master._numDocuments));
    offset+=sizeof(master._numDocuments);

    // Read threshold chunk size
    std::memcpy(&master._chunkSize, baseRegion+offset, sizeof(master._chunkSize));
    offset+=sizeof(master._chunkSize);

    // Read output directory
    master._outputDir = std::string(baseRegion+offset);
    offset+=master._outputDir.size()+1;

    // Read size of chunk list
    size_t numChunks = 0;
    std::memcpy(&numChunks, baseRegion+offset, sizeof(numChunks));
    offset+=sizeof(numChunks);

    // Read chunk list
    for (size_t i = 0; i < numChunks; ++i) {
        std::string chunkName = std::string(baseRegion+offset);
        offset+=chunkName.size()+1;
        master._indexChunks.push_back(chunkName);
    }

    return master;
}

std::vector<std::string> MasterChunk::GetChunkList() {
    return _indexChunks;
}

int MasterChunk::GetNumDocuments() {
    return _numDocuments;
}

void MasterChunk::AddDocument(std::string doc, std::vector<word_t> words) {
    _numDocuments++;
    // Check if index will become too big
    // If too big write to disk and reinitialize _currIndexChunk
    if (_currIndexChunk.GetBytesRequired() > _chunkSize) {
        _serializeCurrIndexChunk();
        _currIndexChunk = IndexChunk();
    }
    _currIndexChunk.AddDocument(doc, words);
}

void MasterChunk::Flush() {
    _serializeCurrIndexChunk();
}
    
void MasterChunk::_serializeCurrIndexChunk() {
    std::string chunkFilePath = _outputDir + "/" + std::to_string(_indexChunks.size());
    _indexChunks.push_back(chunkFilePath);
    // Allocate mmap buffer and serialize Index chunk
    // IndexChunk::Serialize(buf, _currIndexChunk);
}
