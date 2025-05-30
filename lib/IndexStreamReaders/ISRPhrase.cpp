#include <cassert>

#include "ISRPhrase.hpp"

ISRPhrase::ISRPhrase(std::vector<ISR*> children)
    : childISRs(children),
      currentPostEntry(std::nullopt),
      nearestTerm(-1),
      farthestTerm(-1),
      nearestStartLocation(-1),
      nearestEndLocation(-1) {}

ISRPhrase::~ISRPhrase() {
    for (auto child : childISRs) {
        delete child;
    }
}

int ISRPhrase::GetStartLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetStartLocation called when this ISR is not pointing to anything");
    return this->nearestStartLocation;
}

int ISRPhrase::GetEndLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetEndLocation called when this ISR is not pointing to anything");
    return this->nearestEndLocation;
}

std::optional<PostEntry> ISRPhrase::GetCurrentPostEntry() {
    return this->currentPostEntry;
}

uint32_t ISRPhrase::GetDocumentID() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentID called when this ISR is not pointing to anything");
    return this->childISRs[nearestTerm]->GetDocumentID();
}

size_t ISRPhrase::GetDocumentStart() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentStart called when this ISR is not pointing to anything");
    return this->childISRs[nearestTerm]->GetDocumentStart();
}

// PRECONDITION: for this helper function to be called,
//               ALL of the children must be valid still
void ISRPhrase::UpdateMarkers() {
    int whichChildEarliest;
    int whichChildLatest;
    int nearestStart = INT32_MAX;
    int nearestEnd = 0;

    // figure out who is now the newest earliest occurrence and other variables
    for (int i = 0; i < this->childISRs.size(); ++i) {
        auto& child = this->childISRs[i];

        if (child->GetStartLocation() < nearestStart) {
            nearestStart = child->GetStartLocation();
            whichChildEarliest = i;
        }

        if (child->GetEndLocation() > nearestEnd) {
            nearestEnd = child->GetEndLocation();
            whichChildLatest = i;
        }
    }

    this->nearestTerm = whichChildEarliest;
    this->farthestTerm = whichChildLatest;
    this->nearestStartLocation = nearestStart;
    this->nearestEndLocation = nearestEnd;
    this->currentPostEntry = this->childISRs[nearestTerm]->GetCurrentPostEntry();
}

// helper function to check if all the current child ISRs
// are pointing to the same document AND are all consecutive
// occurrences in order
bool ISRPhrase::ChildrenFormPhrase() {
    uint32_t prevDocumentID;
    int prevOccurrence;
    bool first = true;

    for (auto& child : childISRs) {
        if (first) {
            prevDocumentID = child->GetDocumentID();
            prevOccurrence = child->GetStartLocation();
            first = false;
            continue;
        }

        if (child->GetDocumentID() != prevDocumentID) {
            return false;
        }

        if (child->GetStartLocation() != prevOccurrence + 1) {
            return false;
        }

        prevOccurrence = child->GetStartLocation();
    }

    return true;
}

bool ISRPhrase::CatchUpStragglerISRs() {
    return ISRPhrase::NewCatchUpStragglerISRs();
}

bool ISRPhrase::OldCatchUpStragglerISRs() {
    // child ISRs may not currently form a Phrase
    while (true) {
        // means that among child ISRs x, y, and z...
        // ..........x...............
        // .................y........
        // .........................z
        // move forward the proper stragglers until they're hopefully RIGHT NEXT TO z
        uint32_t potentialTargetDocument = (this->childISRs)[this->farthestTerm]->GetDocumentID();
        size_t baselineLocation = (this->childISRs)[this->farthestTerm]->GetStartLocation();

        for (int i = 0; i < childISRs.size(); ++i) {
            if (i == this->farthestTerm) {
                continue;
            }

            uint32_t currentDocument = (this->childISRs)[i]->GetDocumentID();
            size_t currLocation = (this->childISRs)[i]->GetStartLocation();

            // let's say there are child ISRs a, b, c, and d
            // c is the one that is the furthest up ahead
            // this means that a must be 2 locations behind c
            // b must be 1 location behind c
            // d must be 1 location ahead c
            int expectedLocationDifference = i - this->farthestTerm;

            if ((currentDocument == potentialTargetDocument) &&
                (currLocation - baselineLocation == expectedLocationDifference)) {
                continue;
            }

            // that means this child ISR passed none of the checks and needs to move
            if ((this->childISRs)[i]->Next() == std::nullopt) {
                // this child ISR reached the end of its line, thus impossible
                // to now have all ISRs form a phrase
                return false;
            }
        }

        this->UpdateMarkers();

        // now, after doing that, do all these child ISRs form a phrase?
        if (this->ChildrenFormPhrase()) {
            return true;
        }
        // otherwise, try again
    }
}

