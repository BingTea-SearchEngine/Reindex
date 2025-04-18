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
    EXPECT_EQ(ISR_word_megastore->GetDocumentName(), "Document 2");

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
    EXPECT_EQ(ISR_word_and->GetDocumentName(), "Document 1");

    // skipping entry at #10

    EXPECT_EQ(ISR_word_and->NextDocument()->GetDelta(), 27);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetDelta(), 27);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_and->GetStartLocation(), 27);
    EXPECT_EQ(ISR_word_and->GetEndLocation(), 27);
    EXPECT_EQ(ISR_word_and->GetDocumentName(), "Document 2");

    EXPECT_EQ(ISR_word_and->NextDocument()->GetDelta(), 48);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_word_and->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_and->GetStartLocation(), 48);
    EXPECT_EQ(ISR_word_and->GetEndLocation(), 48);
    EXPECT_EQ(ISR_word_and->GetDocumentName(), "Document 4");

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
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 2");

    EXPECT_EQ(ISR_word_the->Seek(45)->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 4");

    EXPECT_EQ(ISR_word_the->Seek(2)->GetDelta(), 3);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 3);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 3);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 3);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 1");

    EXPECT_EQ(ISR_word_the->Seek(20)->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 3");

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 4");

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
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 1");

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 19);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 19);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 19);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 19);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 2");

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 34);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 34);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 3");

    EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 45);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 45);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 4");

    EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);
}

TEST_F(WordISR, ComplexStressTest) {
    // stress test / kitchen sink
}


class WordISRStress : public ::testing::Test {
	protected:
	std::unordered_map<std::string, PostingList> index;

	struct Document {
		std::string name;
		std::vector<std::string> words;
	};

	// same word over and over
	void SetUp() override {
		uint32_t word_counter = 0;
		for (int doc_num = 0; doc_num < 10000; ++doc_num) {
			std::string doc_name = "Doc_" + std::to_string(doc_num);
			for (int i = 0; i < 50; ++i) {
				std::string word = "stressword";
				if (index.find(word) == index.end()) {
					index[word] = PostingList(word);
				}
				index[word].AddWord(doc_name, {word_counter++, wordlocation_t::body});
			}
		}
	}
};

TEST_F(WordISRStress, SuperHeavyWordWithManyOccurrences) {
    ISR* isr = new ISRWord(index["stressword"]);
    int count = 0;
    while (isr->Next()) count++;
    EXPECT_EQ(count, 10000 * 50);
}


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

