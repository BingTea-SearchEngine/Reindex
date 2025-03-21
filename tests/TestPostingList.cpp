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
    word_t w1 = {"word1", 0, wordlocation_t::title};
    word_t w2 = {"word2", 1, wordlocation_t::body};
    word_t w3 = {"word1", 2, wordlocation_t::bold};
    pl.addWord("doc1", w1);
    pl.addWord("doc1", w2);
    pl.addWord("doc2", w3);

    auto postsIterator = pl.begin();
    auto end = pl.end();
    auto wordIterator = postsIterator->begin();
    EXPECT_EQ(postsIterator->document, "doc1");
    EXPECT_EQ(wordIterator->word, w1.word);
    EXPECT_EQ(wordIterator->offset, w1.offset);
    EXPECT_EQ(wordIterator->location, w1.location);
    ++wordIterator;
    EXPECT_EQ(wordIterator->word, w2.word);
    EXPECT_EQ(wordIterator->offset, w2.offset);
    EXPECT_EQ(wordIterator->location, w2.location);

    ++postsIterator;
    wordIterator = postsIterator->begin();
    EXPECT_EQ(postsIterator->document, "doc2");
    EXPECT_EQ(wordIterator->word, w3.word);
    EXPECT_EQ(wordIterator->offset, w3.offset);
    EXPECT_EQ(wordIterator->location, w3.location);

    ++postsIterator;
    EXPECT_EQ(postsIterator, end);
}

TEST(BasicPostingList, TestBytesRequired) {
    PostingList pl("test");
    pl.addWord("doc1", word_t{"word1", 0, wordlocation_t::title});
    pl.addWord("doc1", word_t{"word2", 1, wordlocation_t::body});
}
