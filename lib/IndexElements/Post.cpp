#include <spdlog/spdlog.h>

#include "Post.hpp"

Post::Post() {}

Post::Post(std::string name) : document_name(name), entries() {}

std::string Post::getDocumentName()
{
    return document_name;
}

std::vector<PostEntry> Post::getEntries()
{
    return entries;
}

void Post::addWord(PostEntry word)
{
    entries.push_back(word);
}

void Post::Serialize(char* base_region, size_t &offset, const Post& post)
{
    spdlog::info("Trying to serialize a Post for the document \"{}\"", post.document_name);
    spdlog::info("Offset variable is currently at {}", offset);

    // Serialize the document name
    size_t document_name_size = post.document_name.size() + 1; // account for null terminator
    std::memcpy(base_region + offset, post.document_name.c_str(), document_name_size);
    offset += document_name_size;
    spdlog::info("After writing the std::string document_name, offset is now at {}", offset);

    // Serialize the vector of word occurrences
    size_t num_words = post.entries.size();
    std::memcpy(base_region + offset, &num_words, sizeof(num_words));
    offset += sizeof(num_words);
    spdlog::info("After recording the size of the vec<PostEntry>, offset is now at {}", offset);

    // Serialize each PostEntry
    for (const auto& word : post.entries) {
        PostEntry::Serialize(base_region, offset, word);
    }

    spdlog::info("Finished serializing Post for the document \"{}\"", post.document_name);
    spdlog::info("Offset is now at {}", offset);
}

Post Post::Deserialize(char* base_region, size_t &offset)
{
    spdlog::info("Trying to deserialize a Post");
    spdlog::info("Offset variable is currently at {}", offset);

    Post post;

    // Deserialize the document name
    post.document_name = std::string(base_region + offset);
    offset += post.document_name.size() + 1;
    spdlog::info("After reading the std::string document_name, offset is now at {}", offset);

    // Deserialize the number of words in the vector
    size_t num_of_words;
    std::memcpy(&num_of_words, base_region + offset, sizeof(num_of_words));
    offset += sizeof(num_of_words);
    post.entries.resize(num_of_words);
    spdlog::info("After reading the size of the vector and resizing, offset is now at {}", offset);

    // Deserialize each PostEntry
    for (size_t i = 0; i < num_of_words; ++i) {
        post.entries[i] = PostEntry::Deserialize(base_region, offset);
    }

    spdlog::info("Deserialization of Post complete");
    spdlog::info("Offset is now at {}", offset);

    return post;
}
