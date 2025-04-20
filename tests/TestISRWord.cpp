#include <gtest/gtest.h>

#include "ISR.hpp"
#include "ISRWord.hpp"
#include "PostingList.hpp"

/*
    Document 1:
    i went to the store and grabbed some granola bar and then i went to another store
    0  1   2   3    4    5     6     7      8     9   10  11  12 13  14   15      16
*/

/*
    Document 2:
    costco is the best megastore. it has so many granola and protein bar. love costco
       17  18  19  20     21      22  23 24  25    26    27    28     29   30    31
*/

/*
    Document 3:
    i think the amazon online store is alright. amazon is bananas
    32  33  34   35     36     37   38    39      40   41    42
*/

/*
    Document 4:
    mcdonalds has the best food and fulfills my protein goal. bar none
        43     44  45  46   47   48    49    50   51     52    53  54
*/

/*
output from PostingList::Print()

PostingList{ none }: 1 document(s)
        Post{ Document 4 }: 1 entries
                54 body |
PostingList{ goal }: 1 document(s)
        Post{ Document 4 }: 1 entries
                52 body |
PostingList{ my }: 1 document(s)
        Post{ Document 4 }: 1 entries
                50 body |
PostingList{ fulfills }: 1 document(s)
        Post{ Document 4 }: 1 entries
                49 body |
PostingList{ costco }: 1 document(s)
        Post{ Document 2 }: 2 entries
                17 body | 31 body |
PostingList{ another }: 1 document(s)
        Post{ Document 1 }: 1 entries
                15 body |
PostingList{ then }: 1 document(s)
        Post{ Document 1 }: 1 entries
                11 body |
PostingList{ bar }: 3 document(s)
        Post{ Document 1 }: 1 entries
                9 body |
        Post{ Document 2 }: 1 entries
                29 body |
        Post{ Document 4 }: 1 entries
                53 body |
PostingList{ i }: 2 document(s)
        Post{ Document 1 }: 2 entries
                0 body | 12 body |
        Post{ Document 3 }: 1 entries
                32 body |
PostingList{ and }: 3 document(s)
        Post{ Document 1 }: 2 entries
                5 body | 10 body |
        Post{ Document 2 }: 1 entries
                27 body |
        Post{ Document 4 }: 1 entries
                48 body |
PostingList{ food }: 1 document(s)
        Post{ Document 4 }: 1 entries
                47 body |
PostingList{ best }: 2 document(s)
        Post{ Document 2 }: 1 entries
                20 body |
        Post{ Document 4 }: 1 entries
                46 body |
PostingList{ store }: 2 document(s)
        Post{ Document 1 }: 2 entries
                4 body | 16 body |
        Post{ Document 3 }: 1 entries
                37 body |
PostingList{ granola }: 2 document(s)
        Post{ Document 1 }: 1 entries
                8 body |
        Post{ Document 2 }: 1 entries
                26 body |
PostingList{ the }: 4 document(s)
        Post{ Document 1 }: 1 entries
                3 body |
        Post{ Document 2 }: 1 entries
                19 body |
        Post{ Document 3 }: 1 entries
                34 body |
        Post{ Document 4 }: 1 entries
                45 body |
PostingList{ to }: 1 document(s)
        Post{ Document 1 }: 2 entries
                2 body | 14 body |
PostingList{ megastore }: 1 document(s)
        Post{ Document 2 }: 1 entries
                21 body |
PostingList{ is }: 2 document(s)
        Post{ Document 2 }: 1 entries
                18 body |
        Post{ Document 3 }: 2 entries
                38 body | 41 body |
PostingList{ it }: 1 document(s)
        Post{ Document 2 }: 1 entries
                22 body |
PostingList{ has }: 2 document(s)
        Post{ Document 2 }: 1 entries
                23 body |
        Post{ Document 4 }: 1 entries
                44 body |
PostingList{ so }: 1 document(s)
        Post{ Document 2 }: 1 entries
                24 body |
PostingList{ some }: 1 document(s)
        Post{ Document 1 }: 1 entries
                7 body |
PostingList{ grabbed }: 1 document(s)
        Post{ Document 1 }: 1 entries
                6 body |
PostingList{ think }: 1 document(s)
        Post{ Document 3 }: 1 entries
                33 body |
PostingList{ love }: 1 document(s)
        Post{ Document 2 }: 1 entries
                30 body |
PostingList{ amazon }: 1 document(s)
        Post{ Document 3 }: 2 entries
                35 body | 40 body |
PostingList{ many }: 1 document(s)
        Post{ Document 2 }: 1 entries
                25 body |
PostingList{ mcdonalds }: 1 document(s)
        Post{ Document 4 }: 1 entries
                43 body |
PostingList{ protein }: 2 document(s)
        Post{ Document 2 }: 1 entries
                28 body |
        Post{ Document 4 }: 1 entries
                51 body |
PostingList{ went }: 1 document(s)
        Post{ Document 1 }: 2 entries
                1 body | 13 body |
PostingList{ online }: 1 document(s)
        Post{ Document 3 }: 1 entries
                36 body |
PostingList{ alright }: 1 document(s)
        Post{ Document 3 }: 1 entries
                39 body |
PostingList{ bananas }: 1 document(s)
        Post{ Document 3 }: 1 entries
                42 body |
*/

