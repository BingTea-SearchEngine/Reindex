#pragma once

#include <unordered_map>
#include <unordered_set>
#include <iostream>

using std::cout, std::endl;

#include "PostingList.hpp"
#include "PostEntry.hpp"

class IndexChunk {
   public:
    static void Serialize(const char* buf, const IndexChunk& indexChunk);

    static IndexChunk Deserailize(const char* buf);

    IndexChunk();

    size_t getBytesRequired();

    // Iterates through all words in the document and adds document to posting list of the word
    void addDocument(std::string doc, std::vector<word_t> words);

   private:
    std::unordered_set<std::string> _documents;
    // Need to be built at serial time
    std::unordered_map<std::string, uint32_t> _postingListOffsets;
    std::unordered_map<std::string, PostingList> _postingLists;
    // Bytes Required to serialize this index
    size_t _bytesRequired;
    // Offset out of all documents
    uint32_t _offset;
};
