#include <gtest/gtest.h>

#include "PostingList.hpp"
#include "WordLocation.hpp"

TEST(BasicPostingList, TestConstructor) {
    PostingList pl("hello");
    EXPECT_EQ(pl.getWord(), "hello");
}

TEST(BasicPostingList, TestAddWord) {
    PostingList pl("test");
    PostEntry w1 = {0, wordlocation_t::title};
    PostEntry w2 = {1, wordlocation_t::body};
    PostEntry w3 = {2, wordlocation_t::bold};
    pl.addWord("doc1", w1);
    pl.addWord("doc1", w2);
    pl.addWord("doc2", w3);

    // Access posts using getPosts()
    auto posts = pl.getPosts();
    
    // First post
    EXPECT_EQ(posts[0].getDocumentName(), "doc1");
    auto wordEntries1 = posts[0].getEntries(); // Get entries for the first post
    EXPECT_EQ(wordEntries1[0].getDelta(), w1.getDelta());
    EXPECT_EQ(wordEntries1[0].getLocationFound(), w1.getLocationFound());
    EXPECT_EQ(wordEntries1[1].getDelta(), w2.getDelta());
    EXPECT_EQ(wordEntries1[1].getLocationFound(), w2.getLocationFound());

    // Second post
    EXPECT_EQ(posts[1].getDocumentName(), "doc2");
    auto wordEntries2 = posts[1].getEntries(); // Get entries for the second post
    EXPECT_EQ(wordEntries2[0].getDelta(), w3.getDelta());
    EXPECT_EQ(wordEntries2[0].getLocationFound(), w3.getLocationFound());
}
