// DocStream
// Parse and batch documents to feed into index
#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

using std::cout;
using std::endl;

#include "PostEntry.hpp"
#include "WordLocation.hpp"

bool checkTagExists(std::string line, std::string tag);

struct BatchNumCompare {
    bool operator()(std::string a, std::string b) {
        int anum = std::stoi(a.substr(0, a.find('.')));
        int bnum = std::stoi(b.substr(0, b.find('.')));
        return anum > bnum;  // Min-heap (smallest element at top)
    }
};

struct DocStreamOutput {
    std::string url;
    std::vector<word_t> words;
    metadata_t metadata;
};

class DocStream {
   public:
    // Populate _documents with file path to documents. Sort by batch number where a document is
    // named batch_number-url
    DocStream(std::string dirPath, std::string dictionaryPath);

    // Parse next file in _documents priority queue
    DocStreamOutput nextFile();

    size_t size();

   private:
    std::priority_queue<std::string, std::vector<std::string>, BatchNumCompare> _documents;
    std::unordered_set<std::string> _dictionary;
    std::string _dirPath;
};
