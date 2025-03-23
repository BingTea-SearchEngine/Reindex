#pragma once

#include "Types.hpp"

class Post {
   public:
   /*
        Serializes a given Post object into a specific region of memory

        Preconditions:
            - base_region must be a valid pointer to the very beginning of a
                contiguous region of memory where this nested serialization will occur.
                this is determined by the uppermost parent that wants serialization
            - offset must be a number that when added to base_region will point
                to the region of memory where this specific Post object will be serialized to
            - post is the Post object to be serialized

        Postconditions:
            - Writes the bytes of this Post object into memory at this calculated region
            - offset will be modified. at the end of this function, offset will be a number
                that when added to base_region will point to the next available memory region
                that is not yet written to
    */
    static void Serialize(char* base_region, size_t &offset, const Post& post);

    /*
        Deserializes a given Post object that resides in a certain region of memory

        Preconditions:
            - base_region + offset points to the beginning of a region of memory
                where a Post object resides at
        
        Postconditions:
            - constructs a Post object from the bytes at this region of memory
                and returns it
    */
   static Post Deserialize(char* base_region, size_t &offset);

    Post();

    Post(std::string name);

    std::vector<postentry_t> getEntries();

    size_t addWord(postentry_t word);

    std::vector<postentry_t>::iterator begin();

    std::vector<postentry_t>::iterator end();

    std::string document;

   private:
    std::vector<postentry_t> _entries;
};