class WordISR : public ::testing::Test {
   protected:
    std::unordered_map<std::string, PostingList> index;

    struct Document {
        std::string name;
        std::vector<std::string> words;
    };

    std::vector<Document> documents = {
        {"Document 1",
         {"i", "went", "to", "the", "store", "and", "grabbed", "some",
          "granola", "bar", "and", "then", "i", "went", "to", "another",
          "store"}},
        {"Document 2",
         {"costco", "is", "the", "best", "megastore", "it", "has", "so", "many",
          "granola", "and", "protein", "bar", "love", "costco"}},
        {"Document 3",
         {"i", "think", "the", "amazon", "online", "store", "is", "alright",
          "amazon", "is", "bananas"}},
        {"Document 4",
         {"mcdonalds", "has", "the", "best", "food", "and", "fulfills", "my",
          "protein", "goal", "bar", "none"}}};

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
                        docID++;
                    }
                }
            }
};

TEST_F(WordISR, SimpleNext) {
    ISR* ISR_word_megastore = new ISRWord(index["megastore"]);

    EXPECT_EQ(ISR_word_megastore->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(
        static_cast<ISRWord*>(ISR_word_megastore)->GetDocumentCount(),
        1);  // downward cast because polymorphism and ISR_word has these methods but ISR does not
    EXPECT_EQ(
        static_cast<ISRWord*>(ISR_word_megastore)->GetNumberOfOccurrences(), 1);

    EXPECT_EQ(ISR_word_megastore->Next()->GetDelta(), 21);
    EXPECT_EQ(ISR_word_megastore->GetCurrentPostEntry()->GetDelta(), 21);
    EXPECT_EQ(ISR_word_megastore->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_megastore->GetStartLocation(), 21);
    EXPECT_EQ(ISR_word_megastore->GetEndLocation(), 21);
    EXPECT_EQ(ISR_word_megastore->GetDocumentID(), 2);

    EXPECT_EQ(ISR_word_megastore->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_megastore->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_megastore->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_megastore->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(WordISR, SimpleNextDocument) {
    ISR* ISR_word_and = new ISRWord(index["and"]);

    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(static_cast<ISRWord*>(ISR_word_and)->GetDocumentCount(), 3);
    EXPECT_EQ(static_cast<ISRWord*>(ISR_word_and)->GetNumberOfOccurrences(), 4);

    EXPECT_EQ(ISR_word_and->NextDocument()->GetDelta(), 5);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetDelta(), 5);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_and->GetStartLocation(), 5);
    EXPECT_EQ(ISR_word_and->GetEndLocation(), 5);
    EXPECT_EQ(ISR_word_and->GetDocumentID(), 1);

    // skipping entry at #10

    EXPECT_EQ(ISR_word_and->NextDocument()->GetDelta(), 27);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetDelta(), 27);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_and->GetStartLocation(), 27);
    EXPECT_EQ(ISR_word_and->GetEndLocation(), 27);
    EXPECT_EQ(ISR_word_and->GetDocumentID(), 2);

    EXPECT_EQ(ISR_word_and->NextDocument()->GetDelta(), 48);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_word_and->GetEndLocation(), 48);
    EXPECT_EQ(ISR_word_and->GetDocumentID(), 4);

    EXPECT_EQ(ISR_word_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(WordISR, SimpleSeekAndNext) {
    ISR* ISR_word_the = new ISRWord(index["the"]);

    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(static_cast<ISRWord*>(ISR_word_the)->GetDocumentCount(), 4);
    EXPECT_EQ(static_cast<ISRWord*>(ISR_word_the)->GetNumberOfOccurrences(), 4);

    EXPECT_EQ(ISR_word_the->Seek(15)->GetDelta(), 19);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 19);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 19);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 19);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 2);

    EXPECT_EQ(ISR_word_the->Seek(45)->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 4);

    EXPECT_EQ(ISR_word_the->Seek(2)->GetDelta(), 3);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 3);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 3);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 3);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 1);

    EXPECT_EQ(ISR_word_the->Seek(20)->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 3);

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 4);

    EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_the->Seek(0)->GetDelta(), 3);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 3);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 3);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 3);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 1);

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 19);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 19);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 19);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 19);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 2);

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 3);

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetDocumentID(), 4);

    EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(WordISR, ComplexStressTest) {
    // stress test / kitchen sink
}