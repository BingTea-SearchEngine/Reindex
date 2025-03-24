#include "IndexChunk.hpp"

IndexChunk::IndexChunk() : _bytesRequired(0), _offset(0) {
}

size_t IndexChunk::GetBytesRequired() {
    return _bytesRequired;
}

void IndexChunk::AddDocument(docname doc, words words) {
    // Add to set of documents
    
    // Iterate over words
    for (word_t& word : words) {
        word.offset = _offset;
        cout << word << endl;
        if(_postingLists.find(word.word) == _postingLists.end()){
            _postingLists[word.word] = PostingList(word.word);
            _bytesRequired += _postingLists.at(word.word).GetOverheadBytes(); 
        }
        _bytesRequired += _postingLists[word.word].AddWord(doc, word);
        _offset++;
    }
}
