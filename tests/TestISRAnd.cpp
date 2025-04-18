#include <gtest/gtest.h>

#include "ISR.hpp"
#include "ISRAnd.hpp"
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

class AndISR : public ::testing::Test {
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
        for (const auto& doc : documents) {
            for (size_t i = 0; i < doc.words.size(); ++i) {
                const std::string& word = doc.words[i];
                if (index.find(word) == index.end()) {
                    index[word] = PostingList(word);
                }
                index[word].AddWord(doc.name,
                                    {word_counter, wordlocation_t::body});
                word_counter++;
            }
        }
    }
};

TEST_F(AndISR, SimpleNext) {
    ISR* ISR_word_amazon = new ISRWord(index["amazon"]);
    ISR* ISR_word_store = new ISRWord(index["store"]);

    ISR* ISR_amazon_AND_store = new ISRAnd({ISR_word_amazon, ISR_word_store});

    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry(), std::nullopt);

    // amazon at 35 and store at 37
    EXPECT_EQ(ISR_amazon_AND_store->Next()->GetDelta(), 35);
    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry()->GetDelta(), 35);
    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_amazon_AND_store->GetStartLocation(), 35);
    EXPECT_EQ(ISR_amazon_AND_store->GetEndLocation(), 37);
    EXPECT_EQ(ISR_amazon_AND_store->GetDocumentName(), "Document 3");

    // store at 37 and amazon at 40
    EXPECT_EQ(ISR_amazon_AND_store->Next()->GetDelta(), 37);
    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry()->GetDelta(), 37);
    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_amazon_AND_store->GetStartLocation(), 37);
    EXPECT_EQ(ISR_amazon_AND_store->GetEndLocation(), 40);
    EXPECT_EQ(ISR_amazon_AND_store->GetDocumentName(), "Document 3");

    // no more
    EXPECT_EQ(ISR_amazon_AND_store->Next(), std::nullopt);
    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_amazon_AND_store->Next(), std::nullopt);
    EXPECT_EQ(ISR_amazon_AND_store->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(AndISR, ComplexNext) {
    ISR* ISR_word_granola = new ISRWord(index["granola"]);
    ISR* ISR_word_bar = new ISRWord(index["bar"]);

    ISR* ISR_granola_AND_bar = new ISRAnd({ISR_word_granola, ISR_word_bar});

    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry(), std::nullopt);

    // granola at 8 and bar at 9
    EXPECT_EQ(ISR_granola_AND_bar->Next()->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry()->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_AND_bar->GetStartLocation(), 8);
    EXPECT_EQ(ISR_granola_AND_bar->GetEndLocation(), 9);
    EXPECT_EQ(ISR_granola_AND_bar->GetDocumentName(), "Document 1");

    // granola at 26 and bar at 29
    EXPECT_EQ(ISR_granola_AND_bar->Next()->GetDelta(), 26);
    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry()->GetDelta(), 26);
    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_AND_bar->GetStartLocation(), 26);
    EXPECT_EQ(ISR_granola_AND_bar->GetEndLocation(), 29);
    EXPECT_EQ(ISR_granola_AND_bar->GetDocumentName(), "Document 2");

    // no more
    EXPECT_EQ(ISR_granola_AND_bar->Next(), std::nullopt);
    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_granola_AND_bar->Next(), std::nullopt);
    EXPECT_EQ(ISR_granola_AND_bar->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(AndISR, SimpleNextDocument) {
    ISR* ISR_word_the = new ISRWord(index["the"]);
    ISR* ISR_word_and = new ISRWord(index["and"]);

    ISR* ISR_the_AND_and = new ISRAnd({ISR_word_the, ISR_word_and});

    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

    // the at 3 and and at 5
    EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 3);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 3);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 3);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 5);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 1");

    // the at 19 and and at 27
    EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 19);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 19);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 19);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 2");

    // the at 45 and and at 48
    EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 45);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 45);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

    // no more
    EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(AndISR, SimpleSeekAndNext) {
    ISR* ISR_word_bar = new ISRWord(index["bar"]);
    ISR* ISR_word_and = new ISRWord(index["and"]);

    ISR* ISR_bar_AND_and = new ISRAnd({ISR_word_bar, ISR_word_and});

    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry(), std::nullopt);

    // seek to 5, and=5, bar=9
    EXPECT_EQ(ISR_bar_AND_and->Seek(5)->GetDelta(), 5);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetDelta(), 5);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_AND_and->GetStartLocation(), 5);
    EXPECT_EQ(ISR_bar_AND_and->GetEndLocation(), 9);
    EXPECT_EQ(ISR_bar_AND_and->GetDocumentName(), "Document 1");

    EXPECT_EQ(ISR_bar_AND_and->Seek(5)->GetDelta(), 5);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetDelta(), 5);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_AND_and->GetStartLocation(), 5);
    EXPECT_EQ(ISR_bar_AND_and->GetEndLocation(), 9);
    EXPECT_EQ(ISR_bar_AND_and->GetDocumentName(), "Document 1");

    // seek to 9, bar=9, and=10
    EXPECT_EQ(ISR_bar_AND_and->Seek(9)->GetDelta(), 9);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetDelta(), 9);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_AND_and->GetStartLocation(), 9);
    EXPECT_EQ(ISR_bar_AND_and->GetEndLocation(), 10);
    EXPECT_EQ(ISR_bar_AND_and->GetDocumentName(), "Document 1");

    // seek to 27, bar=29, and=27
    EXPECT_EQ(ISR_bar_AND_and->Seek(27)->GetDelta(), 27);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetDelta(), 27);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_AND_and->GetStartLocation(), 27);
    EXPECT_EQ(ISR_bar_AND_and->GetEndLocation(), 29);
    EXPECT_EQ(ISR_bar_AND_and->GetDocumentName(), "Document 2");

    // seek to 28, bar=53, and=48
    EXPECT_EQ(ISR_bar_AND_and->Seek(28)->GetDelta(), 48);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_AND_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_bar_AND_and->GetEndLocation(), 53);
    EXPECT_EQ(ISR_bar_AND_and->GetDocumentName(), "Document 4");

    // seek to 49, nothing
    EXPECT_EQ(ISR_bar_AND_and->Seek(49), std::nullopt);
    EXPECT_EQ(ISR_bar_AND_and->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(AndISR, SimpleBoundaryAvoidance) {
    ISR* ISR_word_bananas = new ISRWord(index["bananas"]);
    ISR* ISR_word_mcdonalds = new ISRWord(index["mcdonalds"]);

    ISR* ISR_bananas_AND_mcdonalds =
        new ISRAnd({ISR_word_bananas, ISR_word_mcdonalds});

    EXPECT_EQ(ISR_bananas_AND_mcdonalds->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_bananas_AND_mcdonalds->Next(), std::nullopt);
    EXPECT_EQ(ISR_bananas_AND_mcdonalds->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_bananas_AND_mcdonalds->Next(), std::nullopt);
    EXPECT_EQ(ISR_bananas_AND_mcdonalds->GetCurrentPostEntry(), std::nullopt);
}


class AndISRStress : public ::testing::Test {
   protected:
	std::unordered_map<std::string, PostingList> index;

	struct Document {
		std::string name;
		std::vector<std::string> words;
	};

	std::vector<Document> documents = {
		{"Document 1",
			{"the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog",
				"while", "the", "quick", "cat", "sleeps", "under", "a", "tree"}},
		{"Document 2",
			{"in", "a", "galaxy", "far", "far", "away", "the", "rebels", "fight",
				"against", "the", "empire", "with", "hope", "and", "courage"}},
		{"Document 3",
			{"data", "structures", "and", "algorithms", "are", "essential", "for",
				"every", "software", "engineer", "who", "wants", "to", "solve", "problems"}},
		{"Document 4",
			{"the", "adventure", "of", "the", "brave", "knight", "continues", "into",
				"the", "dark", "forest", "full", "of", "mystery", "and", "danger"}},
		{"Document 5",
			{"bar", "none", "the", "best", "protein", "bar", "you", "will", "ever",
				"taste", "with", "no", "added", "sugar", "or", "artificial", "flavors"}}
	};

	void SetUp() override {
		uint32_t word_counter = 0;
		for (const auto& doc : documents) {
			for (size_t i = 0; i < doc.words.size(); ++i) {
				const std::string& word = doc.words[i];
				if (index.find(word) == index.end()) {
					index[word] = PostingList(word);
				}
				index[word].AddWord(doc.name,
									{word_counter, wordlocation_t::body});
				word_counter++;
			}
		}
	}
};

/*
    Document 1:
    the quick brown fox jumps over the lazy dog while the quick cat sleeps under a tree
     0    1     2    3    4     5    6   7  8     9   10   11   12   13    14  15  16

    Document 2:
    in a galaxy far far away the rebels fight against the empire with hope and courage
    17 18  19   20   21   22  23  24     25    26      27    28  29   30    31  32

    Document 3:
    data structures and algorithms are essential for every software engineer who wants to solve problems
    33   34         35      36      37   38     39    40    41       42     43   44    45  46    47

    Document 4:
    the adventure of the brave knight continues into the dark forest full of mystery and danger
    48   49       50  51   52   53        54     55   56   57  58     59  60  61     62   63

    Document 5:
    bar none the best protein bar you will ever taste with no added sugar or artificial flavors
    64   65  66   67   68     69   70 71    72    73   74  75  76   77    78     79      80
*/

// PostingList{ the }: 4 document(s)
//         Post{ Document 1 }: 3 entries
//                 0 body | 6 body | 10 body |
//         Post{ Document 2 }: 2 entries
//                 23 body | 27 body |
//         Post{ Document 4 }: 3 entries
//                 48 body | 51 body | 56 body |
//         Post{ Document 5 }: 1 entries
//                 66 body |

// PostingList{ and }: 3 document(s)
//         Post{ Document 2 }: 1 entries
//                 31 body |
//         Post{ Document 3 }: 1 entries
//                 35 body |
//         Post{ Document 4 }: 1 entries
//                 62 body |

// 2, 4
TEST_F(AndISRStress, StressTest1) {
    ISR* ISR_the = new ISRWord(index["the"]);
    ISR* ISR_and = new ISRWord(index["and"]);

    ISR* ISR_the_AND_and =
        new ISRAnd({ISR_the, ISR_and});

	EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 23);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 23);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 23);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_AND_and->Seek(27)->GetDelta(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_AND_and->NextDocument()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 51);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 51);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 56);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 56);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 56);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->Seek(57), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Seek(25)->GetDelta(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 27);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");
}

