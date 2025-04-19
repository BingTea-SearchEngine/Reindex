#include <gtest/gtest.h>
#include "Parser.hpp"
#include "Expression.hpp"
#include "PostingList.hpp"
#include <iostream>

class QueryCompilerTest : public ::testing::Test {
protected:
    std::unordered_map<std::string, PostingList> index;

    struct Document {
        std::string name;
        std::vector<std::string> words;
    };

    std::vector<Document> documents = {
        {"Document 1", {"i", "went", "to", "the", "store", "and", "grabbed", "some",
                        "granola", "bar", "and", "then", "i", "went", "to", "another",
                        "store"}},
        {"Document 2", {"costco", "is", "the", "best", "megastore", "it", "has", "so", "many",
                        "granola", "and", "protein", "bar", "love", "costco"}},
        {"Document 3", {"i", "think", "the", "amazon", "online", "store", "is", "alright",
                        "amazon", "is", "bananas"}},
        {"Document 4", {"mcdonalds", "has", "the", "best", "food", "and", "fulfills", "my",
                        "protein", "goal", "bar", "none"}}
    };

    void SetUp() override {
        uint32_t word_counter = 0;
        for (const auto& doc : documents) {
            for (const auto& word : doc.words) {
                if (index.find(word) == index.end()) {
                    index[word] = PostingList(word);
                }
                index[word].AddWord(doc.name, {word_counter, wordlocation_t::body});
                word_counter++;
            }
        }
    }
};

TEST_F(QueryCompilerTest, AndQuery) {
    std::string input = "amazon store";  // implicit AND
    Parser parser(input, index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);  // Parsing succeeded

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);  // Evaluation returned a valid ISR

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentName(), "Document 3");

    post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentName(), "Document 3");

    post = root->Next();
    EXPECT_FALSE(post.has_value());  // Should be exhausted

    delete expr;
}

TEST_F(QueryCompilerTest, AndNotQuery) {
    std::string input = "(granola bar) NOT costco";
    Parser parser(input, index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentName(), "Document 1");

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
}

TEST_F(QueryCompilerTest, Phrase) {
    std::string input = "\"i went to the store\"";
    Parser parser(input, index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    auto post = root->Next();
    ASSERT_TRUE(post.has_value());
    EXPECT_EQ(root->GetDocumentName(), "Document 1");

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
}

TEST_F(QueryCompilerTest, OR) {
    std::string input = "(Granola OR Protein) Bar";
    Parser parser(input, index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);

    ISR* root = expr->Eval();
    ASSERT_NE(root, nullptr);

    std::cout<<"0"<<std::endl;
    auto post = root->Next();
    std::cout<<"1"<<std::endl;
    ASSERT_TRUE(post.has_value());
    std::cout<<"2"<<std::endl;
    EXPECT_EQ(root->GetDocumentName(), "Document 1");
    std::cout<<"4"<<std::endl;

    post = root->Next();
    EXPECT_EQ(root->GetDocumentName(), "Document 2");

    post = root->Next();
    EXPECT_EQ(root->GetDocumentName(), "Document 4");

    post = root->Next();
    EXPECT_FALSE(post.has_value());

    delete expr;
}