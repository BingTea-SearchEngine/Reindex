#include <spdlog/spdlog.h>

#include "PostEntry.hpp"

PostEntry::PostEntry() {}

PostEntry::PostEntry(uint32_t delta, wordlocation_t location_found)
    : delta(delta), location_found(location_found) {}

std::ostream& operator<<(std::ostream& os, const PostEntry& obj) {
    os << "PostEntry{ delta=" << obj.delta << " location_found=";
    switch (obj.location_found) {
        case (wordlocation_t::title):
            os << "title";
            break;
        case (wordlocation_t::bold):
            os << "bold";
            break;
        case (wordlocation_t::body):
            os << "body";
            break;
        default:
            os << "unknown";
    }
    os << " }";
    return os;
}

uint32_t PostEntry::GetDelta() const {
    return delta;
}

wordlocation_t PostEntry::GetLocationFound() const {
    return location_found;
}

size_t PostEntry::GetBytesRequired() {
    return sizeof(PostEntry);
}

void PostEntry::Print() const {
    cout << delta << " " << location_found;
}

void PostEntry::Serialize(char* base_region, size_t& offset, const PostEntry& word_occurrence) {
    std::memcpy(base_region + offset, &word_occurrence.delta, sizeof(word_occurrence.delta));
    offset += sizeof(word_occurrence.delta);

    std::memcpy(base_region + offset, &word_occurrence.location_found,
                sizeof(word_occurrence.location_found));
    offset += sizeof(word_occurrence.location_found);
}

PostEntry PostEntry::Deserialize(char* base_region, size_t& offset) {
    PostEntry word_occurrence;

    std::memcpy(&word_occurrence.delta, base_region + offset, sizeof(word_occurrence.delta));
    offset += sizeof(word_occurrence.delta);

    std::memcpy(&word_occurrence.location_found, base_region + offset,
                sizeof(word_occurrence.location_found));
    offset += sizeof(word_occurrence.location_found);

    return word_occurrence;
}
