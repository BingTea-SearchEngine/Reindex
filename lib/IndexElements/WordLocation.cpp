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

std::ostream& operator<<(std::ostream& os, const metadata_t& m) {
    os << "metadata_t\n{\n\t# of words=" << m.numWords
       << "\n\t# of title words=" << m.numTitleWords
       << "\n\t# of out links=" << m.numOutLinks
       << "\n\tpageRank score=" << m.pageRank
       << "\n\tcheiRank score=" << m.cheiRank;
    os << "\n\t}\n";

    return os;
}

bool operator==(const metadata_t& lhs, const metadata_t& rhs) {
    if (!(lhs.numWords == rhs.numWords &&
          lhs.numTitleWords == rhs.numTitleWords &&
          lhs.pageRank == rhs.pageRank && lhs.cheiRank == rhs.cheiRank &&
          lhs.numOutLinks == rhs.numOutLinks)) {
        return false;
    }

    return true;
}
