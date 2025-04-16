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

DocStreamOutput DocStream::nextFile() {
    //TODO
    //Parse the next file in the priority queue. Populate the file type struct with words in the
    //title section and normal words.


    std::string documentName = _documents.top();
    DocStreamOutput out{documentName, std::vector<word_t>{}, {}};

    _documents.pop();
    std::ifstream document(_dirPath + "/" + documentName);
    if (!document) {
        std::cerr << "Error opening file " << documentName << endl;
        return out;
    }


    std::string line;
    std::getline(document, line);
    if (!checkTagExists(line, "URL: ")) {
        return out;
    }

    // Get first line that contains URL
    std::regex url_regex(R"(URL:\s*(\S+)\s+D)");
    std::smatch match;
    std::string url;
    if (std::regex_search(line, match, url_regex)) {
        url = match[1];
    } else {
        std::cerr << "No URL found!" << std::endl;
        return out;
    }

    // Check <title> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<title>")) {
        return out;
    }

    uint32_t offset = 0;
    std::vector<word_t> output;
    // Get title words
    std::getline(document, line);
    std::istringstream titleIss(line);
    std::string word;
    size_t numTitleWords = 0;
    while (titleIss >> word) {
        output.push_back(word_t{word, offset, wordlocation_t::title});
        ++offset;
        ++numTitleWords;
    }

    // Check </title> tag
    std::getline(document, line);
    if (!checkTagExists(line, "</title>")) {
        return out;
    }

    // Check <words>  tag
    std::getline(document, line);
    if (!checkTagExists(line, "<words>")) {
        return out;
    }

    std::getline(document, line);
    std::istringstream bodyIss(line);
    while (bodyIss >> word) {
        output.push_back(word_t{word, offset, wordlocation_t::body});
        ++offset;
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</words>")) {
        return out;
    }

    // Check <links>  tag
    std::getline(document, line);
    if (!checkTagExists(line, "<links>")) {
        return out;
    }

    std::vector<std::string> outLinks;
    while (!checkTagExists(line, "</links>")) {
        std::getline(document, line);
        outLinks.push_back(line);
    }

    //Check <prank> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<prank>")) {
        return out;
    }

    float pageRank = 0.0;
    if(std::getline(document, line)) {
        pageRank = std::stof(line);
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</prank>")) {
        return out;
    }

    //Check <prank> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<crank>")) {
        return out; 
    }

    float cheiRank = 0.0;
    if(std::getline(document, line)) {
        cheiRank = std::stof(line);
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</crank>")) {
        return out;
    }

    out.metadata = {output.size(), numTitleWords, pageRank, cheiRank, outLinks.size(), outLinks};
    out.url = url;
    out.words = output;

    return out;
}

size_t DocStream::size() {
    return _documents.size();
}