bool ISRPhrase::NewCatchUpStragglerISRs() {
    // child ISRs may not currently form a Phrase
    while (true) {
        // means that among child ISRs x, y, and z...
        // ..........x...............
        // .................y........
        // .........................z
        // move forward the proper stragglers until they're hopefully RIGHT NEXT TO z
        uint32_t potentialTargetDocument = (this->childISRs)[this->farthestTerm]->GetDocumentID();
        size_t baselineLocation = (this->childISRs)[this->farthestTerm]->GetStartLocation();

        for (int i = 0; i < childISRs.size(); ++i) {
            if (i == this->farthestTerm) {
                continue;
            }

            uint32_t currentDocument = (this->childISRs)[i]->GetDocumentID();
            size_t currLocation = (this->childISRs)[i]->GetStartLocation();

            // let's say there are child ISRs a, b, c, and d
            // c is the one that is the furthest up ahead
            // this means that a must be 2 locations behind c
            // b must be 1 location behind c
            // d must be 1 location ahead c
            int expectedLocationDifference = i - this->farthestTerm;

            if ((currentDocument == potentialTargetDocument) &&
                (currLocation - baselineLocation == expectedLocationDifference)) {
                continue;
            }

            // that means this child ISR passed none of the checks and needs to move
            if ((this->childISRs)[i]->Seek((int)baselineLocation + expectedLocationDifference) ==
                std::nullopt) {
                // this child ISR reached the end of its line, thus impossible
                // to now have all ISRs form a phrase
                return false;
            }
        }

        this->UpdateMarkers();

        // now, after doing that, do all these child ISRs form a phrase?
        if (this->ChildrenFormPhrase()) {
            return true;
        }
        // otherwise, try again
        // special case:
        // if they're NOT all on the same document but are somehow consecutive, just do a Next() on all of them
        bool somehow_consecutive = true;
        int prevOccurrence;
        bool first = true;

        for (auto& child : childISRs) {
            if (first) {
                prevOccurrence = child->GetStartLocation();
                first = false;
                continue;
            }

            if (child->GetStartLocation() != prevOccurrence + 1) {
                somehow_consecutive = false;
                break;
            }

            prevOccurrence = child->GetStartLocation();
        }

        if (somehow_consecutive) {
            for (int i = 0; i < this->childISRs.size(); ++i) {
                if (this->childISRs[i]->Next() == std::nullopt) {
                    return false;
                }
            }
        }
    }
}

std::optional<PostEntry> ISRPhrase::Next() {
    // doesn't matter if this ISRPhrase has been used before or not
    // need to do a Next() on all the child ISRs
    for (auto& child : childISRs) {
        if (child->Next() == std::nullopt) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // do all these child ISRs now form a phrase?
    if (this->ChildrenFormPhrase()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for a Phrase ISR?
        return this->currentPostEntry;
    }

    if (!this->CatchUpStragglerISRs()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }
    return this->currentPostEntry;
}

std::optional<PostEntry> ISRPhrase::NextDocument() {
    for (auto& child : childISRs) {
        if (child->NextDocument() == std::nullopt) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // are all these child ISRs forming a phrase?
    if (this->ChildrenFormPhrase()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for Phrase ISR?
        return this->currentPostEntry;
    }

    if (!this->CatchUpStragglerISRs()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }
    return this->currentPostEntry;
}

std::optional<PostEntry> ISRPhrase::Seek(size_t target) {
    for (auto& child : childISRs) {
        if (child->Seek(target) == std::nullopt) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    if (this->ChildrenFormPhrase()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for Phrase ISR?
        return this->currentPostEntry;
    }

    if (!this->CatchUpStragglerISRs()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }
    return this->currentPostEntry;
}