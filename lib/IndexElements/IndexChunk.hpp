#pragma once

#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "PostingList.hpp"
#include "Types.hpp"

using std::cout;
using std::endl;


class IndexChunk {
   public:
    static void Serialize(const char* buf, const IndexChunk& indexChunk);

    static IndexChunk Deserailize(const char* buf);

    IndexChunk();

    size_t getBytesRequired();

    // Iterates through all words in the document and adds document to posting list of the word
    void addDocument(docname doc, words words);

   private:
    std::unordered_set<docname> _documents;
    // Need to be built at serial time
    std::unordered_map<docname, uint32_t> _postingListOffsets;
    std::unordered_map<std::string, PostingList> _postingLists;
    // Bytes Required to serialize this index
    size_t _bytesRequired;
    // Offset out of all documents
    uint32_t _offset;
};
