#include <cassert>
#include <cstring>

#include "IndexChunk.hpp"

IndexChunk::IndexChunk() {
    _bytesRequired = 0;
    _offset = 0;
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
    _documents.push_back(doc);

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

std::vector<std::string> IndexChunk::GetDocuments() {
    return _documents;
}

PostingList IndexChunk::GetPostingList(std::string word) {
    return _postingLists[word];
}

void IndexChunk::Serialize(char* base_region, size_t& offset, IndexChunk& index) {
    assert(offset == 0);

    // Serialize document list size
    size_t num_documents = index._documents.size();
    std::memcpy(base_region+offset, &num_documents, sizeof(num_documents));
    offset+=sizeof(num_documents);

    // Serialize document list
    for (std::string& docname : index._documents) {
        size_t docname_size = docname.size()+1;
        std::memcpy(base_region+offset, docname.c_str(), docname_size);
        offset+=docname_size;
    }

    // Serialize number of words
    size_t num_words = index._postingLists.size();
    std::memcpy(base_region+offset, &num_words, sizeof(num_words));
    offset+=sizeof(num_words);

    // Serailize each posting list
    for (auto& [word, posting_list] : index._postingLists) {
        PostingList::Serialize(base_region, offset, posting_list);
    }
}

IndexChunk IndexChunk::Deserailize(char* base_region, size_t& offset){
    assert(offset == 0);
    IndexChunk index;

    // Read document list size
    size_t num_documents = 0;
    std::memcpy(&num_documents, base_region+offset, sizeof(num_documents));
    offset+=sizeof(num_documents);

    // Read chunk list
    for (size_t i = 0; i < num_documents; ++i) {
        std::string docname = std::string(base_region+offset);
        offset+=docname.size()+1;
        index._documents.push_back(docname);
    }

    // Read number of words
    size_t num_words = 0;
    std::memcpy(&num_words, base_region+offset, sizeof(num_words));
    offset+=sizeof(num_words);

    //Read each posting list
    for (size_t i = 0; i < num_words; ++i) {
        PostingList pl = PostingList::Deserialize(base_region, offset);
        index._postingLists[pl.GetWord()] = pl;
    }

    return index;
}
