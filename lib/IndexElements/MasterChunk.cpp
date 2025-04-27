#include "MasterChunk.hpp"

MasterChunk::MasterChunk() {}

MasterChunk::MasterChunk(std::string outputDir, size_t chunkSize)
    : _indexDir(outputDir + "/index/"),
      _metadataDir(outputDir + "/metadata/"),
      _chunkSize(chunkSize),
      _numDocuments(0) {
    std::filesystem::create_directories(_indexDir);
    std::filesystem::create_directories(_metadataDir);
    assert(std::filesystem::exists(_indexDir) && std::filesystem::is_directory(_indexDir));
    assert(std::filesystem::exists(_metadataDir) && std::filesystem::is_directory(_metadataDir));
    _currIndexChunk = std::make_unique<IndexChunk>();
}

void MasterChunk::Serialize(char* baseRegion, size_t& offset, MasterChunk& master) {
    assert(offset == 0);
    // Serialize number of documents
    std::memcpy(baseRegion + offset, &master._numDocuments, sizeof(master._numDocuments));
    offset += sizeof(master._numDocuments);

    // Serialize threshold chunk size
    std::memcpy(baseRegion + offset, &master._chunkSize, sizeof(master._chunkSize));
    offset += sizeof(master._chunkSize);

    // Serialize index output directory
    size_t indexDirectorySize = master._indexDir.size() + 1;
    std::memcpy(baseRegion + offset, master._indexDir.c_str(), indexDirectorySize);
    offset += indexDirectorySize;

    // Serialize metadata output directory
    size_t metadataDirectorySize = master._metadataDir.size() + 1;
    std::memcpy(baseRegion + offset, master._metadataDir.c_str(), metadataDirectorySize);
    offset += metadataDirectorySize;

    // Serialize size of index chunk list (should be the same for index and master chunks since they are flushed together)
    size_t numChunks = master._indexChunks.size();
    std::memcpy(baseRegion + offset, &numChunks, sizeof(numChunks));
    offset += sizeof(numChunks);

    // Serialize chunk list
    for (std::string& chunkName : master._indexChunks) {
        size_t chunkNameSize = chunkName.size() + 1;
        std::memcpy(baseRegion + offset, chunkName.c_str(), chunkNameSize);
        offset += chunkNameSize;
    }

    // Serialize metadata chunk list
    for (std::string& chunkName : master._metadataChunks) {
        size_t chunkNameSize = chunkName.size() + 1;
        std::memcpy(baseRegion + offset, chunkName.c_str(), chunkNameSize);
        offset += chunkNameSize;
    }
}

MasterChunk MasterChunk::Deserailize(char* baseRegion, size_t& offset) {
    assert(offset == 0);
    MasterChunk master;
    // Read number of documents
    std::memcpy(&master._numDocuments, baseRegion + offset, sizeof(master._numDocuments));
    offset += sizeof(master._numDocuments);

    // Read threshold chunk size
    std::memcpy(&master._chunkSize, baseRegion + offset, sizeof(master._chunkSize));
    offset += sizeof(master._chunkSize);

    // Read index output directory
    master._indexDir = std::string(baseRegion + offset);
    offset += master._indexDir.size() + 1;

    // Read metadata output directory
    master._metadataDir = std::string(baseRegion + offset);
    offset += master._metadataDir.size() + 1;

    // Read size of chunk list
    size_t numChunks = 0;
    std::memcpy(&numChunks, baseRegion + offset, sizeof(numChunks));
    offset += sizeof(numChunks);

    // Read chunk list
    for (size_t i = 0; i < numChunks; ++i) {
        std::string chunkName = std::string(baseRegion + offset);
        offset += chunkName.size() + 1;
        master._indexChunks.push_back(chunkName);
    }

    // Read metadata chunk list
    for (size_t i = 0; i < numChunks; ++i) {
        std::string chunkName = std::string(baseRegion + offset);
        offset += chunkName.size() + 1;
        master._metadataChunks.push_back(chunkName);
    }

    return master;
}

std::vector<std::string> MasterChunk::GetChunkList() {
    return _indexChunks;
}

std::vector<std::string> MasterChunk::GetMetadataChunkList() {
    return _metadataChunks;
}

int MasterChunk::GetNumDocuments() {
    return _numDocuments;
}

