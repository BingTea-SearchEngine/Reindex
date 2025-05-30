#include <cassert>

#include "ISRAnd.hpp"

ISRAnd::ISRAnd(std::vector<ISR*> children)
    : childISRs(children),
      currentPostEntry(std::nullopt),
      nearestTerm(-1),
      farthestTerm(-1),
      nearestStartLocation(-1),
      nearestEndLocation(-1) {}

ISRAnd::~ISRAnd() {
    for (auto child : childISRs) {
        delete child;
    }
}

int ISRAnd::GetStartLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetStartLocation called when this ISR is not pointing to anything");
    return this->nearestStartLocation;
}

int ISRAnd::GetEndLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetEndLocation called when this ISR is not pointing to anything");
    return this->nearestEndLocation;
}

std::optional<PostEntry> ISRAnd::GetCurrentPostEntry() {
    return this->currentPostEntry;
}

uint32_t ISRAnd::GetDocumentID() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentID called when this ISR is not pointing to anything");
    return this->childISRs[nearestTerm]->GetDocumentID();
}

size_t ISRAnd::GetDocumentStart() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentStart called when this ISR is not pointing to anything");
    return this->childISRs[nearestTerm]->GetDocumentStart();
}

// helper function to update the internal marker variables
// when any of the ISRs get moved around
// PRECONDITION: when calling this helper function,
//               not even a single one of the children
//               can be finished
void ISRAnd::UpdateMarkers() {
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
// are pointing to the same document
bool ISRAnd::ChildrenOnSameDocument() {
    uint32_t prevDocumentID = 0;
    bool first = true;

    for (auto& child : childISRs) {
        if (first) {
            prevDocumentID = child->GetDocumentID();
            first = false;
            continue;
        }

        if (child->GetDocumentID() != prevDocumentID) {
            return false;
        }
    }

    return true;
}

// helper function used to catch up the child ISRs that
// are behind another child ISR
// by the end of this function, all the child ISRs will be
// pointing to the same document
// returns True or False for successful or unsuccessful
bool ISRAnd::CatchUpStragglerISRs() {
    return ISRAnd::NewCatchUpStragglerISRs();
}

bool ISRAnd::OldCatchUpStragglerISRs() {
    // not all pointing to the same document
    while (true) {
        // means that among child ISRs x, y, and z...
        // ..........x...............
        // .........................y
        // .....................z....
        // move forward the proper stragglers until they're hopefully on the same document as y
        uint32_t potentialTargetDocument = (this->childISRs)[this->farthestTerm]->GetDocumentID();
        for (int i = 0; i < childISRs.size(); ++i) {
            if (i == this->farthestTerm) {
                continue;
            }

            uint32_t currentDocument = (this->childISRs)[i]->GetDocumentID();
            if (currentDocument == potentialTargetDocument) {
                continue;
            }

            // that means this child ISR is behind on the wrong doc and needs to catch up
            if ((this->childISRs)[i]->Next() == std::nullopt) {
                // this child ISR reached the end of its line, thus impossible
                // to now have all ISRs pointing to the same document
                return false;
            }
        }

        this->UpdateMarkers();

        // now, after doing that, are these child ISRs all pointing to the same document?
        if (this->ChildrenOnSameDocument()) {
            return true;
        }
        // otherwise, try again
    }
}

bool ISRAnd::NewCatchUpStragglerISRs() {
    // not all pointing to the same document
    while (true) {
        // means that among child ISRs x, y, and z...
        // ..........x...............
        // .........................y
        // .....................z....
        // move forward the proper stragglers until they're hopefully on the same document as y
        uint32_t potentialTargetDocument = (this->childISRs)[this->farthestTerm]->GetDocumentID();
        size_t beginningOfTargetDocument =
            (this->childISRs)[this->farthestTerm]->GetDocumentStart();

        for (int i = 0; i < childISRs.size(); ++i) {
            if (i == this->farthestTerm) {
                continue;
            }

            uint32_t currentDocument = (this->childISRs)[i]->GetDocumentID();
            if (currentDocument == potentialTargetDocument) {
                continue;
            }

            // that means this child ISR is behind on the wrong doc and needs to catch up
            if ((this->childISRs)[i]->Seek(beginningOfTargetDocument) == std::nullopt) {
                // this child ISR reached the end of its line, thus impossible
                // to now have all ISRs pointing to the same document
                return false;
            }
        }

        this->UpdateMarkers();

        // now, after doing that, are these child ISRs all pointing to the same document?
        if (this->ChildrenOnSameDocument()) {
            return true;
        }
        // otherwise, try again
    }
}

std::optional<PostEntry> ISRAnd::Next() {
    // check whether or not this ISROr
    // has ever been used before
    if (nearestStartLocation == -1) {
        // need to do a Next() on all the child ISRs to initialize them
        for (auto& child : childISRs) {
            if (child->Next() == std::nullopt) {
                this->currentPostEntry = std::nullopt;
                return std::nullopt;
            }
        }
    } else {
        // these ISRs have already been used before and should be pointing at the same document
        // advance the nearest ISR and look for the first match

        // this->nearestTerm points to the childISR that is the earliest
        if (this->childISRs[this->nearestTerm]->Next() == std::nullopt) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // are all these child ISRs now pointing to the same document?
    if (this->ChildrenOnSameDocument()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for an AND ISR?
        return this->currentPostEntry;
    }

    if (!this->CatchUpStragglerISRs()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }
    return this->currentPostEntry;
}

std::optional<PostEntry> ISRAnd::NextDocument() {
    for (auto& child : childISRs) {
        if (child->NextDocument() == std::nullopt) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // are all these child ISRs now pointing to the same document?
    if (this->ChildrenOnSameDocument()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for an AND ISR?
        return this->currentPostEntry;
    }

    if (!this->CatchUpStragglerISRs()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }
    return this->currentPostEntry;
}

std::optional<PostEntry> ISRAnd::Seek(size_t target) {
    for (auto& child : childISRs) {
        if (child->Seek(target) == std::nullopt) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    if (this->ChildrenOnSameDocument()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for an AND ISR?
        return this->currentPostEntry;
    }

    if (!this->CatchUpStragglerISRs()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }
    return this->currentPostEntry;
}