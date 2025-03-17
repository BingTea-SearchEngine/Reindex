#include "IndexChunk.hpp"

IndexChunk::IndexChunk() {

}

void IndexChunk::addDocument(docname doc, words words) {
    // Add to set of documents
    
    // Iterate over words
    for (word_t& word : words) {
        word.offset = _offset;
        cout << word << endl;
        if(_postingLists.find(word.word) == _postingLists.end()){
            _postingLists.insert(make_pair(word.word, PostingList(word.word)));
            _bytesRequired += _postingLists.at(word.word).getOverheadBytes(); 
        }
        _bytesRequired += _postingLists[word.word].addWord(doc, word);
        _offset++;
    }
}
