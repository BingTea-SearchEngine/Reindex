#include "ISRAnd.hpp"

ISRAnd::ISRAnd(std::vector<ISR*> children) : childISRs(children),
                                             nearestTerm(-1), farthestTerm(-1),
                                             nearestStartLocation(-1), nearestEndLocation(-1) {}

int ISRAnd::GetStartLocation() {
    return this->childISRs[nearestTerm]->GetStartLocation();
}

int ISRAnd::GetEndLocation() {
    return this->childISRs[farthestTerm]->GetEndLocation();
}

std::optional<PostEntry> ISRAnd::GetCurrentPostEntry() {
    return this->childISRs[nearestTerm]->GetCurrentPostEntry();
}

std::string ISRAnd::GetDocumentName() {
    return this->childISRs[nearestTerm]->GetDocumentName();
}

// helper function to update the internal marker variables
// when any of the ISRs get moved around
void ISRAnd::UpdateMarkers() {
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
// are pointing to the same document
bool ISRAnd::ChildrenOnSameDocument() {
    std::string prevDocumentName = "";

    for (auto& child : childISRs) {
        if (prevDocumentName == "") {
            prevDocumentName = child->GetDocumentName();
            continue;
        }

        if (child->GetDocumentName() != prevDocumentName) {
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
    // not all pointing to the same document
    while (true) {
        // means that among child ISRs x, y, and z...
        // ..........x...............
        // ..............y...........
        // .....................z....
        // move forward the proper stragglers until they're hopefully on the same document as z
        std::string potentialTargetDocument = (this->childISRs)[this->farthestTerm]->GetDocumentName();
        for (int i = 0; i < childISRs.size(); ++i) {
            if (i == this->farthestTerm) {
                continue;
            }

            std::string currentDocument = (this->childISRs)[i]->GetDocumentName();
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

std::optional<PostEntry> ISRAnd::Next() {
    // check whether or not this ISROr
    // has ever been used before
    if (nearestStartLocation == -1) {
        // need to do a Next() on all the child ISRs to initialize them
        for (auto& child : childISRs) {
            if (child->Next() == std::nullopt) {
                return std::nullopt;
            }
        }
    } else {
        // these ISRs have already been used before and should be pointing at the same document
        // advance the nearest ISR and look for the first match

        // this->nearestTerm points to the childISR that is the earliest
        if (this->childISRs[this->nearestTerm]->Next() == std::nullopt) {
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // are all these child ISRs now pointing to the same document?
    if (this->ChildrenOnSameDocument()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for an AND ISR?
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }

    if (!this->CatchUpStragglerISRs()) {
        return std::nullopt;
    }
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::optional<PostEntry> ISRAnd::NextDocument() {
    for (auto& child : childISRs) {
        if (child->NextDocument() == std::nullopt) {
            return std::nullopt;
        }
    }

    this->UpdateMarkers();

    // are all these child ISRs now pointing to the same document?
    if (this->ChildrenOnSameDocument()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for an AND ISR?
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }

    if (!this->CatchUpStragglerISRs()) {
        return std::nullopt;
    }
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::optional<PostEntry> ISRAnd::Seek(size_t target) {
    for (auto& child : childISRs) {
        if (child->Seek(target) == std::nullopt) {
            return std::nullopt;
        }
    }

    if (this->ChildrenOnSameDocument()) {
        // then, we can return if that is the case
        // TODO: returning the first PostEntry for an AND ISR?
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }

    if (!this->CatchUpStragglerISRs()) {
        return std::nullopt;
    }
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}