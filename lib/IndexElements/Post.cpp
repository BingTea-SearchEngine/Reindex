#include "Post.hpp"

Post::Post() {}

Post::Post(docname name) : document(name) {}

size_t Post::AddWord(word_t word) {
    _entries.push_back(word);
    return word.GetBytesRequired();
}

words::iterator Post::Begin() {
    return _entries.begin();
}

words::iterator Post::End() {
    return _entries.end();
}

