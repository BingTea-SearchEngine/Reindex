#include "ISROr.hpp"

ISROr::ISROr(std::vector<ISR*> children) : childISRs(children), nearestTerm(children.size()),
                                           nearestStartLocation(-1), nearestEndLocation(-1) {}

int ISROr::GetStartLocation() {
    return this->nearestStartLocation;
}

int ISROr::GetEndLocation() {
    return this->nearestEndLocation;
}

std::optional<PostEntry> ISROr::GetCurrentPostEntry() {
    if (nearestTerm >= childISRs.size()) { // constructor hacky way of signifying this ISR has never been used before
        return std::nullopt;
    }

    // extract whatever the earliest child ISR is pointing at
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::string ISROr::GetDocumentName() {
    if (nearestTerm >= childISRs.size()) { // constructor hacky way of signifying this ISR has never been used before
        return "";
    }

    // extract the name of the document that the earliest child ISR is pointing at
    return (this->childISRs)[this->nearestTerm]->GetDocumentName();
}

// helper function to update the internal marker variables
// when any of the ISRs get moved around
void ISROr::UpdateMarkers() {
    int whichChild;
    int nearestStart = INT32_MAX;
    int nearestEnd = 0;

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

std::optional<PostEntry> ISROr::Next() {
    if (childISRs.size() == 0) {
        return std::nullopt;
    }

    // check whether or not this ISROr
    // has ever been used before
    if (this->nearestStartLocation == -1) {
        // need to do a Next() on all the child ISRs to initialize them
        for (int i = childISRs.size() - 1; i >= 0; --i) {
            if (childISRs[i]->Next() == std::nullopt) {
                childISRs.erase(childISRs.begin() + i);
            }
        }
    } else {
        // this ISROr has been used before
        // so its child ISRs are guaranteed
        // to have been modified

        // Among child ISRs x, y, and z,
        // if x is at the earliest location,
        // do a Next() on x, and then return
        // the new nearest/earliest match.

        // this->nearestTerm points to the childISR that is the earliest
        if (this->childISRs[this->nearestTerm]->Next() == std::nullopt) {
            childISRs.erase(childISRs.begin() + this->nearestTerm);
        }
    }

    if (childISRs.size() == 0) {
        return std::nullopt;
    }

    this->UpdateMarkers();
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::optional<PostEntry> ISROr::NextDocument() {
    // Position all the ISRs to the first occurrence
    // immediately after the current document.
    for (auto child : this->childISRs) {
        // need to double check logic on this
        // I'm not actually sure if this works?
        // can't think of a case that disproves
        // but can't 100% say this recursion is correct
        if (child->NextDocument() == std::nullopt) { // TODO: this is wrong logic!
            return std::nullopt;
        }
    }
    this->UpdateMarkers();
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}

std::optional<PostEntry> ISROr::Seek(size_t target) {
    // Seek all the ISRs to the first occurrence beginning at
    // the target location. Return null if there is no match.
    // The document is the document containing the nearest term.
    for (auto child : this->childISRs) {
        if (child->Seek(target) == std::nullopt) { // TODO: this is wrong logic!
            return std::nullopt;
        }
    }
    this->UpdateMarkers();
    return (this->childISRs)[this->nearestTerm]->GetCurrentPostEntry();
}