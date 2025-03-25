#include "IndexChunk.hpp"
//
IndexChunk::IndexChunk() {}

IndexChunk::IndexChunk() : _bytesRequired(0), _offset(0) {
}

size_t IndexChunk::GetBytesRequired() {
    return _bytesRequired;
}

void IndexChunk::addDocument(std::string doc, std::vector<word_t> words) {
    // Add to set of documents

    // Iterate over words
    for (word_t& word : words) {
        cout << word << endl;
        if(_postingLists.find(word.word) == _postingLists.end()){
            _postingLists.insert(make_pair(word.word, PostingList(word.word)));
            // _bytesRequired += _postingLists.at(word.word).getOverheadBytes();
        }
        _postingLists[word.word].addWord(doc, PostEntry(_offset, word.location));
        _offset++;
    }
}
