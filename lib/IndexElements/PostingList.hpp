#pragma once

#include <unordered_map>

#include "Post.hpp"
#include "Types.hpp"

class PostingList {
   public:
    static void Serialize(const char* buf, const PostingList& postingList);

    static PostingList Deserialize(const char* buf);

    PostingList();

    size_t getBytesRequired();

    void addWord(docname doc, word_t word);

    std::string word;

   private:
    std::vector<Post> _posts;
    //TODO
    //Synchronization point. Need to do this as we are writing to memory
    size_t _bytesRequired;

    Post _currPost;
};
