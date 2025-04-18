#include <gtest/gtest.h>

#include "ISR.hpp"
#include "ISROr.hpp"
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

class OrISR : public ::testing::Test {
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

TEST_F(OrISR, SimpleNext) {
    ISR* ISR_word_protein = new ISRWord(index["protein"]);
    ISR* ISR_word_bananas = new ISRWord(index["bananas"]);

    ISR* ISR_protein_OR_bananas =
        new ISROr({ISR_word_protein, ISR_word_bananas});

    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry(), std::nullopt);

    // protein at 28
    EXPECT_EQ(ISR_protein_OR_bananas->Next()->GetDelta(), 28);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry()->GetDelta(), 28);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_protein_OR_bananas->GetStartLocation(), 28);
    EXPECT_EQ(ISR_protein_OR_bananas->GetEndLocation(), 42);
    EXPECT_EQ(ISR_protein_OR_bananas->GetDocumentName(), "Document 2");

    // bananas at 42
    EXPECT_EQ(ISR_protein_OR_bananas->Next()->GetDelta(), 42);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry()->GetDelta(), 42);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_protein_OR_bananas->GetStartLocation(), 42);
    EXPECT_EQ(ISR_protein_OR_bananas->GetEndLocation(), 51);
    EXPECT_EQ(ISR_protein_OR_bananas->GetDocumentName(), "Document 3");

    // protein at 51
    EXPECT_EQ(ISR_protein_OR_bananas->Next()->GetDelta(), 51);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_protein_OR_bananas->GetStartLocation(), 51);
    EXPECT_EQ(ISR_protein_OR_bananas->GetEndLocation(), 51);
    EXPECT_EQ(ISR_protein_OR_bananas->GetDocumentName(), "Document 4");

    EXPECT_EQ(ISR_protein_OR_bananas->Next(), std::nullopt);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_protein_OR_bananas->Next(), std::nullopt);
    EXPECT_EQ(ISR_protein_OR_bananas->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(OrISR, SimpleNextDocument) {
    ISR* ISR_word_the = new ISRWord(index["the"]);
    ISR* ISR_word_store = new ISRWord(index["store"]);

    ISR* ISR_the_OR_store = new ISROr({ISR_word_the, ISR_word_store});

    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry(), std::nullopt);

    // the at 3
    EXPECT_EQ(ISR_the_OR_store->NextDocument()->GetDelta(), 3);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetDelta(), 3);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_store->GetStartLocation(), 3);
    EXPECT_EQ(ISR_the_OR_store->GetEndLocation(), 4);
    EXPECT_EQ(ISR_the_OR_store->GetDocumentName(), "Document 1");

    // the at 19
    EXPECT_EQ(ISR_the_OR_store->NextDocument()->GetDelta(), 19);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetDelta(), 19);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_store->GetStartLocation(), 19);
    EXPECT_EQ(ISR_the_OR_store->GetEndLocation(), 37);
    EXPECT_EQ(ISR_the_OR_store->GetDocumentName(), "Document 2");

    // the at 34
    EXPECT_EQ(ISR_the_OR_store->NextDocument()->GetDelta(), 34);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetDelta(), 34);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_store->GetStartLocation(), 34);
    EXPECT_EQ(ISR_the_OR_store->GetEndLocation(), 37);
    EXPECT_EQ(ISR_the_OR_store->GetDocumentName(), "Document 3");

    // the at 45
    EXPECT_EQ(ISR_the_OR_store->NextDocument()->GetDelta(), 45);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_store->GetStartLocation(), 45);
    EXPECT_EQ(ISR_the_OR_store->GetEndLocation(), 45);
    EXPECT_EQ(ISR_the_OR_store->GetDocumentName(), "Document 4");

    EXPECT_EQ(ISR_the_OR_store->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_store->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_store->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(OrISR, ComplexNextDocument) {
    ISR* ISR_word_bar = new ISRWord(index["bar"]);
    ISR* ISR_word_amazon = new ISRWord(index["amazon"]);

    ISR* ISR_bar_OR_amazon = new ISROr({ISR_word_bar, ISR_word_amazon});

    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry(), std::nullopt);

    // bar at 9
    EXPECT_EQ(ISR_bar_OR_amazon->NextDocument()->GetDelta(), 9);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetDelta(), 9);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_OR_amazon->GetStartLocation(), 9);
    EXPECT_EQ(ISR_bar_OR_amazon->GetEndLocation(), 35);
    EXPECT_EQ(ISR_bar_OR_amazon->GetDocumentName(), "Document 1");

    // bar at 29
    EXPECT_EQ(ISR_bar_OR_amazon->NextDocument()->GetDelta(), 29);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetDelta(), 29);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_OR_amazon->GetStartLocation(), 29);
    EXPECT_EQ(ISR_bar_OR_amazon->GetEndLocation(), 35);
    EXPECT_EQ(ISR_bar_OR_amazon->GetDocumentName(), "Document 2");

    // amazon at 35
    EXPECT_EQ(ISR_bar_OR_amazon->NextDocument()->GetDelta(), 35);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetDelta(), 35);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_OR_amazon->GetStartLocation(), 35);
    EXPECT_EQ(ISR_bar_OR_amazon->GetEndLocation(), 53);
    EXPECT_EQ(ISR_bar_OR_amazon->GetDocumentName(), "Document 3");

    // bar at 53
    EXPECT_EQ(ISR_bar_OR_amazon->NextDocument()->GetDelta(), 53);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetDelta(), 53);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_bar_OR_amazon->GetStartLocation(), 53);
    EXPECT_EQ(ISR_bar_OR_amazon->GetEndLocation(), 53);
    EXPECT_EQ(ISR_bar_OR_amazon->GetDocumentName(), "Document 4");

    EXPECT_EQ(ISR_bar_OR_amazon->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_bar_OR_amazon->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_bar_OR_amazon->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(OrISR, SimpleSeekAndNext) {
    ISR* ISR_word_granola = new ISRWord(index["granola"]);
    ISR* ISR_word_protein = new ISRWord(index["protein"]);

    ISR* ISR_granola_OR_protein =
        new ISROr({ISR_word_granola, ISR_word_protein});

    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry(), std::nullopt);

    // granola at 8
    EXPECT_EQ(ISR_granola_OR_protein->Seek(8)->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 8);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 1");

    // protein at 28
    EXPECT_EQ(ISR_granola_OR_protein->Seek(27)->GetDelta(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 2");

    // protein at 51
    EXPECT_EQ(ISR_granola_OR_protein->Seek(51)->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 4");

    // protein at 51
    EXPECT_EQ(ISR_granola_OR_protein->Seek(51)->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 4");

    // EOF
    EXPECT_EQ(ISR_granola_OR_protein->Seek(60), std::nullopt);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry(), std::nullopt);

    // TESTING the reset with seek
    // granola at 8
    EXPECT_EQ(ISR_granola_OR_protein->Seek(8)->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 8);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 8);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 1");

    // protein at 28
    EXPECT_EQ(ISR_granola_OR_protein->Seek(27)->GetDelta(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 28);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 2");

    // protein at 51
    EXPECT_EQ(ISR_granola_OR_protein->Seek(51)->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 4");

    // protein at 51
    EXPECT_EQ(ISR_granola_OR_protein->Seek(51)->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetDelta(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_granola_OR_protein->GetStartLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetEndLocation(), 51);
    EXPECT_EQ(ISR_granola_OR_protein->GetDocumentName(), "Document 4");

    // EOF
    EXPECT_EQ(ISR_granola_OR_protein->Seek(60), std::nullopt);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_granola_OR_protein->GetCurrentPostEntry(), std::nullopt);
}

class OrISRStress : public ::testing::Test {
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
	}

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

TEST_F(OrISRStress, StressTest1) {
    ISR* ISR_the = new ISRWord(index["the"]);
    ISR* ISR_and = new ISRWord(index["and"]);

    ISR* ISR_the_OR_and =
        new ISROr({ISR_the, ISR_and});

	EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_OR_and->Next()->GetDelta(), 0);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 0);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 0);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 1");

	EXPECT_EQ(ISR_the_OR_and->Seek(30)->GetDelta(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_OR_and->NextDocument()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_OR_and->Seek(25)->GetDelta(), 27);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 27);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 27);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_OR_and->NextDocument()->GetDelta(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 3");

	EXPECT_EQ(ISR_the_OR_and->Next()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 48);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_the_OR_and->Seek(63)->GetDelta(), 66);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 66);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 66);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 66);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 5");

    // EOF
    EXPECT_EQ(ISR_the_OR_and->Seek(68), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
}

