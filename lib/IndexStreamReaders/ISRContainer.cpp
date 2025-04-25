#include <cassert>

#include "ISRContainer.hpp"

ISRContainer::ISRContainer(ISR* includedISR, ISR* excludedISR)
    : included(includedISR), excluded(excludedISR) {
    excluded->NextDocument();
}

ISRContainer::~ISRContainer() {
    delete included;
}

int ISRContainer::GetStartLocation() {
    assert(this->included->GetCurrentPostEntry().has_value() &&
           "GetStartLocation called when this ISR is not pointing to anything");
    return this->included->GetStartLocation();
}

int ISRContainer::GetEndLocation() {
    assert(this->included->GetCurrentPostEntry().has_value() &&
           "GetEndLocation called when this ISR is not pointing to anything");
    return this->included->GetEndLocation();
}

std::optional<PostEntry> ISRContainer::GetCurrentPostEntry() {
    return this->included->GetCurrentPostEntry();
}

uint32_t ISRContainer::GetDocumentID() {
    assert(this->included->GetCurrentPostEntry().has_value() &&
           "GetDocumentID called when this ISR is not pointing to anything");
    return this->included->GetDocumentID();
}

// INTERNAL HELPER FUNCTION
// precondition: at the very beginning of calling this,
// the included and excluded must both be pointing to something meaningful
// this function finds the next match for Included while making sure to
// ignore documents dictated by Excluded
std::optional<PostEntry> ISRContainer::MatchNotOnExcluded() {
    while (true) {
        uint32_t includedDocID = this->included->GetDocumentID();
        uint32_t excludedDocID = this->excluded->GetDocumentID();

        if (includedDocID < excludedDocID) {
            return this->included->GetCurrentPostEntry();
        } else if (includedDocID == excludedDocID) {
            // try again
            this->included->NextDocument();
            this->excluded->NextDocument();
        } else {
            // move forward excluded ISR
            this->excluded->NextDocument();
        }

        if (this->included->GetCurrentPostEntry() == std::nullopt) {
            return std::nullopt;
        }

        if (this->excluded->GetCurrentPostEntry() == std::nullopt) {
            return this->included->GetCurrentPostEntry();
        }
    }
}

std::optional<PostEntry> ISRContainer::Next() {
    if (this->included->Next() == std::nullopt) {
        return std::nullopt;
    }

    if (this->excluded->GetCurrentPostEntry() == std::nullopt) {
        return this->included->GetCurrentPostEntry();
    }

    return this->MatchNotOnExcluded();
}

std::optional<PostEntry> ISRContainer::NextDocument() {
    if (this->included->NextDocument() == std::nullopt) {
        return std::nullopt;
    }

    if (this->excluded->GetCurrentPostEntry() == std::nullopt) {
        return this->included->GetCurrentPostEntry();
    }

    return this->MatchNotOnExcluded();
}

std::optional<PostEntry> ISRContainer::Seek(size_t target) {
    if (this->included->Seek(target) == std::nullopt) {
        return std::nullopt;
    }

    this->excluded->Seek(0);

    if (this->excluded->GetCurrentPostEntry() == std::nullopt) {
        return this->included->GetCurrentPostEntry();
    }

    return this->MatchNotOnExcluded();
}
