#include "PostingList.hpp"

PostingList::PostingList() {}

PostingList::PostingList(const std::string& _word) : word(_word) {}

size_t PostingList::getOverheadBytes() {
    size_t ret;
    ret += sizeof(_posts.size());
    ret += word.size() + 1; // Word and null terminator
    return ret;
}; 

size_t PostingList::addWord(docname doc, word_t word) {
    // If docname is different from current post's document, add to back of vector
    if(doc != _posts.back().document) {
        _posts.emplace_back(doc);
    }
    return _posts.back().addWord(word);
}

Post PostingList::getPost(size_t index) {
    return _posts[index];
}

std::vector<Post>::iterator PostingList::begin() {
    return _posts.begin();
}

std::vector<Post>::iterator PostingList::end() {
    return _posts.end();
}
