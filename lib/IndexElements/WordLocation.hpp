#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class wordlocation_t {
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
    size_t numWords;
    size_t numTitleWords;
    float pageRank;
    float cheiRank;
    size_t numOutLinks;
    std::vector<std::string> outLinks;


    friend std::ostream& operator<<(std::ostream& os, const metadata_t& m) {
        os  << "metadata_t\n{\n\t# of words=" << m.numWords 
            << "\n\t# of title words=" << m.numTitleWords 
            << "\n\tpageRank score=" << m.pageRank
            << "\n\tcheiRank score=" << m.cheiRank;
        os  << "\n\t}\n";

        std::cout << m.numOutLinks << " outward links: \n{\n";
        for (const auto &link : m.outLinks) {
            std::cout << "\t" << link << "\n";
        }
        std::cout << "}";

        return os;
    }

    size_t getBytes() {
        size_t bytes = sizeof(numWords) + sizeof(numTitleWords) + sizeof(pageRank) + sizeof(cheiRank) + sizeof(numOutLinks);
        for(const auto &link : outLinks) {
            bytes += link.size() + 1;
        }

        return bytes;
    }

};
