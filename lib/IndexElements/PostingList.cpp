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

PostingList PostingList::Deserialize(char* base_region, size_t& offset) {
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
