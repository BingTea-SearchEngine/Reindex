#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class wordlocation_t {
    title = 0,
    bold = 1,
    body = 2,
};

struct postentry_t {
    uint32_t offset;
    wordlocation_t location;

    friend std::ostream& operator<<(std::ostream& os, const postentry_t& w) {
        os << "postentry_t{ offset=" << w.offset << " location=";
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
        return sizeof(offset) + sizeof(location);
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
    static void Serialize(char* base_region, size_t &offset, const postentry_t &word_occurrence);

    /*
        Deserializes a given word_t object that resides in a certain region of memory

        Preconditions:
            - base_region + offset points to the beginning of a region of memory
                where a word_t object resides at
        
        Postconditions:
            - constructs a word_t object from the bytes at this region of memory
                and returns it
    */
    static postentry_t Deserialize(char* base_region, size_t &offset);
};

typedef std::vector<postentry_t> words;
typedef std::string docname;
