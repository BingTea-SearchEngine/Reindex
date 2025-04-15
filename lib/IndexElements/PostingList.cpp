#include "Post.hpp"
#include "spdlog/spdlog.h"

#include "PostingList.hpp"

PostingList::PostingList() {}

PostingList::PostingList(const std::string& word) : word(word) {}

size_t PostingList::GetOverheadBytesRequired() {
    return word.size()+1+sizeof(posts.size());
}

std::string PostingList::GetWord() {
    return word;
}

size_t PostingList::AddWord(std::string doc, PostEntry word) {
    size_t bytesRequired = 0;
    if (posts.empty()) {
        posts.emplace_back(doc);
        bytesRequired+=doc.size()+1;
    }

    if (doc != posts.back().GetDocumentName()) {
        posts.emplace_back(doc);
        bytesRequired+=doc.size()+1;
    }

    posts.back().AddWord(word);
    bytesRequired+=word.GetBytesRequired();
    return bytesRequired;
}

void PostingList::Print() const {
    cout << "PostingList{ " << word << " }: ";
    cout << posts.size() << " document(s)" << endl;
    for (const Post& post : posts) {
        post.Print();
    }
}

std::vector<Post> PostingList::GetPosts() {
    return posts;
}

void PostingList::Serialize(char* base_region, size_t& offset,
                            const PostingList& postingList) {
    PostingList::OldSerialize(base_region, offset, postingList);
}

void PostingList::OldSerialize(char* base_region, size_t& offset,
                               const PostingList& postingList) {
    // Serialize the word representing the PostingList
    size_t postingList_word_size = postingList.word.size() + 1;
    std::memcpy(base_region + offset, postingList.word.c_str(),
                postingList_word_size);
    offset += postingList_word_size;

    // Serialize the vector of posts
    size_t num_posts = postingList.posts.size();
    std::memcpy(base_region + offset, &num_posts, sizeof(num_posts));
    offset += sizeof(num_posts);

    for (const auto& post : postingList.posts) {
        Post::Serialize(base_region, offset, post);
    }
}

std::vector<uint8_t> encodeVB(uint32_t number) {
    std::vector<uint8_t> bytes;
    do {
        uint8_t byte = number & 0x7F; // get 7 LSB
        bytes.push_back(byte);
        number >>= 7;
    } while (number > 0);

    std::reverse(bytes.begin(), bytes.end());

    // Set stop bit (MSB) in last byte
    bytes.back() |= 0x80;
    return bytes;
}

uint32_t decodeVB(const std::vector<uint8_t>& bytes, size_t& index) {
    uint32_t n = 0;
    while (index < bytes.size()) {
        uint8_t byte = bytes[index++];
        n = (n << 7) | (byte & 0x7F);
        if (byte & 0x80) {
            break; // this is the last byte
        }
    }
    return n;
}

void PostingList::NewSerialize(char* base_region, size_t& offset,
                               const PostingList& postingList) {
    // Serialize the word
    size_t word_len = postingList.word.size() + 1;
    std::memcpy(base_region + offset, postingList.word.c_str(), word_len);
    offset += word_len;

    // Serialize the number of posts (documents)
    size_t num_posts = postingList.posts.size();
    std::memcpy(base_region + offset, &num_posts, sizeof(num_posts));
    offset += sizeof(num_posts);

    uint32_t prev_position = 0;
    for (auto& post : postingList.posts) {
        // Serialize the document name
        size_t document_name_size = post.GetDocumentName().size() + 1;  // account for null terminator
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
            uint8_t length = encoded.size();
            std::memcpy(base_region + offset, &length, sizeof(length));
            offset += sizeof(length);

            std::memcpy(base_region + offset, encoded.data(), length);
            offset += length;

            // Serialize location_found (it is an enum so fits in a byte)
            uint8_t location = static_cast<uint8_t>(entry.GetLocationFound());
            std::memcpy(base_region + offset, &location, sizeof(location));
            offset += sizeof(location);
        }
    }
}

PostingList PostingList::Deserialize(char* base_region, size_t& offset) {
    return PostingList::OldDeserialize(base_region, offset);
}

PostingList PostingList::OldDeserialize(char* base_region, size_t& offset) {
    PostingList postingList;

    // Deserialize the word associated with the PostingList
    postingList.word = std::string(base_region + offset);
    offset += postingList.word.size() + 1;

    // Deserialize the number of posts
    size_t num_of_posts;
    std::memcpy(&num_of_posts, base_region + offset, sizeof(num_of_posts));
    offset += sizeof(num_of_posts);
    postingList.posts.resize(num_of_posts);

    // Deserialize each post in the vector
    for (size_t i = 0; i < num_of_posts; ++i) {
        postingList.posts[i] = Post::Deserialize(base_region, offset);
    }

    return postingList;
}

PostingList PostingList::NewDeserialize(char* base_region, size_t& offset) {
    PostingList list;
    
    // Deserialize the word
    list.word = std::string(base_region + offset);
    offset += list.word.size() + 1;

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

        uint32_t num_entries;
        std::memcpy(&num_entries, base_region + offset, sizeof(num_entries));
        offset += sizeof(num_entries);

        for (uint32_t j = 0; j < num_entries; ++j) {
            uint8_t length;
            std::memcpy(&length, base_region + offset, sizeof(length));
            offset += sizeof(length);

            std::vector<uint8_t> vb_bytes(length);
            std::memcpy(vb_bytes.data(), base_region + offset, length);
            offset += length;

            size_t idx = 0;
            uint32_t delta = decodeVB(vb_bytes, idx);
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