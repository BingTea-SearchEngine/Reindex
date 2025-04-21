#include "Post.hpp"
#include "spdlog/spdlog.h"

#include "PostingList.hpp"

PostingList::PostingList() {}

PostingList::PostingList(const std::string& word) : word(word) {}

size_t PostingList::GetOverheadBytesRequired() {
    return word.size() + 1 + sizeof(posts.size());
}

std::string PostingList::GetWord() {
    return word;
}

size_t PostingList::AddWord(std::string doc, PostEntry word) {
    size_t bytesRequired = 0;
    if (posts.empty()) {
        posts.emplace_back(doc);
        bytesRequired += doc.size() + 1;
    }

    if (doc != posts.back().GetDocumentName()) {
        posts.emplace_back(doc);
        bytesRequired += doc.size() + 1;
    }

    posts.back().AddWord(word);
    bytesRequired += word.GetBytesRequired();
    return bytesRequired;
}

void PostingList::Print() const {
    cout << "PostingList{ " << word << " }: ";
    cout << posts.size() << " document(s)" << endl;
    for (const Post& post : posts) {
        post.Print();
    }
}

std::vector<Post> PostingList::GetPosts() const {
    return posts;
}

void PostingList::Serialize(char* base_region, size_t& offset, const PostingList& postingList) {
    // PostingList::NewSerialize(base_region, offset, postingList);
    PostingList::OldSerialize(base_region, offset, postingList);
}

void PostingList::OldSerialize(char* base_region, size_t& offset, const PostingList& postingList) {
    // Serialize the word representing the PostingList
    uint16_t word_size = postingList.word.size();
    std::memcpy(base_region + offset, &word_size, sizeof(word_size));
    offset += sizeof(word_size);

    std::memcpy(base_region + offset, postingList.word.c_str(), word_size);
    offset += word_size;

    // Serialize the vector of posts
    size_t num_posts = postingList.posts.size();
    std::memcpy(base_region + offset, &num_posts, sizeof(num_posts));
    offset += sizeof(num_posts);

    for (const auto& post : postingList.posts) {
        Post::Serialize(base_region, offset, post);
    }
}

std::vector<uint8_t> encodeVB(uint32_t value) {
    std::vector<uint8_t> bytes;
    do {
        uint8_t byte = value & 0x7F;  // Get the lowest 7 bits
        value >>= 7;
        if (value != 0) {
            bytes.push_back(byte);  // More bytes to come
        } else {
            byte |= 0x80;  // Set MSB = 1 to mark end
            bytes.push_back(byte);
            break;
        }
    } while (true);

    return bytes;
}

uint32_t decodeVB(const uint8_t* data, size_t& consumed_bytes) {
    uint32_t result = 0;
    int shift = 0;
    consumed_bytes = 0;

    while (true) {
        uint8_t byte = data[consumed_bytes++];
        result |= (byte & 0x7F) << shift;
        if (byte & 0x80)
            break;
        shift += 7;
    }

    return result;
}

