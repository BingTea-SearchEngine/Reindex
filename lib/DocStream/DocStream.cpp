#include "DocStream.hpp"

bool checkTagExists(std::string line, std::string tag) {
    if (line.rfind(tag, 0) != 0) {
        // std::cerr << "Malformed file. " << tag << " not found." << endl;
        return false;
    }
    return true;
}

DocStream::DocStream(std::string dirPath, std::string dictionaryPath) : _dirPath(dirPath) {
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        std::string filename = entry.path().filename();
        if (filename == "logs.txt") {
            continue;
        }
        _documents.push(filename);
    }
    // std::string word;
    // while (std::getline(dictFile, word)) {
    //     word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
    //     _dictionary.insert(word);
    // }
}

DocStreamOutput DocStream::nextFile() {
    //TODO
    //Parse the next file in the priority queue. Populate the file type struct with words in the
    //title section and normal words.
    std::string documentName = _documents.top();
    DocStreamOutput out{"", std::vector<word_t>{}, {}};

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
    std::regex url_doc_regex(R"(URL:\s*(\S+)\s+Doc number:\s*(\d+))");
    std::smatch match;
    std::string url;
    uint32_t docNum;
    if (std::regex_search(line, match, url_doc_regex)) {
        url = match[1];
        docNum = std::stoi(match[2]);
    } else {
        std::cerr << "No URL found!" << std::endl;
        return out;
    }
    out.url = url;

    // Check <title> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<title>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    uint32_t offset = 0;
    std::vector<word_t> output;
    // Get title words
    std::getline(document, line);
    std::istringstream titleIss(line);
    std::string word;
    uint32_t numTitleWords = 0;
    while (titleIss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        word = strip_utf8_spaces(word);
        if (valid(word)) {
            output.push_back(word_t{word, offset, wordlocation_t::title});
        }
        ++offset;
        ++numTitleWords;
    }

    // Check </title> tag
    std::getline(document, line);
    if (!checkTagExists(line, "</title>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    // Check <words>  tag
    std::getline(document, line);
    if (!checkTagExists(line, "<words>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    std::getline(document, line);
    std::istringstream bodyIss(line);
    while (bodyIss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        word = strip_utf8_spaces(word);
        if (valid(word)) {
            output.push_back(word_t{word, offset, wordlocation_t::body});
        }
        ++offset;
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</words>")) {
        std::cerr << documentName << std::endl;
        return out;
    }
    out.words = output;

    // Check <links>  tag
    std::getline(document, line);
    if (!checkTagExists(line, "<links>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    std::getline(document, line);
    uint32_t numOutLinks = 0;
    std::vector<std::string> outLinks;
    while (line != "</links>") {
        std::getline(document, line);
        numOutLinks++;
    }

    out.metadata.numWords = static_cast<uint32_t>(output.size());
    out.metadata.numTitleWords = numTitleWords;
    out.metadata.numOutLinks = numOutLinks;
    out.metadata.docNum = docNum;

    //Check <prank> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<prank>")) {
        std::cerr << documentName << std::endl;
        return out;
    }
    float pageRank = 0.0;
    if (std::getline(document, line)) {
        pageRank = std::stof(line);
    }
    std::getline(document, line);
    if (!checkTagExists(line, "</prank>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    //Check <prank> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<crank>")) {
        std::cerr << documentName << std::endl;
        return out;
    }
    float cheiRank = 0.0;
    if (std::getline(document, line)) {
        cheiRank = std::stof(line);
    }
    std::getline(document, line);
    if (!checkTagExists(line, "</crank>")) {
        return out;
    }

    std::getline(document, line);
    if (!checkTagExists(line, "<community>")) {
        std::cerr << documentName << std::endl;
        return out;
    }
    int community = -1;
    if (std::getline(document, line)) {
        community = std::stof(line);
    }
    std::getline(document, line);
    if (!checkTagExists(line, "</community>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    std::getline(document, line);
    if (!checkTagExists(line, "<communitycount>")) {
        std::cerr << documentName << std::endl;
        return out;
    }
    int communityCount = -1;
    if (std::getline(document, line)) {
        communityCount = std::stof(line);
    }
    std::getline(document, line);
    if (!checkTagExists(line, "</communitycount>")) {
        std::cerr << documentName << std::endl;
        return out;
    }

    out.metadata.pageRank = pageRank;
    out.metadata.cheiRank = cheiRank;
    out.metadata.community = community;
    out.metadata.communityCount = communityCount;

    return out;
}

size_t DocStream::size() {
    return _documents.size();
}