// Only one word in the documents
TEST_F(OrISRStress, OneWord) {
    ISR* ISR_the = new ISRWord(index["and"]);
    ISR* ISR_and = new ISRWord(index["yerr"]);

    ISR* ISR_the_OR_and =
        new ISROr({ISR_the, ISR_and});

	EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_OR_and->Next()->GetDelta(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 31);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_OR_and->NextDocument()->GetDelta(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 35);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 3");

	EXPECT_EQ(ISR_the_OR_and->Seek(1)->GetDelta(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 31);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_the_OR_and->Next()->GetDelta(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 35);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 35);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 3");

	EXPECT_EQ(ISR_the_OR_and->NextDocument()->GetDelta(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetDelta(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_the_OR_and->GetStartLocation(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_the_OR_and->GetDocumentName(), "Document 4");

    // EOF
    EXPECT_EQ(ISR_the_OR_and->Seek(68), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
}

// No words in the documents
TEST_F(OrISRStress, NoDocuments) {
    ISR* ISR_the = new ISRWord(index["ricky"]);
    ISR* ISR_and = new ISRWord(index["danny"]);

    ISR* ISR_the_OR_and =
        new ISROr({ISR_the, ISR_and});

	EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_OR_and->Seek(2), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_OR_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_the_OR_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->Next(), std::nullopt);
    EXPECT_EQ(ISR_the_OR_and->GetCurrentPostEntry(), std::nullopt);
}

// ORISR with three words
// 4, 31, 35, 62, 73
TEST_F(OrISRStress, ThreeWords) {
    ISR* ISR_jumps = new ISRWord(index["jumps"]);
    ISR* ISR_and = new ISRWord(index["and"]);
	ISR* ISR_taste = new ISRWord(index["taste"]);

    ISR* ISR_jumps_OR_and_OR_taste =
        new ISROr({ISR_jumps, ISR_and, ISR_taste});

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->NextDocument()->GetDelta(), 4);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetDelta(), 4);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetStartLocation(), 4);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetDocumentName(), "Document 1");

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->NextDocument()->GetDelta(), 31);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetDelta(), 31);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetStartLocation(), 31);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->Seek(70)->GetDelta(), 73);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetDelta(), 73);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetStartLocation(), 73);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetDocumentName(), "Document 5");

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->Seek(32)->GetDelta(), 35);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetDelta(), 35);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetStartLocation(), 35);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 31);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetDocumentName(), "Document 3");

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->Next()->GetDelta(), 62);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetDelta(), 62);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetStartLocation(), 62);
    // EXPECT_EQ(ISR_the_OR_and->GetEndLocation(), 62);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_jumps_OR_and_OR_taste->NextDocument()->GetDelta(), 73);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetDelta(), 73);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetStartLocation(), 73);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetEndLocation(), 73);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetDocumentName(), "Document 5");

    // EOF
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->Seek(77), std::nullopt);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry(), std::nullopt);
    EXPECT_EQ(ISR_jumps_OR_and_OR_taste->GetCurrentPostEntry(), std::nullopt);
}