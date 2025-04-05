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
    // TODO: inefficient because going through one by one
    // one at a time -- is there a fix? (probably)

    int outerPost = 0;
    int innerPostEntry = 0;

    for (auto post : this->postingList.GetPosts()) {
        std::string currDocumentName = post.GetDocumentName();

        for (auto postEntry : post.GetEntries()) {
            if (innerPostEntry > this->currentPostEntryIdx) {
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

    // this->currentPostEntry was already pointing to the very last
    // PostEntry within this PostingList
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::NextDocument() {
    // TODO: inefficient because going through one by one
    // one at a time -- is there a fix? (probably)

    int outerPost = 0;
    int innerPostEntry = 0;

    for (auto post : this->postingList.GetPosts()) {
        std::string currDocumentName = post.GetDocumentName();

        if (this->currentPostIdx >= outerPost) {
            // this post is not the immediate next of our current one
            outerPost++;
            innerPostEntry += post.GetEntries().size();
        } else {
            // found the first Post belonging to the next document
            // now extract the first PostEntry within this Post
            this->currentPostIdx = outerPost;
            this->currentPostEntryIdx = innerPostEntry;

            auto postEntry =
                post.GetEntries()[0];  // just need to grab the first one
            this->currentPostEntry = postEntry;
            this->absoluteLocation = postEntry.GetDelta();
            this->documentName = currDocumentName;
            return this->currentPostEntry;
        }
    }

    // the current Post that this ISR was pointing to was
    // already the last one within this PostingList
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