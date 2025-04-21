#include <gtest/gtest.h>
#include <iostream>
#include "Expression.hpp"
#include "Parser.hpp"
#include "PostingList.hpp"

class QueryCompilerTest : public ::testing::Test {
   protected:
    std::unordered_map<std::string, PostingList> index;

    struct Document {
        std::string name;
        std::vector<std::string> words;
    };

    std::vector<Document> documents = {
        {"Document 1",
         {"i", "went", "to", "the", "store", "and", "grabbed", "some", "granola", "bar", "and",
          "then", "i", "went", "to", "another", "store"}},
        {"Document 2",
         {"costco", "is", "the", "best", "megastore", "it", "has", "so", "many", "granola", "and",
          "protein", "bar", "love", "costco"}},
        {"Document 3",
         {"i", "think", "the", "amazon", "online", "store", "is", "alright", "amazon", "is",
          "bananas"}},
        {"Document 4",
         {"mcdonalds", "has", "the", "best", "food", "and", "fulfills", "my", "protein", "goal",
          "bar", "none"}}};

    void SetUp() override {
        uint32_t word_counter = 0;
        uint32_t docID = 1;
        for (const auto& doc : documents) {
            for (size_t i = 0; i < doc.words.size(); ++i) {
                const std::string& word = doc.words[i];
                if (index.find(word) == index.end()) {
                    index[word] = PostingList(word);
                }
                index[word].AddWord(docID,
                                    {word_counter, wordlocation_t::body});
                word_counter++;
            }
            docID++;
        }
    }
};

TEST_F(QueryCompilerTest, AND) {
    std::string input = "amazon store";  // implicit AND
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);  // Parsing succeeded
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);  // Evaluation returned a valid ISR

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 3);

    post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 3);

    post = root->Next();
    EXPECT_FALSE(post.has_value());  // Should be exhausted

    delete expr;
    delete root;
}

TEST_F(QueryCompilerTest, ANDNOT) {
    std::string input = "(granola bar) NOT costco";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 1);

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
    delete root;
}

TEST_F(QueryCompilerTest, Phrase) {
    std::string input = "\"i went to the store\"";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 1);

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
    delete root;
}

TEST_F(QueryCompilerTest, ORAND) {
    std::string input = "(Granola OR protein) bar";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 1);

    post = root->Next();
    EXPECT_EQ(root->GetDocumentID(), 2);

    post = root->Next();
    EXPECT_EQ(root->GetDocumentID(), 2);

    post = root->Next();
    EXPECT_EQ(root->GetDocumentID(), 4);

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
    delete root;
}

TEST_F(QueryCompilerTest, PhraseOR) {
    std::string input = "\"Granola Bar\" OR \"Protein Bar\"";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 1);

    post = root->Next();
    EXPECT_EQ(root->GetDocumentID(), 2);

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
    delete root;
}

TEST_F(QueryCompilerTest, PhraseORNOT) {
    std::string input = "(\"Granola Bar\" OR \"Protein Bar\") NOT Costco";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentID(), 1);

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
    delete root;
}
