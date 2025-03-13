#include "IndexChunk.hpp"

IndexChunk::IndexChunk() {

}

void IndexChunk::addDocument(docname doc, words words) {
    // Add to set of documents
    
    // Iterate over words
    for (const word_t& word : words) {
        _postingLists[word.word].addWord(doc, word);
    }
}
