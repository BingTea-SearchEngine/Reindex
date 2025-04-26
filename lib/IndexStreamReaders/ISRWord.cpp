#include <cassert>

#include "ISRWord.hpp"

ISRWord::ISRWord(const PostingList& pL)
    : postingList(pL),
      currentPostIdx(-1),
      currentPostEntry(std::nullopt),
      currentPostEntryIdx(-1),
      absoluteLocation(-1),
      documentID(0),
      numOccurrences(0) {
    this->documentCount = this->postingList.GetPosts().size();

    for (auto& post : this->postingList.GetPosts()) {
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

size_t ISRWord::GetDocumentStart() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentStart called when this ISR is not pointing to anything");
    return this->docStart;
}

std::optional<PostEntry> ISRWord::Next() {
    int outerPost = this->currentPostIdx;
    if (outerPost == -1) {
        outerPost++;
    }

    int innerPostEntry = this->currentPostEntryIdx +
                         1;  // try to move to the next adjacent entry at first

    const auto posts = this->postingList.GetPosts();
    for (
        ; outerPost < posts.size();
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
            this->docStart = post.GetEarliestStart();
            return this->currentPostEntry;
        }

        innerPostEntry = 0;  // reset for next post
    }

    // no more entries found at all
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::NextDocument() {
    const auto posts = this->postingList.GetPosts();
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
        this->docStart = post.GetEarliestStart();
        return this->currentPostEntry;
    }

    // this was already at the last document
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRWord::Seek(size_t target) {
    return ISRWord::NewSeek(target);
}

std::optional<PostEntry> ISRWord::OldSeek(size_t target) {
    int outerPost = 0;

    for (auto post : this->postingList.GetPosts()) {
        int innerPostEntry = 0;
        uint32_t currDocumentID = post.GetDocumentID();

        for (auto postEntry : post.GetEntries()) {
            if (postEntry.GetDelta() >= target) {
                this->currentPostIdx = outerPost;
                this->currentPostEntryIdx = innerPostEntry;
                this->currentPostEntry = postEntry;
                this->absoluteLocation = postEntry.GetDelta();
                this->documentID = currDocumentID;
                this->docStart = post.GetEarliestStart();
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

std::optional<PostEntry> ISRWord::NewSeek(size_t target) {
    const auto& posts = this->postingList.GetPosts();
    const auto& sync_table = this->postingList.GetSyncTable();

    // Step 1: Binary search sync_table
    size_t left = 0, right = sync_table.size();
    size_t start_idx = 0;
    while (left < right) {
        size_t mid = (left + right) / 2;
        assert(mid >= 0 && mid < sync_table.size());
        if (sync_table[mid].position <= target) {
            start_idx = mid;
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // Step 2: Start linear scan from sync_table[start_idx]
    this->currentPostIdx = 0;
    this->currentPostEntryIdx = 0;
    
    if (start_idx < sync_table.size()) {
        const auto& sync = sync_table[start_idx];

        this->currentPostIdx = sync.post_idx;
        this->currentPostEntryIdx = sync.entry_idx;
    }
    bool first = true;
    for (size_t post_idx = this->currentPostIdx; post_idx < posts.size(); ++post_idx) {
        const Post& post = posts[post_idx];
        size_t entry_start = 0;

        if (first) {
            entry_start = this->currentPostEntryIdx;
            first = false;
        }

        const auto& entries = post.GetEntries();

        for (size_t entry_idx = entry_start; entry_idx < entries.size(); ++entry_idx) {
            const PostEntry& entry = entries[entry_idx];
            if (entry.GetDelta() >= target) {
                this->currentPostIdx = post_idx;
                this->currentPostEntryIdx = entry_idx;
                this->currentPostEntry = entry;
                this->absoluteLocation = entry.GetDelta();
                this->documentID = post.GetDocumentID();
                this->docStart = post.GetEarliestStart();
                return this->currentPostEntry;
            }
        }
    }

    // No match found
    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}