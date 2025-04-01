#include "ISROr.hpp"

ISROr::ISROr(std::vector<ISR*> children) : childISRs(children), nearestTerm(children.size()),
                                           nearestStartLocation(-1), nearestEndLocation(-1) {}

size_t ISROr::GetStartLocation() {
    return this->nearestStartLocation;
}

size_t ISROr::GetEndLocation() {
    return this->nearestEndLocation;
}

PostEntry* ISROr::Next() {
    // check whether or not this ISROr
    // has ever been used before
    if (nearestStartLocation == -1) {
        size_t whichChild;
        size_t nearestStart = SIZE_MAX;
        size_t nearestEnd = 0;

        // need to do a Next() on all the child ISRs to initialize them
        for (int i = 0; i < this->childISRs.size(); ++i) {
            auto& child = this->childISRs[i];
            child->Next();

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
        return (this->childISRs)[whichChild]->GetCurrentPost();
    } else {
        // this ISROr has been used before
        // so its child ISRs are guaranteed
        // to have been modified

        // Among child ISRs x, y, and z,
        // if x is at the earliest location,
        // do a Next() on x, and then return
        // the new nearest/earliest match.

        // this->nearestTerm points to the childISR that is the earliest
        this->childISRs[this->nearestTerm]->Next();

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
        return (this->childISRs)[whichChild]->GetCurrentPost();
    }
}