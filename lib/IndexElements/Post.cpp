#include "Post.hpp"

Post::Post() {}

Post::Post(docname name) : document(name) {}

size_t Post::addWord(word_t word) {
    _entries.push_back(word);
    return word.getBytesRequired();
}

words::iterator Post::begin() {
    return _entries.begin();
}

words::iterator Post::end() {
    return _entries.end();
}

