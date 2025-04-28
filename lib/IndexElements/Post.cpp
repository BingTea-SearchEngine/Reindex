#include <spdlog/spdlog.h>

#include "Post.hpp"

Post::Post() {}

Post::Post(uint32_t docID, size_t earliestOccurrenceInDoc)
    : docID(docID), docStart(earliestOccurrenceInDoc), entries() {}

uint32_t Post::GetDocumentID() const {
    return docID;
}

size_t Post::GetEarliestStart() const {
    return docStart;
}

std::vector<PostEntry> Post::GetEntries() const {
    return entries;
}

void Post::AddWord(PostEntry word) {
    entries.push_back(word);
}

void Post::Print() const {
    cout << "\tPost{ " << docID << " }: " << entries.size() << " entries" << endl;
    cout << "\t\t";
    for (const PostEntry& entry : entries) {
        entry.Print();
        cout << " | ";
    }
    cout << endl;
}

void Post::Serialize(char* base_region, size_t& offset, const Post& post) {
    // // Serialize size of document name
    // uint16_t document_name_size =
    //     static_cast<uint16_t>(post.document_name.size());
    // std::memcpy(base_region + offset, &document_name_size,
    //             sizeof(document_name_size));
    // offset += sizeof(document_name_size);

    // // Serialize document name
    // std::memcpy(base_region + offset, post.document_name.c_str(),
    //             document_name_size);
    // offset += document_name_size;

    // Serialize the ID of this document
    std::memcpy(base_region + offset, &post.docID, sizeof(post.docID));
    offset += sizeof(post.docID);

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

    // // Deserialize size of document name
    // uint16_t document_name_size;
    // std::memcpy(&document_name_size, base_region + offset,
    //             sizeof(document_name_size));
    // offset += sizeof(document_name_size);

    // // Deserialize document name
    // std::string document_name(document_name_size, '\0');
    // std::memcpy(document_name.data(), base_region + offset, document_name_size);
    // offset += document_name_size;
    // post.document_name = document_name;

    uint32_t docID = 0;
    std::memcpy(&docID, base_region + offset, sizeof(docID));
    offset += sizeof(docID);
    post.docID = docID;

    // Deserialize the number of words in the vector
    uint32_t num_of_words;
    std::memcpy(&num_of_words, base_region + offset, sizeof(num_of_words));
    offset += sizeof(num_of_words);
    post.entries.reserve(num_of_words);

    // Deserialize each PostEntry
    for (size_t i = 0; i < num_of_words; ++i) {
        post.entries.emplace_back(PostEntry::Deserialize(base_region, offset));
    }

    return post;
}
