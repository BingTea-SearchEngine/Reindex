#include "ISRWord.hpp"

ISRWord::ISRWord(const PostingList& pL)
    : postingList(pL),
      currentPostIdx(-1),
      currentPostEntry(std::nullopt),
      currentPostEntryIdx(-1),
      absoluteLocation(-1),
      documentName("") {
    this->documentCount = this->postingList.GetPosts().size();

    for (auto& post : this->postingList.GetPosts()) {
        this->numOccurrences += post.GetEntries().size();
    }
}

int ISRWord::GetStartLocation() {
    return this->absoluteLocation;
}

int ISRWord::GetEndLocation() {
    return this->absoluteLocation;
}

int ISRWord::GetDocumentCount() {
    return this->documentCount;
}

int ISRWord::GetNumberOfOccurrences() {
    return this->numOccurrences;
}

std::optional<PostEntry> ISRWord::GetCurrentPostEntry() {
    return this->currentPostEntry;
}

std::string ISRWord::GetDocumentName() {
    return this->documentName;
}

std::optional<PostEntry> ISRWord::Next() {
    int outerPost = this->currentPostIdx;
    int innerPostEntry = this->currentPostEntryIdx + 1; // try to initially move to the next entry

    const auto& posts = this->postingList.GetPosts();
    for (; outerPost < posts.size(); ++outerPost) { // this for loop guaranteed to only run 0, 1, or 2 times
        auto& post = posts[outerPost];
        const auto& entries = post.GetEntries();
        const std::string currDocumentName = post.GetDocumentName();

        if (innerPostEntry < entries.size()) {
            const auto& postEntry = entries[innerPostEntry];

            this->currentPostIdx = outerPost;
            this->currentPostEntryIdx = innerPostEntry;
            this->currentPostEntry = postEntry;
            this->absoluteLocation = postEntry.GetDelta();
            this->documentName = currDocumentName;
            return this->currentPostEntry;
        }

        innerPostEntry = 0; // reset for next post
    }

    // no more entries found at all
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::NextDocument() {
    const auto& posts = this->postingList.GetPosts();
    int nextPostIdx = this->currentPostIdx + 1;
    
    if (nextPostIdx < posts.size()) {
        const auto& post = posts[nextPostIdx];
        const auto& entries = post.GetEntries();

        this->currentPostIdx = nextPostIdx;
        this->currentPostEntryIdx = 0;
        this->currentPostEntry = entries[0];
        this->absoluteLocation = entries[0].GetDelta();
        this->documentName = post.GetDocumentName();
        return this->currentPostEntry;
    }

    // this was already at the last document
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::Seek(size_t target) {
    int outerPost = 0;
    int innerPostEntry = 0;

    for (auto post : this->postingList.GetPosts()) {
        std::string currDocumentName = post.GetDocumentName();

        for (auto postEntry : post.GetEntries()) {
            if (postEntry.GetDelta() >= target) {
                this->currentPostIdx = outerPost;
                this->currentPostEntryIdx = innerPostEntry;
                this->currentPostEntry = postEntry;
                this->absoluteLocation = postEntry.GetDelta();
                this->documentName = currDocumentName;
                return this->currentPostEntry;
            }

            innerPostEntry++;
        }

        outerPost++;
    }

    // no PostEntry was found at a location >= target
    this->currentPostEntry = std::nullopt;
    return std::nullopt;

    // TODO: implement seeking for PostingList
    // so that we can do something like this?
    // but then how would internal state change?
    // return this->postingList->Seek(target);
}