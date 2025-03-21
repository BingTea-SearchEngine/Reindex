#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "Post.hpp"
#include <cstring>

Post::Post() {}

Post::Post(docname name) : document(name), _entries() {}

words Post::getEntries() {
    return _entries;
}

void Post::Serialize(char* base_region, size_t &offset, const Post& post) {
    /*
        Post object has:
            - string document_name ("cnn.com/index.html")
            - vector<word_t> _entries
    */
    spdlog::info("offset variable is currently at {}", offset);

    size_t document_name_size = post.document.size() + 1; // account for null terminator
    std::memcpy(base_region + offset, post.document.c_str(), document_name_size);
    offset += document_name_size;
    spdlog::info("After writing the std::string document_name, offset is now at {}", offset);

    // serialize the vector of words

    // first, record the number of entries in the vector
    size_t num_words = post._entries.size();
    std::memcpy(base_region + offset, &num_words, sizeof(num_words));
    offset += sizeof(num_words);
    spdlog::info("After recording the size of the vec<word_t>, offset is now at {}", offset);

    // then, serialize each individual word occurrence
    for (const auto& word: post._entries) {
        word_t::Serialize(base_region, offset, word);
    }

    spdlog::info("Finished serializing Post for the document \"{}\"", post.document);
    spdlog::info("Offset is now at {}", offset);
}

Post Post::Deserialize(char* base_region, size_t &offset) {
    spdlog::info("Attempting to deserialize a Post at location {} + {} = {}", base_region, offset, base_region + offset);
    spdlog::info("offset variable is currently at {}", offset);

    Post post;

    post.document = std::string(base_region + offset);
    offset += post.document.size() + 1;
    spdlog::info("After reading the std::string document_name, offset is now at {}", offset);

    size_t num_of_words;
    std::memcpy(&num_of_words, base_region + offset, sizeof(num_of_words));
    offset += sizeof(num_of_words);
    post._entries.resize(num_of_words);
    spdlog::info("After reading the size of the vector and resizing, offset is now at {}", offset);

    for (size_t i = 0; i < num_of_words; ++i) {
        post._entries[i] = word_t::Deserialize(base_region, offset);
    }

    spdlog::info("Deserializing a Post complete");
    spdlog::info("Offset is now at {}", offset);

    return post;
}

size_t Post::addWord(word_t word) {
    _entries.push_back(word);
    return word.getBytesRequired();
}

words::iterator Post::begin() {
    return _entries.begin();
}

words::iterator Post::end() {
    return _entries.end();
}

