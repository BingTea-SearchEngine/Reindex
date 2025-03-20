#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class wordlocation_t {
    title = 0,
    bold = 1,
    body = 2,
};

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

    size_t getBytesRequired() {
        // 1 for null terminator
        return word.size() + 1 + sizeof(offset) + sizeof(location);
    }

    /*
        Serializes a given word_t object into a specific region of memory

        Preconditions:
            - base_region must be a valid pointer to the very beginning of a
                contiguous region of memory where this nested serialization will occur.
                this is determined by the uppermost parent that wants serialization
            - offset must be a number that when added to base_region will point
                to the region of memory where this specific word_t object will be serialized to
            - word_occurrence is the word_t object to be serialized

        Postconditions:
            - Writes the bytes of this word_t object into memory at this calculated region
            - offset will be modified. at the end of this function, offset will be a number
                that when added to base_region will point to the next available memory region
                that is not yet written to
    */
    static void Serialize(char* base_region, size_t &offset, const word_t &word_occurrence);

};

typedef std::vector<word_t> words;
typedef std::string docname;
