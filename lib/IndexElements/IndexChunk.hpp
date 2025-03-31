#pragma once

#include <unordered_map>
#include <iostream>

using std::cout, std::endl;

#include "PostingList.hpp"
#include "PostEntry.hpp"

class IndexChunk {
   public:

    IndexChunk();

    size_t GetBytesRequired();

    // Iterates through all words in the document and adds document to posting list of the word
    void AddDocument(std::string doc, std::vector<word_t> words);

    std::vector<std::string> GetDocuments();

    /*
     * @brief Prints the contents of the IndexChunk
     * */
    void Print() const;

    static void Serialize(char* base_region, size_t& offset, IndexChunk& index);

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
