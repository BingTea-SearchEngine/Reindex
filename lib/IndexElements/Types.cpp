#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "Types.hpp"
#include "cstring"

bool operator==(const postentry_t& lhs, const postentry_t& rhs) {
    return lhs.offset == rhs.offset &&
           lhs.location == rhs.location;
}

void postentry_t::Serialize(char* base_region, size_t &offset, const postentry_t &word_occurrence) {
    /*
        word_t object has:
            - uint32_t offset
            - enum int location
    */
    spdlog::info("offset variable is currently at {}", offset);

    std::memcpy(base_region + offset, &word_occurrence.offset, sizeof(word_occurrence.offset));
    offset += sizeof(word_occurrence.offset);
    spdlog::info("After writing the uint32_t offset, offset is now at {}", offset);

    std::memcpy(base_region + offset, &word_occurrence.location, sizeof(word_occurrence.location));
    offset += sizeof(word_occurrence.location);
    spdlog::info("After writing the enum int location, offset is now at {}", offset);

    spdlog::info("Serializing this postentry_t complete");
    spdlog::info("offset variable is now at {}", offset);
}

postentry_t postentry_t::Deserialize(char* base_region, size_t &offset) {
    spdlog::info("offset variable is currently at {}", offset);

    postentry_t word;

    std::memcpy(&word.offset, base_region + offset, sizeof(word.offset));
    offset += sizeof(word.offset);
    spdlog::info("After reading the uint32_t offset, offset is now at {}", offset);

    std::memcpy(&word.location, base_region + offset, sizeof(word.location));
    offset += sizeof(word.location);
    spdlog::info("After reading the enum int location, offset is now at {}", offset);

    spdlog::info("Deserializing a postentry_t complete");
    spdlog::info("Offset is now at {}", offset);

    return word;
}