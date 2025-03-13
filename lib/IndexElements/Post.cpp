#include "Post.hpp"

Post::Post(docname name) : document(name) {
}

void Post::addWord(word_t word) {
    _entries.push_back(word);
}
