#include <gtest/gtest.h>

#include "ISR.hpp"
#include "ISRAnd.hpp"
#include "ISRContainer.hpp"
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

class ContainerISR : public ::testing::Test {
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

TEST_F(ContainerISR, SimpleNext) {
    // query = "granola bar -costco"
    ISR* ISR_word_granola = new ISRWord(&index["granola"]);
    ISR* ISR_word_bar = new ISRWord(&index["bar"]);

    ISR* ISR_granola_AND_bar = new ISRAnd({ISR_word_granola, ISR_word_bar});

    ISR* ISR_word_costco = new ISRWord(&index["costco"]);

    ISR* ISR_granola_bar_container_no_costco =
        new ISRContainer(ISR_granola_AND_bar, ISR_word_costco);

    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetCurrentPostEntry(), std::nullopt);

    // 8 and 9
    EXPECT_EQ(ISR_granola_bar_container_no_costco->Next()->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetCurrentPostEntry()->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetStartLocation(), 8);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetEndLocation(), 9);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetDocumentID(), 1);

    // no more
    EXPECT_EQ(ISR_granola_bar_container_no_costco->Next(), std::nullopt);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_granola_bar_container_no_costco->Next(), std::nullopt);
    EXPECT_EQ(ISR_granola_bar_container_no_costco->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(ContainerISR, SimpleNextDocument) {
    // query = "the -granola"
    ISR* ISR_word_the = new ISRWord(&index["the"]);
    ISR* ISR_word_granola = new ISRWord(&index["granola"]);

    ISR* ISR_the_no_granola = new ISRContainer(ISR_word_the, ISR_word_granola);

    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry(), std::nullopt);

    // 34
    EXPECT_EQ(ISR_the_no_granola->Next()->GetDelta(), 34);
    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry()->GetDelta(), 34);
    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry()->GetLocationFound(), wordlocation_t::body);
    EXPECT_EQ(ISR_the_no_granola->GetStartLocation(), 34);
    EXPECT_EQ(ISR_the_no_granola->GetEndLocation(), 34);
    EXPECT_EQ(ISR_the_no_granola->GetDocumentID(), 3);

    // 45
    EXPECT_EQ(ISR_the_no_granola->Next()->GetDelta(), 45);
    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry()->GetLocationFound(), wordlocation_t::body);
    EXPECT_EQ(ISR_the_no_granola->GetStartLocation(), 45);
    EXPECT_EQ(ISR_the_no_granola->GetEndLocation(), 45);
    EXPECT_EQ(ISR_the_no_granola->GetDocumentID(), 4);

    // no more
    EXPECT_EQ(ISR_the_no_granola->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_the_no_granola->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_no_granola->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(ContainerISR, SimpleSeekAndNext) {
    // query = "best -mcdonalds"
    ISR* ISR_word_best = new ISRWord(&index["best"]);
    ISR* ISR_word_mcdonalds = new ISRWord(&index["mcdonalds"]);

    ISR* ISR_best_no_mcdonalds = new ISRContainer(ISR_word_best, ISR_word_mcdonalds);

    EXPECT_EQ(ISR_best_no_mcdonalds->GetCurrentPostEntry(), std::nullopt);

    // seek at 20 -> 20
    EXPECT_EQ(ISR_best_no_mcdonalds->Seek(20)->GetDelta(), 20);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetCurrentPostEntry()->GetDelta(), 20);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetStartLocation(), 20);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetEndLocation(), 20);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetDocumentID(), 2);

    // seek at 40 -> nothing
    EXPECT_EQ(ISR_best_no_mcdonalds->Seek(40), std::nullopt);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_best_no_mcdonalds->Next(), std::nullopt);
    EXPECT_EQ(ISR_best_no_mcdonalds->GetCurrentPostEntry(), std::nullopt);
}