/*

PostingList{ the }: 4 document(s)
        Post{ Document 1 }: 3 entries
                0 body | 6 body | 10 body |
        Post{ Document 2 }: 2 entries
                23 body | 27 body |
        Post{ Document 4 }: 3 entries
                48 body | 51 body | 56 body |
        Post{ Document 5 }: 1 entries
                66 body |

PostingList{ quick }: 1 document(s)
        Post{ Document 1 }: 2 entries
                1 body | 11 body |

PostingList{ brown }: 1 document(s)
        Post{ Document 1 }: 1 entries
                2 body |

PostingList{ fox }: 1 document(s)
        Post{ Document 1 }: 1 entries
                3 body |

PostingList{ jumps }: 1 document(s)
        Post{ Document 1 }: 1 entries
                4 body |

PostingList{ over }: 1 document(s)
        Post{ Document 1 }: 1 entries
                5 body |

PostingList{ lazy }: 1 document(s)
        Post{ Document 1 }: 1 entries
                7 body |

PostingList{ dog }: 1 document(s)
        Post{ Document 1 }: 1 entries
                8 body |

PostingList{ while }: 1 document(s)
        Post{ Document 1 }: 1 entries
                9 body |

PostingList{ cat }: 1 document(s)
        Post{ Document 1 }: 1 entries
                12 body |

PostingList{ sleeps }: 1 document(s)
        Post{ Document 1 }: 1 entries
                13 body |

PostingList{ under }: 1 document(s)
        Post{ Document 1 }: 1 entries
                14 body |

PostingList{ a }: 2 document(s)
        Post{ Document 1 }: 1 entries
                15 body |
        Post{ Document 2 }: 1 entries
                18 body |

PostingList{ tree }: 1 document(s)
        Post{ Document 1 }: 1 entries
                16 body |

PostingList{ in }: 1 document(s)
        Post{ Document 2 }: 1 entries
                17 body |

PostingList{ galaxy }: 1 document(s)
        Post{ Document 2 }: 1 entries
                19 body |

PostingList{ far }: 1 document(s)
        Post{ Document 2 }: 2 entries
                20 body | 21 body |

PostingList{ away }: 1 document(s)
        Post{ Document 2 }: 1 entries
                22 body |

PostingList{ rebels }: 1 document(s)
        Post{ Document 2 }: 1 entries
                24 body |

PostingList{ fight }: 1 document(s)
        Post{ Document 2 }: 1 entries
                25 body |

PostingList{ against }: 1 document(s)
        Post{ Document 2 }: 1 entries
                26 body |

PostingList{ empire }: 1 document(s)
        Post{ Document 2 }: 1 entries
                28 body |

PostingList{ with }: 2 document(s)
        Post{ Document 2 }: 1 entries
                29 body |
        Post{ Document 5 }: 1 entries
                73 body |

PostingList{ hope }: 1 document(s)
        Post{ Document 2 }: 1 entries
                30 body |

PostingList{ and }: 3 document(s)
        Post{ Document 2 }: 1 entries
                31 body |
        Post{ Document 3 }: 1 entries
                35 body |
        Post{ Document 4 }: 1 entries
                62 body |

PostingList{ courage }: 1 document(s)
        Post{ Document 2 }: 1 entries
                32 body |

PostingList{ data }: 1 document(s)
        Post{ Document 3 }: 1 entries
                33 body |

PostingList{ structures }: 1 document(s)
        Post{ Document 3 }: 1 entries
                34 body |

PostingList{ algorithms }: 1 document(s)
        Post{ Document 3 }: 1 entries
                36 body |

PostingList{ are }: 1 document(s)
        Post{ Document 3 }: 1 entries
                37 body |

PostingList{ essential }: 1 document(s)
        Post{ Document 3 }: 1 entries
                38 body |

PostingList{ for }: 1 document(s)
        Post{ Document 3 }: 1 entries
                39 body |

PostingList{ every }: 1 document(s)
        Post{ Document 3 }: 1 entries
                40 body |

PostingList{ software }: 1 document(s)
        Post{ Document 3 }: 1 entries
                41 body |

PostingList{ engineer }: 1 document(s)
        Post{ Document 3 }: 1 entries
                42 body |

PostingList{ who }: 1 document(s)
        Post{ Document 3 }: 1 entries
                43 body |

PostingList{ wants }: 1 document(s)
        Post{ Document 3 }: 1 entries
                44 body |

PostingList{ to }: 1 document(s)
        Post{ Document 3 }: 1 entries
                45 body |

PostingList{ solve }: 1 document(s)
        Post{ Document 3 }: 1 entries
                46 body |

PostingList{ problems }: 1 document(s)
        Post{ Document 3 }: 1 entries
                47 body |

PostingList{ adventure }: 1 document(s)
        Post{ Document 4 }: 1 entries
                49 body |

PostingList{ of }: 2 document(s)
        Post{ Document 4 }: 2 entries
                50 body | 62 body |

PostingList{ brave }: 1 document(s)
        Post{ Document 4 }: 1 entries
                52 body |

PostingList{ knight }: 1 document(s)
        Post{ Document 4 }: 1 entries
                53 body |

PostingList{ continues }: 1 document(s)
        Post{ Document 4 }: 1 entries
                54 body |

PostingList{ into }: 1 document(s)
        Post{ Document 4 }: 1 entries
                55 body |

PostingList{ dark }: 1 document(s)
        Post{ Document 4 }: 1 entries
                57 body |

PostingList{ forest }: 1 document(s)
        Post{ Document 4 }: 1 entries
                58 body |

PostingList{ full }: 1 document(s)
        Post{ Document 4 }: 1 entries
                59 body |

PostingList{ mystery }: 1 document(s)
        Post{ Document 4 }: 1 entries
                61 body |

PostingList{ danger }: 1 document(s)
        Post{ Document 4 }: 1 entries
                63 body |

PostingList{ bar }: 1 document(s)
        Post{ Document 5 }: 2 entries
                64 body | 69 body |

PostingList{ none }: 1 document(s)
        Post{ Document 5 }: 1 entries
                65 body |

PostingList{ best }: 1 document(s)
        Post{ Document 5 }: 1 entries
                67 body |

PostingList{ protein }: 1 document(s)
        Post{ Document 5 }: 1 entries
                68 body |

PostingList{ you }: 1 document(s)
        Post{ Document 5 }: 1 entries
                70 body |

PostingList{ will }: 1 document(s)
        Post{ Document 5 }: 1 entries
                71 body |

PostingList{ ever }: 1 document(s)
        Post{ Document 5 }: 1 entries
                72 body |

PostingList{ taste }: 1 document(s)
        Post{ Document 5 }: 1 entries
                73 body |

PostingList{ no }: 1 document(s)
        Post{ Document 5 }: 1 entries
                75 body |

PostingList{ added }: 1 document(s)
        Post{ Document 5 }: 1 entries
                76 body |

PostingList{ sugar }: 1 document(s)
        Post{ Document 5 }: 1 entries
                77 body |

PostingList{ or }: 1 document(s)
        Post{ Document 5 }: 1 entries
                78 body |

PostingList{ artificial }: 1 document(s)
        Post{ Document 5 }: 1 entries
                79 body |

PostingList{ flavors }: 1 document(s)
        Post{ Document 5 }: 1 entries
                80 body |


*/

