#include "WordLocation.hpp"

std::ostream& operator<<(std::ostream& os, wordlocation_t loc) {
    switch (loc) {
        case wordlocation_t::title:
            os << "title";
            break;
        case wordlocation_t::bold:
            os << "bold";
            break;
        case wordlocation_t::body:
            os << "body";
            break;
        default:
            os << "unknown";
            break;
    }
    return os;
}
