#pragma once

#include "Types.hpp"

class Post {
   public:
    static void Serialize(const char* buf, const Post& post);

    static Post Deserailize(const char* buf);

    size_t getBytesRequired(Post* post);

    void addWord(word_t word);

    docname document;

   private:
    words _entries;

    size_t bytesRequired;
};
