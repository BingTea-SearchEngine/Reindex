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

void MasterChunk::AddDocument(std::string doc, std::vector<word_t> words, metadata_t metadata) {
    _numDocuments++;
    // Check if index will become too big
    // If too big write to disk and reinitialize _currIndexChunk
    if (_currIndexChunk.GetBytesRequired() > _chunkSize) {
        Flush();
    }
    _currIndexChunk.AddDocument(doc, words);
    _currMetadataChunk.AddDocument(doc, metadata);
}

void MasterChunk::Flush() {
    _serializeCurrIndexChunk();
    _currIndexChunk = IndexChunk();
}

void MasterChunk::PrintCurrentIndexChunk() const {
    cout << "---------- Index Chunk " << _indexChunks.size() << " ----------" << endl;
    _currIndexChunk.Print();
    cout << "---------- Index Chunk " << _indexChunks.size() << " ----------" << endl;
}
    
void MasterChunk::_serializeCurrIndexChunk() {
    std::string chunkFilePath = _outputDir + "/" + std::to_string(_indexChunks.size());
    cout << chunkFilePath << endl;
    _indexChunks.push_back(chunkFilePath);

    int fd = -1;
    // Allocate times two of chunk size to be safe
    void* base_region = create_mmap_region(fd, _chunkSize*2, chunkFilePath);
    assert(fd != -1);

    size_t offset = 0;
    IndexChunk::Serialize(static_cast<char*>(base_region), offset, _currIndexChunk);
    munmap(base_region, _chunkSize*2);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);
}
