#pragma once

#include <iostream>

#include "Types.hpp"

using std::cout, std::endl;

class Post {
   public:
    static void Serialize(const char* buf, const Post& post);

    static Post Deserailize(const char* buf);

    Post();

    Post(docname name);

    size_t AddWord(word_t word);

    words::iterator Begin();

    words::iterator End();

    docname document;

   private:
    words _entries;
};
