#pragma once

#include <fcntl.h>     // For O_CREAT, O_RDWR
#include <sys/mman.h>  // For shm_open, mmap, PROT_READ, PROT_WRITE, MAP_SHARED, munmap
#include <sys/resource.h>
#include <sys/stat.h>  // For mode constants
#include <unistd.h>    // For ftruncate, close
#include <algorithm>
#include <regex>
#include <unordered_set>
#include <utility>

#include <stdexcept>
#include <string>

const static std::regex validWordRegex("^[a-zA-Z0-9_+\\-.]*[a-zA-Z][a-zA-Z0-9_+\\-.]*$");
const static std::unordered_set<std::string> skipWords = {
    "a",     "an",     "the",     "and",     "or",    "but",   "if",      "then",   "else",
    "when",  "where",  "while",   "at",      "by",    "for",   "to",      "in",     "on",
    "from",  "up",     "down",    "with",    "about", "as",    "into",    "like",   "over",
    "after", "before", "between", "without", "under", "again", "further", "once",   "just",
    "only",  "same",   "so",      "too",     "very",  "can",   "will",    "should", "would",
    "could", "is",     "am",      "are",     "was",   "were",  "be",      "been",   "being",
    "have",  "has",    "had",     "do",      "does",  "did",   "not",     "i"};

std::string strip_utf8_spaces(const std::string& input);

bool valid(const std::string& word);
/*
 * @brief Create a memory mapped region
 *
 * */
void* create_mmap_region(int& fd, size_t size, std::string filename);

std::pair<void*, size_t> read_mmap_region(int& fd, std::string filename);

long getBytesUsed();
