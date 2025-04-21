#include <cassert>

#include "ISRContainer.hpp"

ISRContainer::ISRContainer(ISR* includedISR, ISR* excludedISR)
    : included(includedISR), currentPostEntry(std::nullopt) {
    excludedISR->NextDocument();
    while (excludedISR->GetCurrentPostEntry() != std::nullopt) {
        excludedDocuments.insert(excludedISR->GetDocumentID());
        excludedISR->NextDocument();
    }
    delete excludedISR;
}

ISRContainer::~ISRContainer(){
    delete included;
}

int ISRContainer::GetStartLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetStartLocation called when this ISR is not pointing to anything");
    return this->included->GetStartLocation();
}

int ISRContainer::GetEndLocation() {
    assert(this->currentPostEntry.has_value() &&
           "GetEndLocation called when this ISR is not pointing to anything");
    return this->included->GetEndLocation();
}

std::optional<PostEntry> ISRContainer::GetCurrentPostEntry() {
    return this->included->GetCurrentPostEntry();
}

uint32_t ISRContainer::GetDocumentID() {
    assert(this->currentPostEntry.has_value() &&
           "GetDocumentID called when this ISR is not pointing to anything");
    return this->included->GetDocumentID();
}

std::optional<PostEntry> ISRContainer::Next() {
    while (this->included->Next() != std::nullopt) {
        this->currentPostEntry = this->included->GetCurrentPostEntry();
        if (this->excludedDocuments.find(this->included->GetDocumentID()) ==
            this->excludedDocuments.end()) {
            return this->currentPostEntry;
        }
    }

    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRContainer::NextDocument() {
    while (this->included->NextDocument() != std::nullopt) {
        this->currentPostEntry = this->included->GetCurrentPostEntry();
        if (this->excludedDocuments.find(this->included->GetDocumentID()) ==
            this->excludedDocuments.end()) {
            return this->currentPostEntry;
        }
    }

    this->currentPostEntry = std::nullopt;
    return std::nullopt;
}

std::optional<PostEntry> ISRContainer::Seek(size_t target) {
    if (this->included->Seek(target) == std::nullopt) {
        this->currentPostEntry = std::nullopt;
        return std::nullopt;
    }

    this->currentPostEntry = this->included->GetCurrentPostEntry();

    if (this->excludedDocuments.find(this->included->GetDocumentID()) ==
        this->excludedDocuments.end()) {
        return this->currentPostEntry;
    }

    return this->Next();
}
