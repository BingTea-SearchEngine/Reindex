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

    static Post Deserialize(const char* buf);

    Post();

    Post(docname name);

    size_t addWord(word_t word);

    words::iterator begin();

    words::iterator end();

    docname document;

   private:
    words _entries;
};
