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
        {"Document 1", {"went", "store", "grabbed", "some", "granola", "bar", "went", "another", "store"}},
        {"Document 2", {"costco", "best", "megastore", "many", "granola", "protein", "bar", "love", "costco"}},
        {"Document 3", {"think", "amazon", "online", "store", "alright", "amazon", "bananas", "university", "michigan"}},
        {"Document 4", {"mcdonalds", "best", "food", "fulfills", "my", "protein", "goal", "bar", "none"}}
    };

    void SetUp() override {
        uint32_t word_counter = 0;
        uint32_t docID = 1;
        for (const auto& doc : documents) {
            for (size_t i = 0; i < doc.words.size(); ++i) {
                const std::string& word = doc.words[i];
                if (index.find(word) == index.end()) {
                    index[word] = PostingList(word);
                }
                if (docID == 1) {
                    index[word].AddWord(docID, 0, {word_counter, wordlocation_t::body});
                } else if (docID == 2) {
                    index[word].AddWord(docID, 17, {word_counter, wordlocation_t::body});
                } else if (docID == 3) {
                    index[word].AddWord(docID, 32, {word_counter, wordlocation_t::body});
                } else {
                    index[word].AddWord(docID, 43, {word_counter, wordlocation_t::body});
                }
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

TEST_F(QueryCompilerTest, Invalid) {
    std::string input = "These words are not in the index";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    ASSERT_NE(expr, nullptr);
    std::cout << expr->GetString() << std::endl;

    ISR* root = expr->Eval();
    EXPECT_EQ(root, nullptr);

    delete expr;
}

TEST_F(QueryCompilerTest, Test) {
    std::string input = "university of michigan";
    Parser parser(input, &index);
    Expression* expr = parser.Parse();
    std::cout<<expr->GetString()<<std::endl;
    auto ISR = expr->Eval();
    ASSERT_NE(ISR, nullptr);
    delete expr;
}
