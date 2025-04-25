#include "Util.hpp"

std::string strip_utf8_spaces(const std::string& input) {
    std::string output;

    for (size_t i = 0; i < input.size();) {
        unsigned char c = input[i];
        // ASCII space
        if (c == 0x20) {
            ++i;
            continue;
        }

        // UTF-8 no-break space (U+00A0)
        if (i + 1 < input.size() && c == 0xC2 && input[i + 1] == 0xA0) {
            i += 2;
            continue;
        }

        // UTF-8 en/em/thin/narrow spaces (U+2002 – U+200B or so)
        if (i + 2 < input.size() && c == 0xE2 && input[i + 1] == 0x80) {
            unsigned char third = input[i + 2];
            if (third == 0x82 || third == 0x83 || third == 0x89 || third == 0xAF) {
                i += 3;
                continue;
            }
        }
        output += c;
        ++i;
    }
    return output;
}

bool valid(const std::string& word) {
    // return word.size() > 2 && skipWords.find(word) == skipWords.end() &&
    //        std::regex_match(word, validWordRegex);
    return word.size() > 2 && skipWords.find(word) == skipWords.end() &&
           std::all_of(word.begin(), word.end(),
                       [](char c) {
                           return std::isalnum(c) || c == '+' || c == '-' || c == '_' || c == '.';
                       }) &&
           std::any_of(word.begin(), word.end(), ::isalpha);

    // return std::any_of(word.begin(), word.end(), ::isalpha);
    // return std::all_of(word.begin(), word.end(), [](unsigned char c) {
    //     return c >= 32 && c <= 126;  // printable ASCII
    //     // or use: return c < 128; for strict 7-bit ASCII
    // });
}

void* create_mmap_region(int& fd, size_t size, std::string filename) {
    fd = open(filename.c_str(), O_CREAT | O_RDWR, 0666);  // 0666 = rw
    ftruncate(fd, size);
    return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

std::pair<void*, size_t> read_mmap_region(int& fd, std::string filename) {
    fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error("Failed to open file");
    }

    struct stat fileStat;
    size_t fileSize;
    if (fstat(fd, &fileStat) == -1) {
        close(fd);
        throw std::runtime_error("Error: Failed to get file size for test_posting_list");
    }
    fileSize = fileStat.st_size;

    void* mappedRegion = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mappedRegion == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Error: Failed to memory-map file");
    }

    return {mappedRegion, fileSize};
}

long getBytesUsed() {
    struct rusage usage;
    int ret;
    ret = getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}
