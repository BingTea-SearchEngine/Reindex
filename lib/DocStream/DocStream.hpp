// DocStream
// Parse and batch documents to feed into index
#pragma once

#include <queue>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>


using std::cout;
using std::endl;

#include "Types.hpp"

bool checkTagExists(std::string line, std::string tag);

struct BatchNumCompare {
    bool operator()(std::string a, std::string b) {
        return a[0] > b[0]; // Min-heap (smallest element at top)
    }
};

class DocStream {
   public:
    // Populate _documents with file path to documents. Sort by batch number where a document is
    // named batch_number-url
    DocStream(std::string dirPath);

    // Parse next file in _documents priority queue
    std::pair<docname, words> nextFile();

    size_t size();

   private:
    std::priority_queue<std::string, std::vector<std::string>, BatchNumCompare> _documents;
    std::string _dirPath;
};

