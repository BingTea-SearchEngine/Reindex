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

size_t ISRContainer::GetDocumentStart() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentStart called when this ISR is not pointing to anything");
    return this->included->GetDocumentStart();
}

std::optional<PostEntry> ISRContainer::Next() {
    while (this->included->Next() != std::nullopt) {
        this->currentPostEntry = this->included->GetCurrentPostEntry();
        if (this->excludedDocuments.find(this->included->GetDocumentID()) ==
            this->excludedDocuments.end()) {
            return this->currentPostEntry;
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
