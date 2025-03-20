#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "PostingList.hpp"
#include "cstring"

PostingList::PostingList() {}

PostingList::PostingList(const std::string& _word) : word(_word) {}

void PostingList::Serialize(char* base_region, size_t &offset, const PostingList& postingList) {
    /*
        A PostingList object has:
            - vector<Post> posts
            - TODO: 100% need a synchronization table
    */

    spdlog::info("Trying to serialize a PostingList for the word {}", postingList.word);
    spdlog::info("offset variable is currently at {}", offset);

    size_t num_posts = postingList._posts.size();
    std::memcpy(base_region + offset, &num_posts, sizeof(num_posts));
    offset += sizeof(num_posts);

    spdlog::info("Before trying to serialize each Post, offset is currently at {}", offset);
    for (const auto& post : postingList._posts) {
        Post::Serialize(base_region, offset, post);
    }

    spdlog::info("Finished serializing PostingList for the word {}", postingList.word);
    spdlog::info("Offset is now at {}", offset);
}

size_t PostingList::getOverheadBytes() {
    size_t ret;
    ret += sizeof(_posts.size());
    ret += word.size() + 1; // Word and null terminator
    return ret;
}; 

size_t PostingList::addWord(docname doc, word_t word) {
    // If docname is different from current post's document, add to back of vector
    if(doc != _posts.back().document) {
        _posts.emplace_back(doc);
    }
    return _posts.back().addWord(word);
}

Post PostingList::getPost(size_t index) {
    return _posts[index];
}

std::vector<Post>::iterator PostingList::begin() {
    return _posts.begin();
}

std::vector<Post>::iterator PostingList::end() {
    return _posts.end();
}
