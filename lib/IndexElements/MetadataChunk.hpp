#pragma once

#include <unordered_map>
#include <iostream>

using std::cout, std::endl;

#include "WordLocation.hpp"

class MetadataChunk {
public:
    /*
     * @brief Default constructor for an MetadataChunk
     * */
    MetadataChunk();

    /*
     * @brief Gets the number of bytes required to serialize this MetadataChunk
     *
     * @return size_t
     * */
    size_t GetBytesRequired();

    /*
     * @brief Adds a document and its metadata to the current chunk being created
     *
     * @param doc The name of the document
     * @param metadata The metadata describing the document
     * */
    void AddDocument(std::string doc, metadata_t metadata);

    /*
     * @brief Gets the documents included in this chunk
     *
     * @return vector<std::string> of documents included in this chunk
     * */
    std::vector<std::string> GetDocuments();

    /*
     * @brief Get the metadata for a document
     * 
     * @param The document to look for
     * @return metadata for that document
     * */
    metadata_t GetMetadata(std::string doc);

    /*
     * @brief Prints the contents of the MetadataChunk
     * */
    void Print() const;

    /**
     * @brief Serializes a given MetadataChunk object into a specific region of memory.
     *
     * @param base_region A pointer to the beginning of the contiguous memory region 
     *                    where serialization will occur. This is determined by the
     *                    uppermost parent that wants serialization. This contiguous
     *                    memory region is memory-mapped to disk.
     * @param offset A reference to an offset value. After serialization, 
     *               this will be updated to point to the next available memory region.
     * @param chunk The MetadataChunk object to serialize.
     *
     * @pre `base_region` must be a valid pointer to a memory region that is mmap'ed.
     * @pre `offset` must be a valid number such that `base_region + offset`
     *       points to the target serialization location. For serialization of a IndexChunk, offset
     *       should always be 0
     * 
     * @post Writes the bytes of the MetadataChunk object into memory at the calculated region.
     * @post Updates `offset` to the next available memory location.
     * */
    static void Serialize(char* base_region, size_t& offset, MetadataChunk& chunk);

    /**
     * @brief Deserializes an MetadataChunk object from a specific region of memory.
     *
     * @param base_region A pointer to the beginning of the memory region containing the object.
     *                    This memory region is memory-mapped to disk.
     * @param offset A reference to an offset value. After deserialization, 
     *               this will be updated to point to the next available memory region. For
     *               deserialiation of a MasterChunk, offset should always be 0
     * @return The deserialized MetadataChunk object.
     *
     * @pre `base_region + offset` must point to a valid serialized MasterChunk object.
     * @post A MetadataChunk object is created and the offset is updated.
     * */
    static MetadataChunk Deserailize(char* base_region, size_t& offset);

private:

    // list of docs
    std::vector<std::string> _documents;
    // doc to metadata map
    std::unordered_map<std::string, metadata_t> _docMetadata;
    // Estimation of bytes required to serialize this metadata chunk. Need to test if it is accurate
    size_t _bytesRequired;

};