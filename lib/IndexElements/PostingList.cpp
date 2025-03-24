#include "PostingList.hpp"

PostingList::PostingList() {}

PostingList::PostingList(const std::string& _word) : word(_word) {}

size_t PostingList::GetOverheadBytes() {
    size_t ret = 0;
    ret += sizeof(_posts.size());
    ret += word.size() + 1; // Word and null terminator
    return ret;
}; 

size_t PostingList::AddWord(docname doc, word_t word) {
    // If docname is different from current post's document, add to back of vector
    if(_posts.empty() || doc != _posts.back().document) {
        _posts.emplace_back(doc);
    }
    return _posts.back().AddWord(word);
}

Post PostingList::GetPost(size_t index) {
    return _posts[index];
}

std::vector<Post>::iterator PostingList::Begin() {
    return _posts.begin();
}

std::vector<Post>::iterator PostingList::End() {
    return _posts.end();
}
