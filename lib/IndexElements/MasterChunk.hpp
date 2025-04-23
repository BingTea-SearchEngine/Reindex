#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>


#include "IndexChunk.hpp"
#include "MetadataChunk.hpp"
#include "Util.hpp"
#include "WordLocation.hpp"

/**
 * @class MasterChunk
 * @brief A manager of the entire index and all of its chunks. 
 *
 * Stores the filepaths of all index chunks and the number of documents indexed.
 */
class MasterChunk {
   public:
    /*
     * @brief Constructs a master chunk with the output directory of all index chunks and the
     * threshold size in bytes of each chunk before it is written to disk
     * */
    MasterChunk(std::string outputDir, size_t chunkSize);

    /*
     * @brief Gets the file paths of index chunks in a vector
     *
     * @return Vector of file paths to index chunks
     * */
    std::vector<std::string> GetChunkList();

    /*
     * @brief Gets the file paths of metadadta chunks in a vector
     *
     * @return Vector of file paths to metadata chunks
     * */
    std::vector<std::string> GetMetadataChunkList();

    /*
     * @brief Adds a document to the index. The document and words in the document are propogated
     * into the current index chunk that is being built
     *
     * @param doc The name of the document
     * @param words The words in the document in a vector
     * */
    void AddDocument(std::string doc, std::vector<word_t> words, metadata_t metadata);

    /*
     * @brief Gets the number of documents stored by this index
     *
     * @return int of number of documents in this index
     * */
    int GetNumDocuments();

    /*
     * @brief Writes the current index chunk being built into disk ignoring the threshold size
     * passed into the constructor
     * */
    void Flush();

    /*
     * @brief Prints the current index chunk
     * */
    void PrintCurrentIndexChunk() const;

    /*
     * @brief Prints the current metadata chunk
     * */
    void PrintCurrentMetadataChunk() const;

    /*
     * @brief Return the index chunk at index i of _indexChunks
     *
     * @param i The index of the index. Must be less than the size of _indexChunks
     * */
    std::unique_ptr<IndexChunk> GetIndexChunk(size_t i);

    /*
     * @brief Return the metadata chunk at index i of metadata chunk
     *
     * @param i The index of the index. Must be less than the size of _medataChunks
     * */
    std::unique_ptr<MetadataChunk> GetMetadataChunk(size_t i);

    /*
     * @brief Return the number of chunks in this index server
     *
     * */
    size_t NumChunks();

    /**
     * @brief Serializes a given MasterChunk object into a specific region of memory.
     *
     * @param baseRegion A pointer to the beginning of the contiguous memory region 
     *                    where serialization will occur. This is determined by the
     *                    uppermost parent that wants serialization. This contiguous
     *                    memory region is memory-mapped to disk.
     * @param offset A reference to an offset value. After serialization, 
     *               this will be updated to point to the next available memory region.
     * @param master The MasterChunk object to serialize.
     *
     * @pre `baseRegion` must be a valid pointer to a memory region that is mmap'ed.
     * @pre `offset` must be a valid number such that `base_region + offset`
     *       points to the target serialization location. For serialization of a MasterChunk, offset
     *       should always be 0
     * 
     * @post Writes the bytes of the MasterChunk object into memory at the calculated region.
     * @post Updates `offset` to the next available memory location.
     * */
    static void Serialize(char* baseRegion, size_t& offset, MasterChunk& master);

    /**
     * @brief Deserializes a MasterChunk object from a specific region of memory.
     *
     * @param base_region A pointer to the beginning of the memory region containing the object.
     *                    This memory region is memory-mapped to disk.
     * @param offset A reference to an offset value. After deserialization, 
     *               this will be updated to point to the next available memory region. For
     *               deserialiation of a MasterChunk, offset should always be 0
     * @return The deserialized MaseterChunk object.
     *
     * @pre `base_region + offset` must point to a valid serialized MasterChunk object.
     * @post A MasterChunk object is created and the offset is updated.
     * */
    static MasterChunk Deserailize(char* baseRegion, size_t& offset);

   private:
    /*
     * @brief Default constructor only to be used in the Deserialize method
     * */
    MasterChunk();

    /*
     * @brief Serialize the current index chunk being built by calling the Serialize method on
     * current index chunk
     * */
    void _serializeCurrIndexChunk();

    void _serializeCurrMetadataChunk();

    // List of index chunks and their file paths
    std::vector<std::string> _indexChunks;
    // List of metadata chunks and their file paths
    std::vector<std::string> _metadataChunks;
    // The output directory of the index chunks
    // std::string _outputDir;
    std::string _indexDir;
    std::string _metadataDir;
    // The current index chunk being built
    std::unique_ptr<IndexChunk> _currIndexChunk;
    // The current metadata chunk being built
    MetadataChunk _currMetadataChunk;
    // The threshold size of an index chunk
    size_t _chunkSize;
    // The number of documents indexed
    int _numDocuments;
};

