#pragma once

#include <unordered_map>

#include "Post.hpp"
#include "Types.hpp"

class PostingList {
   public:
   /*
        Serializes a given PostingList object into a specific region of memory

        Preconditions:
            - base_region must be a valid pointer to the very beginning of a
                contiguous region of memory where this nested serialization will occur.
                this is determined by the uppermost parent that wants serialization
            - offset must be a number that when added to base_region will point
                to the region of memory where this specific PostingList object will be serialized to
            - postingList is the PostingList object to be serialized

        Postconditions:
            - Writes the bytes of this PostingList object into memory at this calculated region
            - offset will be modified. at the end of this function, offset will be a number
                that when added to base_region will point to the next available memory region
                that is not yet written to
    */
    static void Serialize(char* base_region, size_t &offset, const PostingList& postingList);

    /*
        Deserializes a given PostingList object that resides in a certain region of memory

        Preconditions:
            - base_region + offset points to the beginning of a region of memory
                where a PostingList object resides at
        
        Postconditions:
            - constructs a PostingList object from the bytes at this region of memory
                and returns it
    */
   static PostingList Deserialize(char* base_region, size_t &offset);


    PostingList();

    PostingList(const std::string& _word);

    size_t getOverheadBytes();

    size_t addWord(docname doc, word_t word);

    Post getPost(size_t index);

    std::vector<Post>::iterator begin();

    std::vector<Post>::iterator end();

    std::string word;

   private:
    std::vector<Post> _posts;

    std::unordered_map<uint32_t, std::tuple<size_t>> _sync;
};
