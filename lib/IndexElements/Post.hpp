#pragma once

#include "Types.hpp"

class Post {
   public:
    static void Serialize(const char* buf, const Post& post);

    static Post Deserailize(const char* buf);

    Post();

    Post(docname name);

    size_t addWord(word_t word);

    words::iterator begin();

    words::iterator end();

    docname document;

   private:
    words _entries;
};
