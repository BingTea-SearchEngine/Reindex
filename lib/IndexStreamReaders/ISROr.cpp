#include "ISROr.hpp"

ISROr::ISROr(std::vector<ISR*> children) : childISRs(children), nearestTerm(children.size()),
                                           nearestStartLocation(-1), nearestEndLocation(-1) {}

size_t ISROr::GetStartLocation() {
    return this->nearestStartLocation;
}

size_t ISROr::GetEndLocation() {
    return this->nearestEndLocation;
}

PostEntry* ISROr::GetCurrentPostEntry() {
    // extract whatever the earliest child ISR is pointing at
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::string ISROr::GetDocumentName() {
    // extract the name of the document that the earliest child ISR is pointing at
    return (this->childISRs)[this->nearestTerm]->GetDocumentName();
}

// helper function to update the internal marker variables
// when any of the ISRs get moved around
void ISROr::UpdateMarkers() {
    size_t whichChild;
    size_t nearestStart = SIZE_MAX;
    size_t nearestEnd = 0;

    // figure out who is now the newest earliest occurrence and other variables
    for (int i = 0; i < this->childISRs.size(); ++i) {
        auto& child = this->childISRs[i];

        if (child->GetStartLocation() < nearestStart) {
            nearestStart = child->GetStartLocation();
            whichChild = i;
        }

        if (child->GetEndLocation() > nearestEnd) {
            nearestEnd = child->GetEndLocation();
        }
    }

    // whichChild is now the value of whatever child is at the earliest location
    this->nearestTerm = whichChild;
    this->nearestStartLocation = nearestStart;
    this->nearestEndLocation = nearestEnd;
}

PostEntry* ISROr::Next() {
    // check whether or not this ISROr
    // has ever been used before
    if (nearestStartLocation == -1) {
        // need to do a Next() on all the child ISRs to initialize them
        for (auto& child : childISRs) {
            if (child->Next() == nullptr) {
                return nullptr;
            }
        }

        this->UpdateMarkers();

        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    } else {
        // this ISROr has been used before
        // so its child ISRs are guaranteed
        // to have been modified

        // Among child ISRs x, y, and z,
        // if x is at the earliest location,
        // do a Next() on x, and then return
        // the new nearest/earliest match.

        // this->nearestTerm points to the childISR that is the earliest
        if (this->childISRs[this->nearestTerm]->Next() == nullptr) {
            return nullptr;
        }
        this->UpdateMarkers();
        return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
    }
}

PostEntry* ISROr::NextDocument() {
    // Position all the ISRs to the first occurrence
    // immediately after the current document.
    for (auto& child : this->childISRs) {
        // need to double check logic on this
        // I'm not actually sure if this works?
        // can't think of a case that disproves
        // but can't 100% say this recursion is correct
        if (child->NextDocument() == nullptr) {
            return nullptr;
        }
    }
    this->UpdateMarkers();
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

PostEntry* ISROr::Seek(size_t target) {
    // Seek all the ISRs to the first occurrence beginning at
    // the target location. Return null if there is no match.
    // The document is the document containing the nearest term.
    for (auto& child : this->childISRs) {
        if (child->Seek(target) == nullptr) {
            return nullptr;
        }
    }
    this->UpdateMarkers();
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}