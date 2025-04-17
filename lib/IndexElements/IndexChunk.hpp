#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>

using std::cout, std::endl;

#include "PostEntry.hpp"
#include "PostingList.hpp"

class IndexChunk {
   public:
    /*
     * @brief Default constructor for an Index Chunk
     * */
    IndexChunk();

    /*
     * @brief Gets the number of bytes required to serialize this Index Chunk
     *
     * @return size_t
     * */
    size_t GetBytesRequired();

    /*
     * @brief Adds a document to the current index being created. Iterates over the words vector and
     * adds the occurence into the posting list for that word
     *
     * @param doc The name of the document
     * @param words The words in the document in a vector
     * */
    void AddDocument(std::string doc, std::vector<word_t> words);

    /*
     * @brief Gets the documents included in this index
     *
     * @return vector<std::string> of documents included in this index
     * */
    std::vector<std::string> GetDocuments();

    /*
     * @brief Get the posting list for a word
     * 
     * @param The word to look for
     * @return PostingList for that word
     * */
    PostingList GetPostingList(std::string word);

    /*
     * @brief Prints the contents of the IndexChunk
     * */
    void Print() const;

    /**
     * @brief Serializes a given IndexChunk object into a specific region of memory.
     *
     * @param base_region A pointer to the beginning of the contiguous memory region 
     *                    where serialization will occur. This is determined by the
     *                    uppermost parent that wants serialization. This contiguous
     *                    memory region is memory-mapped to disk.
     * @param offset A reference to an offset value. After serialization, 
     *               this will be updated to point to the next available memory region.
     * @param master The IndexChunk object to serialize.
     *
     * @pre `base_region` must be a valid pointer to a memory region that is mmap'ed.
     * @pre `offset` must be a valid number such that `base_region + offset`
     *       points to the target serialization location. For serialization of a IndexChunk, offset
     *       should always be 0
     * 
     * @post Writes the bytes of the IndexChunk object into memory at the calculated region.
     * @post Updates `offset` to the next available memory location.
     * */
    static void Serialize(char* base_region, size_t& offset, IndexChunk& index);

    /**
     * @brief Deserializes an IndexChunk object from a specific region of memory.
     *
     * @param base_region A pointer to the beginning of the memory region containing the object.
     *                    This memory region is memory-mapped to disk.
     * @param offset A reference to an offset value. After deserialization, 
     *               this will be updated to point to the next available memory region. For
     *               deserialiation of a MasterChunk, offset should always be 0
     * @return The deserialized IndexChunk object.
     *
     * @pre `base_region + offset` must point to a valid serialized MasterChunk object.
     * @post A IndexChunk object is created and the offset is updated.
     * */
    static IndexChunk Deserailize(char* base_region, size_t& offset);

   private:
    // Set of documents in this index chunk
    std::vector<std::string> _documents;
    // Word to posting list map
    std::unordered_map<std::string, PostingList> _postingLists;
    // Estimation of bytes required to serialize this index chunk. Need to test if it is accurate
    size_t _bytesRequired;
    // Offset out of all documents in this chunk
    uint32_t _offset;
};
