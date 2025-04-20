#include "DocStream.hpp"

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
            if (third == 0x82 || third == 0x83 || third == 0x89 ||
                third == 0xAF) {
                i += 3;
                continue;
            }
        }
        output += c;
        ++i;
    }
    return output;
}

bool is_ascii(const std::string& word) {
    return std::all_of(word.begin(), word.end(), [](unsigned char c) {
        return c >= 32 && c <= 126;  // printable ASCII
        // or use: return c < 128; for strict 7-bit ASCII
    });
}

bool checkTagExists(std::string line, std::string tag) {
    if (line.rfind(tag, 0) != 0) {
        // std::cerr << "Malformed file. " << tag << " not found." << endl;
        return false;
    }
    return true;
}

DocStream::DocStream(std::string dirPath, std::string dictionaryPath)
    : _dirPath(dirPath) {
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        std::string filename = entry.path().filename();
        if (filename == "logs.txt") {
            continue;
        }
        _documents.push(filename);
    }

    std::ifstream dictFile(dictionaryPath);
    if (!dictFile.is_open()) {
        std::cerr << "Error opening dictionary" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string word;
    while (std::getline(dictFile, word)) {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace),
                   word.end());
        _dictionary.insert(word);
    }
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
        if (is_ascii(word)) {
            output.push_back(word_t{word, offset, wordlocation_t::title});
        }
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
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        word = strip_utf8_spaces(word);

        if (is_ascii(word)) {
            output.push_back(word_t{word, offset, wordlocation_t::title});
        }
        ++offset;
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</words>")) {
        return out;
    }
    out.words = output;

    // Check <links>  tag
    std::getline(document, line);
    if (!checkTagExists(line, "<links>")) {
        return out;
    }

    std::getline(document, line);
    uint32_t numOutLinks = 0;
    std::vector<std::string> outLinks;
    while (line != "</links>") {
        std::getline(document, line);
        numOutLinks++;
    }

    out.metadata = {static_cast<uint32_t>(output.size()),
                    numTitleWords,
                    numOutLinks,
                    0.0,
                    0.0,
                    docNum};

    //Check <prank> tag
    std::getline(document, line);
    if (!checkTagExists(line, "<prank>")) {
        return out;
    }

    float pageRank = 0.0;
    if (std::getline(document, line)) {
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
    if (std::getline(document, line)) {
        cheiRank = std::stof(line);
    }

    std::getline(document, line);
    if (!checkTagExists(line, "</crank>")) {
        return out;
    }

    out.metadata.pageRank = pageRank;
    out.metadata.cheiRank = cheiRank;

    return out;
}

size_t DocStream::size() {
    return _documents.size();
}
