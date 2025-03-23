#include <spdlog/spdlog.h>

#include "PostEntry.hpp"

PostEntry::PostEntry() {}

PostEntry::PostEntry(uint32_t delta, wordlocation_t location_found) : delta(delta), location_found(location_found) {}

// bool operator==(const PostEntry& lhs, const PostEntry& rhs)
// {
//     return (lhs.delta == rhs.delta) && (lhs.location_found == rhs.location_found);
// }

std::ostream& operator<<(std::ostream& os, const PostEntry& obj)
{
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

uint32_t PostEntry::getDelta()
{
    return delta;
}

wordlocation_t PostEntry::getLocationFound()
{
    return location_found;
}

void PostEntry::Serialize(char* base_region, size_t &offset, const PostEntry &word_occurrence)
{
    spdlog::info("Trying to serialize a PostEntry");
    spdlog::info("Offset variable is currently at {}", offset);

    std::memcpy(base_region + offset, &word_occurrence.delta, sizeof(word_occurrence.delta));
    offset += sizeof(word_occurrence.delta);
    spdlog::info("After writing the uint32_t delta, offset is now at {}", offset);

    std::memcpy(base_region + offset, &word_occurrence.location_found, sizeof(word_occurrence.location_found));
    offset += sizeof(word_occurrence.location_found);
    spdlog::info("After writing the enum int location_found, offset is now at {}", offset);

    spdlog::info("Serializing this PostEntry complete");
    spdlog::info("Offset variable is now at {}", offset);
}

PostEntry PostEntry::Deserialize(char* base_region, size_t &offset)
{
    spdlog::info("Trying to deserialize a PostEntry");
    spdlog::info("Offset variable is currently at {}", offset);

    PostEntry word_occurrence;

    std::memcpy(&word_occurrence.delta, base_region + offset, sizeof(word_occurrence.delta));
    offset += sizeof(word_occurrence.delta);
    spdlog::info("After reading the uint32_t delta, offset is now at {}", offset);

    std::memcpy(&word_occurrence.location_found, base_region + offset, sizeof(word_occurrence.location_found));
    offset += sizeof(word_occurrence.location_found);
    spdlog::info("After reading the enum int location_found, offset is now at {}", offset);

    spdlog::info("Deserializing a PostEntry complete");
    spdlog::info("Offset is now at {}", offset);

    return word_occurrence;
}
