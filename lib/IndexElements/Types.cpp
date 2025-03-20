#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "Types.hpp"
#include "cstring"

bool operator==(const word_t& lhs, const word_t& rhs) {
    return lhs.word == rhs.word &&
           lhs.offset == rhs.offset &&
           lhs.location == rhs.location;
}

void word_t::Serialize(char* base_region, size_t &offset, const word_t &word_occurrence) {
    /*
        word_t object has:
            - std::string word
            - uint32_t offset
            - enum int location
    */

    spdlog::info("Attempting to serialize {} at location {} + {} = {}", word_occurrence, base_region, offset, base_region + offset);
    spdlog::info("offset variable is currently at {}", offset);

    size_t word_size = word_occurrence.word.size() + 1; // account for null terminator
    std::memcpy(base_region + offset, word_occurrence.word.c_str(), word_size);
    offset += word_size;
    spdlog::info("After writing the std::string word, offset is now at {}", offset);

    std::memcpy(base_region + offset, &word_occurrence.offset, sizeof(word_occurrence.offset));
    offset += sizeof(word_occurrence.offset);
    spdlog::info("After writing the uint32_t offset, offset is now at {}", offset);

    std::memcpy(base_region + offset, &word_occurrence.location, sizeof(word_occurrence.location));
    offset += sizeof(word_occurrence.location);
    spdlog::info("After writing the enum int location, offset is now at {}", offset);

    spdlog::info("Serializing {} complete", word_occurrence);
    spdlog::info("offset variable is now at {}", offset);
}

word_t word_t::Deserialize(char* base_region, size_t &offset) {
    spdlog::info("Attempting to deserialize a word_t at location {} + {} = {}", base_region, offset, base_region + offset);
    spdlog::info("offset variable is currently at {}", offset);

    word_t word;

    word.word = std::string(base_region + offset);
    offset += word.word.size() + 1;
    spdlog::info("After reading the std::string word, offset is now at {}", offset);

    std::memcpy(&word.offset, base_region + offset, sizeof(word.offset));
    offset += sizeof(word.offset);
    spdlog::info("After reading the uint32_t offset, offset is now at {}", offset);

    std::memcpy(&word.location, base_region + offset, sizeof(word.location));
    offset += sizeof(word.location);
    spdlog::info("After reading the enum int location, offset is now at {}", offset);

    spdlog::info("Deserializing a word_t complete");
    spdlog::info("Offset is now at {}", offset);

    return word;
}