#include <gtest/gtest.h>

#include "ISR.hpp"
#include "ISRWord.hpp"
#include "ISRAnd.hpp"
#include "ISRPhrase.hpp"
#include "PostingList.hpp"
#include "Util.hpp"

TEST(BasicSyncTable, SpeedComparisonAnd) {
    PostingList mcd_postingList = PostingList("mcdonalds");
    PostingList burgers_postingList = PostingList("burgers");

    const int num_docs = 1000;  // let there be 1000 documents
    const int doc_length = 100; // each doc has 100 words in it

    for (int docID = 1; docID <= num_docs; ++docID) {
        int doc_start = (docID - 1) * doc_length; // each doc has word corpus ranging [0...99], [100...199], etc.

        // Add 10 entries of "mcdonalds" for every doc
        for (int i = 0; i < 10; ++i) {
            int offset = doc_start + (i * 5); // spaced out
            PostEntry entry(offset, wordlocation_t::body);
            mcd_postingList.AddWord(docID, doc_start, entry);
        }

        // Add 2 entries of "burgers" for every 100th doc
        if (docID % 100 == 0) {
            PostEntry entry1(doc_start + 11, wordlocation_t::body);
            PostEntry entry2(doc_start + 91, wordlocation_t::body);
            burgers_postingList.AddWord(docID, doc_start, entry1);
            burgers_postingList.AddWord(docID, doc_start, entry2);
        }
    }

    const size_t REGION_SIZE = 4 * 1024 * 1024;

    int fd;
    void* mcd_base_region = create_mmap_region(fd, REGION_SIZE, "fake_mcd_posting_list");
    size_t mcd_offset = 0;
    PostingList::NewSerialize(static_cast<char*>(mcd_base_region), mcd_offset, mcd_postingList);
    munmap(mcd_base_region, REGION_SIZE);
    close(fd);

    int fd2;
    auto [mcd_base_region2, size] = read_mmap_region(fd2, "fake_mcd_posting_list");
    mcd_offset = 0;
    PostingList mcd_postingList_equipped_with_sync_table = PostingList::NewDeserialize(static_cast<char*>(mcd_base_region2), mcd_offset);
    close(fd2);
    ASSERT_EQ(mcd_postingList.GetSyncTable().size(), 0);
    ASSERT_GT(mcd_postingList_equipped_with_sync_table.GetSyncTable().size(), 0);

    int fd3;
    void* burgers_base_region = create_mmap_region(fd3, REGION_SIZE, "fake_burgers_posting_list");
    size_t burgers_offset = 0;
    PostingList::NewSerialize(static_cast<char*>(burgers_base_region), burgers_offset, burgers_postingList);
    munmap(burgers_base_region, REGION_SIZE);
    close(fd3);

    int fd4;
    auto [burgers_base_region2, size2] = read_mmap_region(fd3, "fake_burgers_posting_list");
    burgers_offset = 0;
    PostingList burgers_postingList_equipped_with_sync_table = PostingList::NewDeserialize(static_cast<char*>(burgers_base_region2), burgers_offset);
    close(fd4);
    ASSERT_EQ(burgers_postingList.GetSyncTable().size(), 0);
    ASSERT_GT(burgers_postingList_equipped_with_sync_table.GetSyncTable().size(), 0);

    ISR* ISR_word_mcdonalds = new ISRWord(mcd_postingList_equipped_with_sync_table);
    ISR* ISR_word_burgers = new ISRWord(burgers_postingList_equipped_with_sync_table);

    ISR* ISR_mcdonalds_AND_burgers = new ISRAnd({ISR_word_mcdonalds, ISR_word_burgers});

    // ISRWord::OldSeek AND ISRAnd::OldCatchUpStragglerISRs took on average 1.6 seconds
    // ISRWord::OldSeek AND ISRAnd::NewCatchUpStragglerISRs took on average 0.006 seconds
    // ISRWord::NewSeek AND ISRAnd::NewCatchUpStragglerISRs took on average 0.004 seconds
    // these results make sense
    std::vector<uint32_t> matching_documents;

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    while (ISR_mcdonalds_AND_burgers->NextDocument() != std::nullopt) {
        matching_documents.push_back(ISR_mcdonalds_AND_burgers->GetDocumentID());
    }

    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double time_taken = elapsed.count(); // seconds

    std::cout << "It took " << time_taken << " seconds to find all the matching documents" << std::endl;
    std::cout << "Here are the matching documents: " << std::endl;
    for (auto x : matching_documents) {
        std::cout << "    " << x << std::endl;
    }
}

