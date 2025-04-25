#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

enum class wordlocation_t : uint8_t {
    title = 0,
    bold = 1,
    body = 2,
};

std::ostream& operator<<(std::ostream& os, wordlocation_t loc);

struct word_t {
    std::string word;
    uint32_t offset;
    wordlocation_t location;

    friend std::ostream& operator<<(std::ostream& os, const word_t& w) {
        os << "word_t{ word=" << w.word << " offset=" << w.offset << " location=";
        switch (w.location) {
            case (wordlocation_t::title):
                os << "title";
                break;
            case (wordlocation_t::bold):
                os << "bold";
                break;
            case (wordlocation_t::body):
                os << "body";
                break;
            default:
                os << "unknown";
        }
        os << " }";
        return os;
    }

    size_t GetBytesRequired() {
        // 1 for null terminator
        return word.size() + 1 + sizeof(offset) + sizeof(location);
    }
};

struct metadata_t {
    uint32_t numWords;
    uint32_t numTitleWords;
    uint32_t numOutLinks;
    float pageRank;
    float cheiRank;
    int community;
    int communityCount;
    uint32_t docNum;          // For snippet retrieval
    uint32_t docStartOffset;  // For snippet retrieval
    uint32_t docEndOffset;    // For ISR

    friend std::ostream& operator<<(std::ostream& os, const metadata_t& m);

    friend bool operator==(const metadata_t& lhs, const metadata_t& rhs);

    size_t getBytes() {
        size_t bytes = sizeof(numWords) + sizeof(numTitleWords) + sizeof(numOutLinks) +
                       sizeof(pageRank) + sizeof(cheiRank) + sizeof(community) +
                       sizeof(communityCount) + sizeof(docNum) + sizeof(docStartOffset) +
                       sizeof(docEndOffset);
        return bytes;
    }
};