void MasterChunk::AddDocument(std::string doc, std::vector<word_t> words, metadata_t metadata) {
    // spdlog::info("Adding {}", doc);
    // Check if index will become too big
    // If too big write to disk and reinitialize _currIndexChunk
    cout << metadata.docNum << endl;
    if (_currIndexChunk->GetBytesRequired() > _chunkSize ||
        _currMetadataChunk.GetBytesRequired() > _chunkSize) {
        Flush();
    }
    metadata.docStartOffset = _currIndexChunk->GetCurrentOffset();
    _currIndexChunk->AddDocument(doc, words);
    cout << "Done index add" << endl;
    metadata.docEndOffset = _currIndexChunk->GetCurrentOffset();
    cout << "Done meta add" << endl;
    _currMetadataChunk.AddDocument(doc, metadata);
    _numDocuments++;
    if (_numDocuments % 10000 == 0) {
        spdlog::info("{} indexed", _numDocuments);
    }
}

void MasterChunk::Flush() {
    spdlog::info("Flushing");
    spdlog::info("Offset reached {}", _currIndexChunk->_offset);
    _serializeCurrIndexChunk();
    _serializeCurrMetadataChunk();
    _currIndexChunk.reset(new IndexChunk());
    _currMetadataChunk = MetadataChunk();
    spdlog::info("Done Flushing, number of index chunks: {}", _indexChunks.size());
    spdlog::info("Number of documents indexed so far {}", _numDocuments);
}

size_t MasterChunk::NumChunks() {
    assert(_indexChunks.size() == _metadataChunks.size());
    return _indexChunks.size();
}

void MasterChunk::PrintCurrentIndexChunk() const {
    cout << "---------- Index Chunk " << _indexChunks.size() << " ----------" << endl;
    _currIndexChunk->Print();
    cout << "---------- Index Chunk " << _indexChunks.size() << " ----------" << endl;
}

void MasterChunk::PrintCurrentMetadataChunk() const {
    cout << "---------- Metadata Chunk " << _metadataChunks.size() << " ----------" << endl;
    _currMetadataChunk.Print();
    cout << "---------- Metadata Chunk " << _metadataChunks.size() << " ----------" << endl;
}

std::unique_ptr<IndexChunk> MasterChunk::GetIndexChunk(size_t i) {
    assert(i < _indexChunks.size());
    std::string indexFilePath = _indexChunks[i];
    int fd = -1;
    auto [buf, size] = read_mmap_region(fd, indexFilePath);
    size_t offset = 0;
    std::unique_ptr<IndexChunk> chunk = IndexChunk::Deserailize(static_cast<char*>(buf), offset);
    munmap(buf, size);
    close(fd);
    return chunk;
}

std::unique_ptr<MetadataChunk> MasterChunk::GetMetadataChunk(size_t i) {
    assert(i < _metadataChunks.size());
    std::string indexFilePath = _metadataChunks[i];
    int fd = -1;
    auto [buf, size] = read_mmap_region(fd, indexFilePath);
    size_t offset = 0;
    MetadataChunk chunk = MetadataChunk::Deserailize(static_cast<char*>(buf), offset);
    munmap(buf, size);
    close(fd);
    return std::make_unique<MetadataChunk>(chunk);
}

void MasterChunk::_serializeCurrIndexChunk() {
    std::string chunkFilePath = _indexDir + std::to_string(_indexChunks.size());
    _indexChunks.push_back(chunkFilePath);

    int fd = -1;
    // Allocate times two of chunk size to be safe
    void* base_region = create_mmap_region(fd, _chunkSize * 2, chunkFilePath);
    assert(fd != -1);

    size_t offset = 0;
    IndexChunk::Serialize(static_cast<char*>(base_region), offset, *_currIndexChunk);
    munmap(base_region, _chunkSize * 2);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);
}

void MasterChunk::_serializeCurrMetadataChunk() {
    std::string chunkFilePath = _metadataDir + std::to_string(_metadataChunks.size());
    _metadataChunks.push_back(chunkFilePath);

    int fd = -1;
    // Allocate times two of chunk size to be safe
    void* base_region = create_mmap_region(fd, _chunkSize * 2, chunkFilePath);
    assert(fd != -1);

    size_t offset = 0;
    MetadataChunk::Serialize(static_cast<char*>(base_region), offset, _currMetadataChunk);
    munmap(base_region, _chunkSize * 2);
    if (ftruncate(fd, offset) == -1) {
        perror("Error truncating file");
    }
    close(fd);
}
