#include <utility>

#include "DocStream.hpp"

bool checkTagExists(std::string line, std::string tag) {
    if (line.rfind(tag, 0) != 0) {
        std::cerr << "Malformed file. " << tag << " not found." << endl;
        return false;
    }
    return true;
}

DocStream::DocStream(std::string dirPath) : _dirPath(dirPath) {
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        std::string filename = entry.path().filename();
        if (filename == "logs.txt") {
            continue;
        }
        _documents.push(filename);
    }
}

std::pair<std::string, std::vector<word_t>> DocStream::nextFile() {
    //TODO
    //Parse the next file in the priority queue. Populate the file type struct with words in the
    //title section and normal words.
    std::string documentName = _documents.top();
    _documents.pop();
    std::ifstream document(_dirPath + "/" + documentName);
    if (!document) {
        std::cerr << "Error opening file " << documentName << endl;
        return std::make_pair(documentName, std::vector<word_t>{});
    }
    std::string line;
    std::getline(document, line);
    if (!checkTagExists(line, "URL: ")) {
        return std::make_pair(documentName, std::vector<word_t>{});
    }

    // Get first line that contains URL
    std::regex url_regex(R"(URL:\s*(\S+)\s+D)");
    std::smatch match;
    std::string url;
    if (std::regex_search(line, match, url_regex)) {
        url = match[1];
    } else {
        std::cerr << "No URL found!" << std::endl;
        return std::make_pair(documentName, std::vector<word_t>{});
    }

    // Check <title> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<title>")) {
        return std::make_pair(documentName, std::vector<word_t>{});
    }

    uint32_t offset = 0;
    std::vector<word_t> output;
    // Get title words
    std::getline(document, line);
    std::istringstream titleIss(line);
    std::string word;
    while (titleIss >> word) {
        output.push_back(word_t{word, offset, wordlocation_t::title});
        ++offset;
    }

    // Check </title> tag
    std::getline(document, line);
    if (!checkTagExists(line, "</title>")) {
        return std::make_pair(documentName, std::vector<word_t>{});
    }

    // Check <words>  tag
    std::getline(document, line);
    if (!checkTagExists(line, "<words>")) {
        return std::make_pair(documentName, std::vector<word_t>{});
    }

    std::getline(document, line);
    std::istringstream bodyIss(line);
    while (bodyIss >> word) {
        output.push_back(word_t{word, offset, wordlocation_t::body});
        ++offset;
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</words>")) {
        return std::make_pair(documentName, std::vector<word_t>{});
    }

    return std::make_pair(url, output);
}

size_t DocStream::size() {
    return _documents.size();
}
