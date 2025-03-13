#include "IndexChunk.hpp"

IndexChunk::IndexChunk() {

}

void IndexChunk::addDocument(docname doc, words words) {
    // Add to set of documents
    
    // Iterate over words
    for (word_t& word : words) {
        word.offset = _offset;
        cout << word << endl;
        _postingLists[word.word].addWord(doc, word);
        _offset++;
    }
}