class WordISRMore : public ::testing::Test {
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


 TEST_F(WordISRMore, NextAllOccurrences_The) {
    ISR* isr = new ISRWord(index["the"]);
    std::vector<int> expected_deltas = {0, 6, 10, 23, 27, 48, 51, 56, 66}; // manually counted
    std::vector<int> actual_deltas;

    while (auto entry = isr->Next()) {
        actual_deltas.push_back(entry->GetDelta());
    }

    EXPECT_EQ(actual_deltas, expected_deltas);
}

TEST_F(WordISRMore, NextAllOccurrences_And) {
    ISR* isr = new ISRWord(index["and"]);
    std::vector<int> expected_deltas = {31, 35, 62}; // manually counted
    std::vector<int> actual_deltas;

    while (auto entry = isr->Next()) {
        actual_deltas.push_back(entry->GetDelta());
    }

    EXPECT_EQ(actual_deltas, expected_deltas);
}

TEST_F(WordISRMore, NextDocument_the) {
    ISR* isr = new ISRWord(index["the"]);
    std::vector<std::string> expected_docs = {
        "Document 1", "Document 2", "Document 4", "Document 5"
    };
    std::vector<std::string> actual_docs;

    while (auto entry = isr->NextDocument()) {
        actual_docs.push_back(isr->GetDocumentName());
    }

    EXPECT_EQ(actual_docs, expected_docs);
}

TEST_F(WordISRMore, NextDocument_And) {
    ISR* isr = new ISRWord(index["and"]);
    std::vector<std::string> expected_docs = {
        "Document 2", "Document 3", "Document 4"
    };
    std::vector<std::string> actual_docs;

    while (auto entry = isr->NextDocument()) {
        actual_docs.push_back(isr->GetDocumentName());
    }

    EXPECT_EQ(actual_docs, expected_docs);
}

TEST_F(WordISRMore, SeekBehavior_far) {
    ISR* isr_word_far = new ISRWord(index["far"]);

    auto e1 = isr->Seek(21);
    ASSERT_TRUE(e1.has_value());
    EXPECT_EQ(e1->GetDelta(), 21);
	EXPECT_EQ(ISR_word_far->GetCurrentPostEntry()->GetDelta(), 21);
    EXPECT_EQ(isr_word_far->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(isr_word_far->GetStartLocation(), 21);
    EXPECT_EQ(isr_word_far->GetEndLocation(), 21);
    EXPECT_EQ(isr_word_far->GetDocumentName(), "Document 2");

    auto e2 = isr->Seek(20);
    ASSERT_TRUE(e2.has_value());
    EXPECT_EQ(e2->GetDelta(), 20);
	EXPECT_EQ(isr_word_far->GetCurrentPostEntry()->GetDelta(), 20);
    EXPECT_EQ(isr_word_far->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(isr_word_far->GetStartLocation(), 20);
    EXPECT_EQ(isr_word_far->GetEndLocation(), 20);
    EXPECT_EQ(isr_word_far->GetDocumentName(), "Document 2");

    auto e3 = isr->Seek(22);
    EXPECT_EQ(e3, std::nullopt);
	EXPECT_EQ(ISR_word_far->GetCurrentPostEntry(), std::nullopt);
}
 

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

 TEST_F(WordISRMore, StressTest) {
    ISR* ISR_word_the = new ISRWord(index["the"]);

	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);
	EXPECT_EQ(
		static_cast<ISRWord*>(ISR_word_the)->GetDocumentCount(),
		4);  // downward cast because polymorphism and ISR_word has these methods but ISR does not
	EXPECT_EQ(
		static_cast<ISRWord*>(ISR_word_the)->GetNumberOfOccurrences(), 9);

	EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 21);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 21);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
			wordlocation_t::body);
	EXPECT_EQ(ISR_word_the->GetStartLocation(), 21);
	EXPECT_EQ(ISR_word_the->GetEndLocation(), 21);
	EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_word_the->Seek(66)->GetDelta(), 66);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 66);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 66);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 66);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 5");

    EXPECT_EQ(ISR_word_the->Seek(6)->GetDelta(), 6);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 6);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 6);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 6);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 1");

	// Skips 10
	EXPECT_EQ(ISR_word_the->NextDocument()->GetDelta(), 23);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 23);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
			wordlocation_t::body);
	EXPECT_EQ(ISR_word_the->GetStartLocation(), 23);
	EXPECT_EQ(ISR_word_the->GetEndLocation(), 23);
	EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 27);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 27);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
			wordlocation_t::body);
	EXPECT_EQ(ISR_word_the->GetStartLocation(), 27);
	EXPECT_EQ(ISR_word_the->GetEndLocation(), 27);
	EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 2");

	EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 48);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 48);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
			wordlocation_t::body);
	EXPECT_EQ(ISR_word_the->GetStartLocation(), 48);
	EXPECT_EQ(ISR_word_the->GetEndLocation(), 48);
	EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 66);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 66);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
			wordlocation_t::body);
	EXPECT_EQ(ISR_word_the->GetStartLocation(), 66);
	EXPECT_EQ(ISR_word_the->GetEndLocation(), 66);
	EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 5");

	EXPECT_EQ(ISR_word_the->Next(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);

    EXPECT_EQ(ISR_word_the->NextDocument(), std::nullopt);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry(), std::nullopt);

	EXPECT_EQ(ISR_word_the->Seek(28)->GetDelta(), 48);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 48);
    EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
              wordlocation_t::body);
    EXPECT_EQ(ISR_word_the->GetStartLocation(), 48);
    EXPECT_EQ(ISR_word_the->GetEndLocation(), 48);
    EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 4");

	EXPECT_EQ(ISR_word_the->Next()->GetDelta(), 51);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetDelta(), 51);
	EXPECT_EQ(ISR_word_the->GetCurrentPostEntry()->GetLocationFound(),
			wordlocation_t::body);
	EXPECT_EQ(ISR_word_the->GetStartLocation(), 51);
	EXPECT_EQ(ISR_word_the->GetEndLocation(), 51);
	EXPECT_EQ(ISR_word_the->GetDocumentName(), "Document 4");
}