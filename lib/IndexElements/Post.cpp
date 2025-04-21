#include <spdlog/spdlog.h>

#include "Post.hpp"

Post::Post() {}

Post::Post(std::string name) : document_name(name), entries() {}

std::string Post::GetDocumentName() const {
    return document_name;
}

std::vector<PostEntry> Post::GetEntries() const {
    return entries;
}

void Post::AddWord(PostEntry word) {
    entries.push_back(word);
}

void Post::Print() const {
    cout << "\tPost{ " << document_name << " }: " << entries.size() << " entries" << endl;
    cout << "\t\t";
    for (const PostEntry& entry : entries) {
        entry.Print();
        cout << " | ";
    }
    cout << endl;
}

void Post::Serialize(char* base_region, size_t& offset, const Post& post) {
    // Serialize size of document name
    uint16_t document_name_size = static_cast<uint16_t>(post.document_name.size());
    std::memcpy(base_region + offset, &document_name_size, sizeof(document_name_size));
    offset += sizeof(document_name_size);

    // Serialize document name
    std::memcpy(base_region + offset, post.document_name.c_str(), document_name_size);
    offset += document_name_size;

    // Serialize the vector of word occurrences
    uint32_t num_words = static_cast<uint32_t>(post.entries.size());
    std::memcpy(base_region + offset, &num_words, sizeof(num_words));
    offset += sizeof(num_words);

    // Serialize each PostEntry
    for (const auto& word : post.entries) {
        PostEntry::Serialize(base_region, offset, word);
    }
}

Post Post::Deserialize(char* base_region, size_t& offset) {
    Post post;

    // Deserialize size of document name
    uint16_t document_name_size;
    std::memcpy(&document_name_size, base_region + offset, sizeof(document_name_size));
    offset += sizeof(document_name_size);

    // Deserialize document name
    std::string document_name(document_name_size, '\0');
    std::memcpy(document_name.data(), base_region + offset, document_name_size);
    offset += document_name_size;
    post.document_name = document_name;

    // Deserialize the number of words in the vector
    uint32_t num_of_words;
    std::memcpy(&num_of_words, base_region + offset, sizeof(num_of_words));
    offset += sizeof(num_of_words);
    post.entries.resize(num_of_words);

    // Deserialize each PostEntry
    for (size_t i = 0; i < num_of_words; ++i) {
        post.entries[i] = std::move(PostEntry::Deserialize(base_region, offset));
    }

    return post;
}
