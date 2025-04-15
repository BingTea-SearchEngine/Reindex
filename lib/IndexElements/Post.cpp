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
    // Serialize the document name
    size_t document_name_size =
        post.document_name.size() + 1;  // account for null terminator
    std::memcpy(base_region + offset, post.document_name.c_str(),
                document_name_size);
    offset += document_name_size;

    // Serialize the vector of word occurrences
    size_t num_words = post.entries.size();
    std::memcpy(base_region + offset, &num_words, sizeof(num_words));
    offset += sizeof(num_words);

    // Serialize each PostEntry
    for (const auto& word : post.entries) {
        PostEntry::Serialize(base_region, offset, word);
    }
}

Post Post::Deserialize(char* base_region, size_t& offset) {
    Post post;

    // Deserialize the document name
    post.document_name = std::string(base_region + offset);
    offset += post.document_name.size() + 1;

    // Deserialize the number of words in the vector
    size_t num_of_words;
    std::memcpy(&num_of_words, base_region + offset, sizeof(num_of_words));
    offset += sizeof(num_of_words);
    post.entries.resize(num_of_words);

    // Deserialize each PostEntry
    for (size_t i = 0; i < num_of_words; ++i) {
        post.entries[i] = PostEntry::Deserialize(base_region, offset);
    }

    return post;
}
