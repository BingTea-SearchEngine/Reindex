#include <gtest/gtest.h>

#include "PostingList.hpp"
#include "Types.hpp"

TEST(BasicPostingList, TestConstructor) {
    PostingList pl("hello");
    EXPECT_EQ(pl.word, "hello");
    EXPECT_EQ(pl.getOverheadBytes(), 10);
}

TEST(BasicPostingList, TestAddWord) {
    PostingList pl("test");
    postentry_t w1 = {0, wordlocation_t::title};
    postentry_t w2 = {1, wordlocation_t::body};
    postentry_t w3 = {2, wordlocation_t::bold};
    pl.addWord("doc1", w1);
    pl.addWord("doc1", w2);
    pl.addWord("doc2", w3);

    auto postsIterator = pl.begin();
    auto end = pl.end();
    auto wordIterator = postsIterator->begin();
    EXPECT_EQ(postsIterator->document, "doc1");
    EXPECT_EQ(wordIterator->offset, w1.offset);
    EXPECT_EQ(wordIterator->location, w1.location);
    ++wordIterator;
    EXPECT_EQ(wordIterator->offset, w2.offset);
    EXPECT_EQ(wordIterator->location, w2.location);

    ++postsIterator;
    wordIterator = postsIterator->begin();
    EXPECT_EQ(postsIterator->document, "doc2");
    EXPECT_EQ(wordIterator->offset, w3.offset);
    EXPECT_EQ(wordIterator->location, w3.location);

    ++postsIterator;
    EXPECT_EQ(postsIterator, end);
}

TEST(BasicPostingList, TestBytesRequired) {
    PostingList pl("test");
    pl.addWord("doc1", postentry_t{0, wordlocation_t::title});
    pl.addWord("doc1", postentry_t{1, wordlocation_t::body});
}
