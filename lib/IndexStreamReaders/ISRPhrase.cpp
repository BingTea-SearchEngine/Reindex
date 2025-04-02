#include "ISRPhrase.hpp"

ISRPhrase::ISRPhrase(std::vector<ISR*> children) : childISRs(children) {}

int ISRPhrase::GetStartLocation() {
    return this->childISRs[nearestTerm]->GetStartLocation();
}

int ISRPhrase::GetEndLocation() {
    return this->childISRs[farthestTerm]->GetEndLocation();
}

std::optional<PostEntry> ISRPhrase::GetCurrentPostEntry() {
    return this->childISRs[nearestTerm]->GetCurrentPostEntry();
}

std::string ISRPhrase::GetDocumentName() {
    return this->childISRs[nearestTerm]->GetDocumentName();
}

void ISRPhrase::UpdateMarkers() {
    size_t whichChildEarliest;
    size_t whichChildLatest;
    size_t nearestStart = SIZE_MAX;
    size_t nearestEnd = 0;

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

    // whichChild is now the value of whatever child is at the earliest location
    this->nearestTerm = whichChildEarliest;
    this->farthestTerm = whichChildLatest;
    this->nearestStartLocation = nearestStart;
    this->nearestEndLocation = nearestEnd;
}

// helper function to check if all the current child ISRs
// are pointing to the same document AND are all consecutive
// occurrences in order
bool ISRPhrase::ChildrenFormPhrase() {
    std::string prevDocumentName;
    size_t prevOccurrence;
    bool first = true;

    for (auto& child : childISRs) {
        if (first) {
            prevDocumentName = child->GetDocumentName();
            prevOccurrence = child->GetStartLocation();
            first = false;
            continue;
        }

        if (child->GetDocumentName() != prevDocumentName) {
            return false;
        }

        if (child->GetStartLocation() != prevOccurrence + 1) {
            return false;
        }
    }

    return true;
}

bool ISRPhrase::CatchUpStragglerISRs() {
    // child ISRs may not currently form a Phrase
    while (true) {
        // means that among child ISRs x, y, and z...
        // ..........x...............
        // .................y........
        // ....z.....................
        // move forward the proper stragglers until they're hopefully RIGHT NEXT TO y
        std::string potentialTargetDocument = (this->childISRs)[this->farthestTerm]->GetDocumentName();
        size_t baselineLocation = (this->childISRs)[this->farthestTerm]->GetStartLocation();
        for (int i = 0; i < childISRs.size(); ++i) {
            if (i == this->farthestTerm) {
                continue;
            }

            std::string currentDocument = (this->childISRs)[i]->GetDocumentName();
            if (currentDocument == potentialTargetDocument) {
                continue;
            }

            size_t currLocation = (this->childISRs)[i]->GetStartLocation();
            // let's say there are child ISRs a, b, c, and d
            // c is the one that is the furthest up ahead
            // this means that a must be 2 locations behind c
            // b must be 1 location behind c
            // d must be 1 location ahead c
            int expectedLocationDifference = i - this->farthestTerm;

            if (currLocation - baselineLocation == expectedLocationDifference) {
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

std::optional<PostEntry> ISRPhrase::Next() {
    // doesn't matter if this ISRPhrase has been used before or not
    // need to do a Next() on all the child ISRs
    for (auto& child : childISRs) {
        if (child->Next() == std::nullopt) {
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // do all these child ISRs now form a phrase?
    if (this->ChildrenFormPhrase()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for a Phrase ISR?
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }

    if (!this->CatchUpStragglerISRs()) {
        return std::nullopt;
    }
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::optional<PostEntry> ISRPhrase::NextDocument() {
    for (auto& child : childISRs) {
        if (child->NextDocument() == std::nullopt) {
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // are all these child ISRs forming a phrase?
    if (this->ChildrenFormPhrase()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for Phrase ISR?
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }

    if (!this->CatchUpStragglerISRs()) {
        return std::nullopt;
    }
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::optional<PostEntry> ISRPhrase::Seek(size_t target) {
    for (auto& child : childISRs) {
        if (child->Seek(target) == std::nullopt) {
            return std::nullopt;
        }
    }

    if (this->ChildrenFormPhrase()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for Phrase ISR?
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }

    if (!this->CatchUpStragglerISRs()) {
        return std::nullopt;
    }
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}