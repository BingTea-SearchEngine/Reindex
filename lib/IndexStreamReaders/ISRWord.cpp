#include "ISRWord.hpp"

ISRWord::ISRWord(PostingList* pL) : postingList(pL), currentPostIdx(-1),
                                    currentPostEntry(nullptr), currentPostEntryIdx(-1),
                                    absoluteLocation(-1) {}

size_t ISRWord::GetDocumentCount() {
    return postingList->GetPosts().size();
}

size_t ISRWord::GetNumberOfOccurrences() {
    size_t occurrences = 0;

    for (auto& post : postingList->GetPosts()) {
        occurrences += post.GetEntries().size();
    }

    return occurrences;
}

PostEntry* ISRWord::GetCurrentPost() {
    return currentPostEntry;
}

PostEntry* ISRWord::Next() {
    // TODO: inefficient because going through one by one
    // one at a time -- is there a fix? (probably)
    
    size_t outerPost = 0;
    size_t innerPostEntry = 0;

    for (auto& post : this->postingList->GetPosts()) {
        for (auto& postEntry : post.GetEntries()) {
            if (innerPostEntry > this->currentPostEntryIdx) {
                this->currentPostIdx = outerPost;
                this->currentPostEntryIdx = innerPostEntry;
                this->currentPostEntry = &postEntry;
                this->absoluteLocation = postEntry.GetDelta();
                return this->currentPostEntry;
            }

            innerPostEntry++;
        }

        outerPost++;
    }

    // this->currentPostEntry was already pointing to the very last
    // PostEntry within this PostingList
    return nullptr;
}

PostEntry* ISRWord::NextDocument() {
    // TODO: inefficient because going through one by one
    // one at a time -- is there a fix? (probably)

    size_t outerPost = 0;
    size_t innerPostEntry = 0;

    for (auto& post : this->postingList->GetPosts()) {
        if (this->currentPostIdx >= outerPost) {
            // this post is not the immediate next of our current one
            outerPost++;
            innerPostEntry += post.GetEntries().size();
        }
        else {
            // found the first Post belonging to the next document
            // now extract the first PostEntry within this Post
            this->currentPostIdx = outerPost;
            this->currentPostEntryIdx = innerPostEntry;

            auto& postEntry = post.GetEntries()[0]; // just need to grab the first one // TODO: inefficient but negligible?
            this->currentPostEntry = &postEntry;
            this->absoluteLocation = postEntry.GetDelta();
            return this->currentPostEntry;
        }
    }

    // the current Post that this ISR was pointing to was
    // already the last one within this PostingList
    return nullptr;
}

PostEntry* ISRWord::Seek(size_t target) {
    size_t outerPost = 0;
    size_t innerPostEntry = 0;

    for (auto& post : this->postingList->GetPosts()) {
        for (auto& postEntry : post.GetEntries()) {
            if (postEntry.GetDelta() >= target) {
                this->currentPostIdx = outerPost;
                this->currentPostEntryIdx = innerPostEntry;
                this->currentPostEntry = &postEntry;
                this->absoluteLocation = postEntry.GetDelta();
                return this->currentPostEntry;
            }

            innerPostEntry++;
        }

        outerPost++;
    }

    // no PostEntry was found at a location >= target
    return nullptr;

    // TODO: implement seeking for PostingList
    // so that we can do something like this?
    // but then how would internal state change?
    // return this->postingList->Seek(target);
}