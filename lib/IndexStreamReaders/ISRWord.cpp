#include <cassert>

#include "ISRWord.hpp"

ISRWord::ISRWord(const PostingList* pL)
    : postingList(pL),
      currentPostIdx(-1),
      currentPostEntry(std::nullopt),
      currentPostEntryIdx(-1),
      absoluteLocation(-1),
      documentID(0),
      numOccurrences(0) {
    this->documentCount = this->postingList.GetPosts().size();

    for (auto& post : this->postingList->GetPosts()) {
        this->numOccurrences += post.GetEntries().size();
    }
}

int ISRWord::GetStartLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetStartLocation called when this ISR is not pointing to anything");
    return this->absoluteLocation;
}

int ISRWord::GetEndLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetEndLocation called when this ISR is not pointing to anything");
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

uint32_t ISRWord::GetDocumentID() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentID called when this ISR is not pointing to anything");
    return this->documentID;
}

std::optional<PostEntry> ISRWord::Next() {
    int outerPost = this->currentPostIdx;
    if (outerPost == -1) {
        outerPost++;
    }

    int innerPostEntry =
        this->currentPostEntryIdx + 1;  // try to move to the next adjacent entry at first

    const auto posts = this->postingList->GetPosts();
    for (; outerPost < posts.size();
         ++outerPost) {  // this for loop guaranteed to only run 0, 1, or 2 times
        assert(outerPost >= 0 && outerPost < posts.size());
        auto post = posts[outerPost];
        const auto entries = post.GetEntries();
        const uint32_t documentID = post.GetDocumentID();

        if (innerPostEntry < entries.size()) {
            assert(innerPostEntry >= 0 && innerPostEntry < entries.size());
            const auto postEntry = entries[innerPostEntry];

            this->currentPostIdx = outerPost;
            this->currentPostEntryIdx = innerPostEntry;
            this->currentPostEntry = postEntry;
            this->absoluteLocation = postEntry.GetDelta();
            this->documentID = documentID;
            return this->currentPostEntry;
        }

        innerPostEntry = 0;  // reset for next post
    }

    // no more entries found at all
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::NextDocument() {
    const auto posts = this->postingList->GetPosts();
    int nextPostIdx = this->currentPostIdx + 1;

    if (nextPostIdx < posts.size()) {
        assert(nextPostIdx >= 0 && nextPostIdx < posts.size());
        const auto post = posts[nextPostIdx];
        const auto entries = post.GetEntries();

        this->currentPostIdx = nextPostIdx;
        this->currentPostEntryIdx = 0;
        this->currentPostEntry = entries[0];
        this->absoluteLocation = entries[0].GetDelta();
        this->documentID = post.GetDocumentID();
        return this->currentPostEntry;
    }

    // this was already at the last document
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::Seek(size_t target) {
    // TODO: implement seeking for PostingList
    // so that we can do something like this?
    // but then how would internal state change?
    // return this->postingList->Seek(target);
    // Do this with synchronization table.
    int outerPost = 0;

    for (auto post : this->postingList->GetPosts()) {
        int innerPostEntry = 0;
        uint32_t currDocumentID = post.GetDocumentID();

        for (auto postEntry : post.GetEntries()) {
            if (postEntry.GetDelta() >= target) {
                this->currentPostIdx = outerPost;
                this->currentPostEntryIdx = innerPostEntry;
                this->currentPostEntry = postEntry;
                this->absoluteLocation = postEntry.GetDelta();
                this->documentID = currDocumentID;
                return this->currentPostEntry;
            }

            innerPostEntry++;
        }

        outerPost++;
    }

    // no PostEntry was found at a location >= target
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}
