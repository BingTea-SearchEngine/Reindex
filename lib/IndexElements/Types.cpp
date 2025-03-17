#include "Types.hpp"

bool operator==(const word_t& lhs, const word_t& rhs) {
    return lhs.word == rhs.word &&
           lhs.offset == rhs.offset &&
           lhs.location == rhs.location;
}
