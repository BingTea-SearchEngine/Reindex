#include <cassert>
#include <cstring>

#include "IndexChunk.hpp"

IndexChunk::IndexChunk() {
    _bytesRequired = 0;
    _offset = 0;
    documentID = 0;
}

size_t IndexChunk::GetBytesRequired() {
    return _bytesRequired;
}

void IndexChunk::Print() const {
    cout << docID_to_doc_name.size() << " Documents: ";
    for (const auto& [id, doc] : docID_to_doc_name) {
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
    // _documents.push_back(doc);

    docID_to_doc_name[documentID] = doc;

    // Iterate over words
    for (word_t& word : words) {
        if (_postingLists.find(word.word) == _postingLists.end()) {
            _postingLists.insert(make_pair(word.word, PostingList(word.word)));
            _bytesRequired += _postingLists.at(word.word).GetOverheadBytesRequired();
        }
        _bytesRequired +=
            _postingLists[word.word].AddWord(documentID, PostEntry(_offset, word.location));
        _offset++;
        assert(_offset < UINT32_MAX);
    }

    documentID++;
}

std::unordered_map<uint32_t, std::string> IndexChunk::GetDocuments() {
    return docID_to_doc_name;
}

const PostingList& IndexChunk::GetPostingList(std::string word) {
    return _postingLists[word];
}

const std::unordered_map<std::string, PostingList>& IndexChunk::GetAllPostingLists() const {
    return _postingLists;
}

uint32_t IndexChunk::GetCurrentOffset() {
    return _offset;
}

std::string IndexChunk::GetDocName(uint32_t docId) const {
    return docID_to_doc_name.at(docId);
}

void IndexChunk::Serialize(char* base_region, size_t& offset, IndexChunk& index) {
    assert(offset == 0);

    // serialize the docID_to_doc_name

    // first, the size of this map

    // then, each key value pair

    size_t num_mappings = index.docID_to_doc_name.size();
    std::memcpy(base_region + offset, &num_mappings, sizeof(num_mappings));
    offset += sizeof(num_mappings);

    for (auto& [ID, docName] : index.docID_to_doc_name) {
        // the 4-byte integer
        std::memcpy(base_region + offset, &ID, sizeof(ID));
        offset += sizeof(ID);

        // the length of the string
        size_t document_name_size = docName.size();
        std::memcpy(base_region + offset, &document_name_size, sizeof(document_name_size));
        offset += sizeof(document_name_size);

        // the actual string
        std::memcpy(base_region + offset, docName.c_str(), document_name_size);
        offset += document_name_size;
    }

    // Serialize number of words
    size_t num_words = index._postingLists.size();
    std::memcpy(base_region + offset, &num_words, sizeof(num_words));
    offset += sizeof(num_words);

    // Serailize each posting list
    for (auto& [word, posting_list] : index._postingLists) {
        PostingList::Serialize(base_region, offset, posting_list);
    }
}

IndexChunk IndexChunk::Deserailize(char* base_region, size_t& offset) {
    assert(offset == 0);
    IndexChunk index;

    // Read docID_to_doc_name
    size_t num_mappings = 0;
    std::memcpy(&num_mappings, base_region + offset, sizeof(num_mappings));
    cout << "num mappings " << num_mappings << endl;
    offset += sizeof(num_mappings);

    for (size_t _ = 0; _ < num_mappings; ++_) {
        uint32_t ID = 0;
        std::memcpy(&ID, base_region + offset, sizeof(ID));
        offset += sizeof(ID);

        size_t doc_name_size = 0;
        std::memcpy(&doc_name_size, base_region + offset, sizeof(doc_name_size));
        offset += sizeof(doc_name_size);
        cout << "doc name sizeu " << doc_name_size << endl;

        // CHECK BACK HERE LATER //////////////////////////////////////////////////////////////////////////////
        std::string actualDocName(doc_name_size, '\0');
        std::memcpy(actualDocName.data(), base_region + offset, doc_name_size);
        offset += doc_name_size;

        // insert the entry
        index.docID_to_doc_name[ID] = actualDocName;
    }

    // Read number of words
    size_t num_words = 0;
    std::memcpy(&num_words, base_region + offset, sizeof(num_words));
    offset += sizeof(num_words);
    cout << "num words " << num_words << endl;

    //Read each posting list
    for (size_t i = 0; i < num_words; ++i) {
        PostingList pl = PostingList::Deserialize(base_region, offset);
        // index._postingLists[pl.GetWord()] = std::move(pl);
        index._postingLists.emplace(pl.GetWord(), std::move(pl));
    }

    return std::move(index);
}