TEST(BasicSyncTable, SpeedComparisonPhrase) {
    PostingList mcd_postingList = PostingList("mcdonalds");
    PostingList burgers_postingList = PostingList("burgers");

    const int num_docs = 1000;  // let there be 1000 documents
    const int doc_length = 100; // each doc has 100 words in it

    for (int docID = 1; docID <= num_docs; ++docID) {
        int doc_start = (docID - 1) * doc_length; // each doc has word corpus ranging [0...99], [100...199], etc.

        // Add 10 entries of "mcdonalds" for every doc
        for (int i = 0; i < 10; ++i) {
            int offset = doc_start + (i * 5); // spaced out
            PostEntry entry(offset, wordlocation_t::body);
            mcd_postingList.AddWord(docID, doc_start, entry);
        }

        // Add 2 entries of "burgers" for every 100th doc
        if (docID % 100 == 0) {
            PostEntry entry1(doc_start + 14, wordlocation_t::body);
            PostEntry entry2(doc_start + 41, wordlocation_t::body);
            burgers_postingList.AddWord(docID, doc_start, entry1);
            burgers_postingList.AddWord(docID, doc_start, entry2);
        }
    }

    const size_t REGION_SIZE = 4 * 1024 * 1024;

    int fd;
    void* mcd_base_region = create_mmap_region(fd, REGION_SIZE, "fake_mcd_posting_list");
    size_t mcd_offset = 0;
    PostingList::NewSerialize(static_cast<char*>(mcd_base_region), mcd_offset, mcd_postingList);
    munmap(mcd_base_region, REGION_SIZE);
    close(fd);

    int fd2;
    auto [mcd_base_region2, size] = read_mmap_region(fd2, "fake_mcd_posting_list");
    mcd_offset = 0;
    PostingList mcd_postingList_equipped_with_sync_table = PostingList::NewDeserialize(static_cast<char*>(mcd_base_region2), mcd_offset);
    close(fd2);
    ASSERT_EQ(mcd_postingList.GetSyncTable().size(), 0);
    ASSERT_GT(mcd_postingList_equipped_with_sync_table.GetSyncTable().size(), 0);

    int fd3;
    void* burgers_base_region = create_mmap_region(fd3, REGION_SIZE, "fake_burgers_posting_list");
    size_t burgers_offset = 0;
    PostingList::NewSerialize(static_cast<char*>(burgers_base_region), burgers_offset, burgers_postingList);
    munmap(burgers_base_region, REGION_SIZE);
    close(fd3);

    int fd4;
    auto [burgers_base_region2, size2] = read_mmap_region(fd3, "fake_burgers_posting_list");
    burgers_offset = 0;
    PostingList burgers_postingList_equipped_with_sync_table = PostingList::NewDeserialize(static_cast<char*>(burgers_base_region2), burgers_offset);
    close(fd4);
    ASSERT_EQ(burgers_postingList.GetSyncTable().size(), 0);
    ASSERT_GT(burgers_postingList_equipped_with_sync_table.GetSyncTable().size(), 0);

    ISR* ISR_word_mcdonalds = new ISRWord(mcd_postingList_equipped_with_sync_table);
    ISR* ISR_word_burgers = new ISRWord(burgers_postingList_equipped_with_sync_table);

    ISR* ISR_mcdonalds_burgers_phrase = new ISRPhrase({ISR_word_mcdonalds, ISR_word_burgers});

    // ISRWord::OldSeek AND ISRAnd::OldCatchUpStragglerISRs took on average 1.6 seconds
    // ISRWord::OldSeek AND ISRAnd::NewCatchUpStragglerISRs took on average 0.08 seconds
    // ISRWord::NewSeek AND ISRAnd::NewCatchUpStragglerISRs took on average 0.05 seconds
    // these results make sense
    std::vector<uint32_t> matching_documents;

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    while (ISR_mcdonalds_burgers_phrase->NextDocument() != std::nullopt) {
        matching_documents.push_back(ISR_mcdonalds_burgers_phrase->GetDocumentID());
    }

    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double time_taken = elapsed.count(); // seconds

    std::cout << "It took " << time_taken << " seconds to find all the matching documents" << std::endl;
    std::cout << "Here are the matching documents: " << std::endl;
    for (auto x : matching_documents) {
        std::cout << "    " << x << std::endl;
    }
}