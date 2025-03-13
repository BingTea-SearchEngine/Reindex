#include "PostingList.hpp"

PostingList::PostingList() {

}

void PostingList::addWord(docname doc, word_t word) {
    // If docname is different from _currPost.document, add _currPost to vector of Posts and
    // initialize a new Post instance

    // Add word to _currPost
    _currPost.addWord(word);
}
