#include "IndexChunk.hpp"

IndexChunk::IndexChunk() : _bytesRequired(0), _offset(0) {
}

size_t IndexChunk::GetBytesRequired() {
    return _bytesRequired;
}

void IndexChunk::Print() const {
    cout << _documents.size() << " Documents: ";
    for (std::string doc : _documents) {
        cout << doc << " ";
    }
    cout << endl;
    cout << _postingLists.size() << " Words" << endl;
    for (auto [word, postingList] : _postingLists) {
        postingList.Print();
    }
}

void IndexChunk::AddDocument(std::string doc, std::vector<word_t> words) {
    // Add to set of documents
    _documents.insert(doc);

    // Iterate over words
    for (word_t& word : words) {
        if(_postingLists.find(word.word) == _postingLists.end()){
            _postingLists.insert(make_pair(word.word, PostingList(word.word)));
            _bytesRequired += _postingLists.at(word.word).GetOverheadBytesRequired();
        }
        _bytesRequired+=_postingLists[word.word].AddWord(doc, PostEntry(_offset, word.location));
        _offset++;
    }
}
