#pragma once

#include <string>
#include <vector>

enum class wordlocation_t {
    title = 0,
    bold = 1,
    body = 2,
};

struct word_t {
    std::string word;
    uint32_t offset;
    wordlocation_t location;
};

typedef std::vector<word_t> words;
typedef std::string docname;
