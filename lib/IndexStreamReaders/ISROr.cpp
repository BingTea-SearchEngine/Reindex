#include <cassert>

#include "ISROr.hpp"

ISROr::ISROr(std::vector<ISR*> children)
    : childISRs(children),
      whichChildFinished(children.size(), false),
      nearestTerm(children.size()),
      currentPostEntry(std::nullopt),
      nearestStartLocation(-1),
      nearestEndLocation(-1) {}

int ISROr::GetStartLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetStartLocation called when this ISR is not pointing to anything");
    return this->nearestStartLocation;
}

int ISROr::GetEndLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetEndLocation called when this ISR is not pointing to anything");
    return this->nearestEndLocation;
}

std::optional<PostEntry> ISROr::GetCurrentPostEntry() {
    return this->currentPostEntry;
}

std::string ISROr::GetDocumentName() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentName called when this ISR is not pointing to anything");
    return (this->childISRs)[this->nearestTerm]->GetDocumentName();
}

// helper function to update the internal marker variables
// when any of the ISRs get moved around
// PRECONDITION: this helper function must only be called when
//               NOT ALL of the children are finished
void ISROr::UpdateMarkers() {
    int whichChild;
    int nearestStart = INT32_MAX;
    int nearestEnd = 0;

    // figure out who is now the newest earliest occurrence and other variables
    // whilst making sure to ignore those that are already finished
    for (int i = 0; i < this->childISRs.size(); ++i) {
        if (this->whichChildFinished[i]) {
            continue;
        }
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
    this->currentPostEntry =
        this->childISRs[this->nearestTerm]->GetCurrentPostEntry();
}

// internal helper function to determine if
// all the child ISRs have finished
bool ISROr::AllChildrenFinished() {
    for (auto b : this->whichChildFinished) {
        if (!b) {
            return false;
        }
    }

    return true;
}

std::optional<PostEntry> ISROr::Next() {
    if (this->AllChildrenFinished()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }

    // check whether or not this ISROr
    // has ever been used before
    if (this->nearestStartLocation == -1) {
        // need to do a Next() on all the child ISRs to initialize them
        for (int i = 0; i < this->childISRs.size(); ++i) {
            if (this->childISRs[i]->Next() == std::nullopt) {
                this->whichChildFinished[i] = true;
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
            this->whichChildFinished[this->nearestTerm] = true;
        }
    }

    if (this->AllChildrenFinished()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }

    this->UpdateMarkers();
    return this->currentPostEntry;
}

std::optional<PostEntry> ISROr::NextDocument() {
    if (this->AllChildrenFinished()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }

    // check whether or not this ISROr
    // has ever been used before
    if (this->nearestStartLocation == -1) {
        // need to do a NextDocument() on all the child ISRs to initialize them
        for (int i = 0; i < this->childISRs.size(); ++i) {
            if (this->childISRs[i]->NextDocument() == std::nullopt) {
                this->whichChildFinished[i] = true;
            }
        }

        if (this->AllChildrenFinished()) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }

        this->UpdateMarkers();
        return this->currentPostEntry;
    }

    std::string prevDocumentName = this->GetDocumentName();

    while (this->GetDocumentName() == prevDocumentName) {
        if (this->childISRs[this->nearestTerm]->NextDocument() ==
            std::nullopt) {
            this->whichChildFinished[this->nearestTerm] = true;
        }

        if (this->AllChildrenFinished()) {
            this->currentPostEntry = std::nullopt;
            return std::nullopt;
        }

        this->UpdateMarkers();
    }

    return this->currentPostEntry;
}

std::optional<PostEntry> ISROr::Seek(size_t target) {
    // reset the whichChildFinished bool container to allow arbitrary seeking at any point
    for (int i = 0; i < this->whichChildFinished.size(); ++i) {
        this->whichChildFinished[i] = false;
    }

    // Seek all the ISRs to the first occurrence beginning at
    // the target location. Return null if there is no match.
    // The document is the document containing the nearest term.
    for (int i = 0; i < this->childISRs.size(); ++i) {
        assert(i >= 0 && i < this->childISRs.size());
        if (this->childISRs[i]->Seek(target) == std::nullopt) {
            assert(i >= 0 && i < this->whichChildFinished.size());
            this->whichChildFinished[i] = true;
        }
    }

    if (this->AllChildrenFinished()) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }

    this->UpdateMarkers();
    return this->currentPostEntry;
}