#pragma once

#include <unordered_map>

#include "Post.hpp"
#include "Types.hpp"

class PostingList {
   public:
    static void Serialize(const char* buf, const PostingList& postingList);

    static PostingList Deserialize(const char* buf);
    

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