// Only one word in the documents
TEST_F(AndISRStress, OneWord) {
    ISR* ISR_the = new ISRWord(index["the"]);
    ISR* ISR_and = new ISRWord(index["yerr"]);

    ISR* ISR_the_AND_and =
        new ISRAnd({ISR_the, ISR_and});

	EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_the_AND_and->Seek(0), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
}


// No words in the documents
TEST_F(AndISRStress, NoDocuments) {
    ISR* ISR_the = new ISRWord(index["ricky"]);
    ISR* ISR_and = new ISRWord(index["danny"]);

    ISR* ISR_the_AND_and =
        new ISRAnd({ISR_the, ISR_and});

	EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Seek(2), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
}

// (48, 51, 56), 62, 63
// Three Words
TEST_F(AndISRStress, ThreeWords) {
    ISR* ISR_the = new ISRWord(index["the"]);
    ISR* ISR_and = new ISRWord(index["and"]);
	ISR* ISR_danger = new ISRWord(index["danger"]);

    ISR* ISR_the_AND_and =
        new ISRAnd({ISR_the, ISR_and, ISR_danger});

	EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 63);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 51);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 51);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 63);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->Next()->GetDelta(), 56);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 56);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 56);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 63);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Seek(5)->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_AND_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_the_AND_and->GetEndLocation(), 63);
    EXPECT_EQ(ISR_the_AND_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Seek(57), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_AND_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_AND_and->GetCurrentPostEntry(), std::nullopt);
}