void PostingList::NewSerialize(char* base_region, size_t& offset, const PostingList& postingList) {
    // Serialize size of word
    uint16_t word_size = static_cast<uint16_t>(postingList.word.size());
    std::memcpy(base_region + offset, &word_size, sizeof(word_size));
    offset += sizeof(word_size);

    // Serialize the word representing the PostingList
    std::memcpy(base_region + offset, postingList.word.c_str(), word_size);
    offset += word_size;

    // Serialize the number of posts (documents)
    size_t num_posts = postingList.posts.size();
    std::memcpy(base_region + offset, &num_posts, sizeof(num_posts));
    offset += sizeof(num_posts);

    uint32_t prev_position = 0;
    for (auto& post : postingList.posts) {
        // Serialize the document name
        size_t document_name_size =
            post.GetDocumentName().size() + 1;  // account for null terminator
        std::memcpy(base_region + offset, post.GetDocumentName().c_str(), document_name_size);
        offset += document_name_size;

        const auto& entries = post.GetEntries();
        size_t num_entries = entries.size();
        std::memcpy(base_region + offset, &num_entries, sizeof(num_entries));
        offset += sizeof(num_entries);

        // Delta + VB encoding for each PostEntry
        for (const auto& entry : entries) {
            uint32_t abs_pos = entry.GetDelta();  // assume this is absolute
            uint32_t delta = abs_pos - prev_position;
            prev_position = abs_pos;

            // Variable Byte encode the delta
            std::vector<uint8_t> encoded = encodeVB(delta);
            for (uint8_t b : encoded) {
                std::memcpy(base_region + offset, &b, sizeof(b));
                offset += sizeof(b);
            }

            // Serialize location_found (it is an enum so fits in a byte)
            uint8_t location = static_cast<uint8_t>(entry.GetLocationFound());
            std::memcpy(base_region + offset, &location, sizeof(location));
            offset += sizeof(location);
        }
    }
}

// 10000101
// 0001000 1000101

PostingList PostingList::Deserialize(char* base_region, size_t& offset) {
    // return PostingList::NewDeserialize(base_region, offset);
    return PostingList::OldDeserialize(base_region, offset);
}

PostingList PostingList::OldDeserialize(char* base_region, size_t& offset) {
    PostingList postingList;

    // Deserialize word size
    uint16_t word_size;
    std::memcpy(&word_size, base_region + offset, sizeof(word_size));
    offset += sizeof(word_size);

    // Deserialize the word associated with the PostingList
    std::string word(word_size, '\0');
    std::memcpy(word.data(), base_region + offset, word_size);
    offset += word_size;
    postingList.word = word;

    // Deserialize the number of posts
    size_t num_of_posts;
    std::memcpy(&num_of_posts, base_region + offset, sizeof(num_of_posts));
    offset += sizeof(num_of_posts);
    postingList.posts.reserve(num_of_posts);

    // Deserialize each post in the vector
    for (size_t i = 0; i < num_of_posts; ++i) {
        // postingList.posts[i] = Post::Deserialize(base_region, offset);
        postingList.posts.emplace_back(std::move(Post::Deserialize(base_region, offset)));
    }

    return postingList;
}

PostingList PostingList::NewDeserialize(char* base_region, size_t& offset) {
    PostingList list;

    // Deserialize word size
    uint16_t word_size;
    std::memcpy(&word_size, base_region + offset, sizeof(word_size));
    offset += sizeof(word_size);

    // Deserialize the word associated with the PostingList
    std::string word(word_size, '\0');
    std::memcpy(word.data(), base_region + offset, word_size);
    offset += word_size;
    list.word = word;

    // Read number of posts
    size_t num_posts;
    std::memcpy(&num_posts, base_region + offset, sizeof(num_posts));
    offset += sizeof(num_posts);
    list.posts.resize(num_posts);

    uint32_t prev_pos = 0;
    for (size_t i = 0; i < num_posts; ++i) {
        std::string docName(base_region + offset);

        Post post(docName);
        offset += post.GetDocumentName().size() + 1;

        size_t num_entries;
        std::memcpy(&num_entries, base_region + offset, sizeof(num_entries));
        offset += sizeof(num_entries);

        for (uint32_t j = 0; j < num_entries; ++j) {
            size_t bytes_read = 0;
            uint32_t delta =
                decodeVB(reinterpret_cast<const uint8_t*>(base_region + offset), bytes_read);
            offset += bytes_read;
            uint32_t abs_pos = prev_pos + delta;
            prev_pos = abs_pos;

            uint8_t location_raw;
            std::memcpy(&location_raw, base_region + offset, sizeof(location_raw));
            offset += sizeof(location_raw);

            wordlocation_t location = static_cast<wordlocation_t>(location_raw);
            PostEntry entry(abs_pos, location);
            post.AddWord(entry);
        }

        list.posts[i] = post;
    }

    return list;
}
