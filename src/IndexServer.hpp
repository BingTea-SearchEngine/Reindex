#pragma once

#include <spdlog/fmt/bundled/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <argparse/argparse.hpp>
#include <cmath>
#include <fstream>
#include <memory>
#include <queue>
#include <sstream>

#include "Expression.hpp"
#include "GatewayServer.hpp"
#include "IndexChunk.hpp"
#include "IndexInterface.hpp"
#include "MasterChunk.hpp"
#include "Parser.hpp"
#include "Util.hpp"

struct search_result_t {
    std::string url;
    uint32_t numWords;
    uint32_t numTitleWords;
    uint32_t numOutLinks;
    float pageRank;
    float cheiRank;
    int community;
    int communityCount;
    uint32_t numTitleMatch;
    uint32_t numBodyMatch;
    uint32_t docNum;               // For snippet retrieval
    uint32_t docStartOffset;       // For snippet retrieval
    uint32_t matchAbsoluteOffset;  // For snippet retrieval
    float rankingScore;

    search_result_t(std::string url, uint32_t numWords, uint32_t numTitleWords,
                    uint32_t numOutLinks, float pageRank, float cheiRank, int community,
                    int communityCount, uint32_t numTitleMatch, uint32_t numBodyMatch,
                    uint32_t docNum, uint32_t docStartOffset, uint32_t matchAbsoluteOffset)
        : url(std::move(url)),
          numWords(numWords),
          numTitleWords(numTitleWords),
          numOutLinks(numOutLinks),
          pageRank(pageRank),
          cheiRank(cheiRank),
          community(community),
          communityCount(communityCount),
          numTitleMatch(numTitleMatch),
          numBodyMatch(numBodyMatch),
          docNum(docNum),
          docStartOffset(docStartOffset),
          matchAbsoluteOffset(matchAbsoluteOffset) {
        if (pageRank <= 0) {
            pageRank = 1e-9;
        }
        if (cheiRank <= 0) {
            cheiRank = 1e-9;
        }
        rankingScore = 10.0f * std::log(1 + numTitleMatch) +  // strongest boost
                       5.0f * std::log(1 + numBodyMatch) +    // secondary boost
                       3.0f * std::log1p(pageRank * 1e6f) +   // incoming link quality
                       2.0f * std::log1p(cheiRank * 1e6f) +   // hubness
                       1.0f * std::log(1 + numTitleWords) +   // descriptive title
                       0.5f * std::log(1 + numWords) +        // content depth
                       0.2f * std::log(1 + numOutLinks);      // resourcefulness
    }

    friend std::ostream& operator<<(std::ostream& os, const search_result_t& r) {
        os << "{"
           << "numWords=" << r.url << ", "
           << "numWords=" << r.numWords << ", "
           << "numTitleWords=" << r.numTitleWords << ", "
           << "numOutLinks=" << r.numOutLinks << ", "
           << "pageRank=" << r.pageRank << ", "
           << "cheiRank=" << r.cheiRank << ", "
           << "community=" << r.community << ", "
           << "communityCount=" << r.communityCount << ", "
           << "docNum=" << r.docNum << ", "
           << "docStartOffset=" << r.docStartOffset << ", "
           << "matchAbsoluteOffset=" << r.matchAbsoluteOffset << "staticScore=" << r.rankingScore
           << "}";
        return os;
    }
};

using search_results = std::vector<search_result_t>;

struct SearchResultCompare {
    bool operator()(const search_result_t& a, const search_result_t& b) {
        return a.rankingScore < b.rankingScore;  // max-heap: bigger float comes first
    }
};

class IndexServer {
   public:
    IndexServer(int port, int maxClients, std::string indexPath, std::string htmlPath,
                int matchCount, int waitTimeMS, int numChunksLoaded, MasterChunk master);

    void Start();

   private:
    search_results findDocuments(std::string query);

    void searchChunk(std::string query, size_t chunkIndex, int matchCount, search_results* reuslts);

    std::pair<std::string, std::string> getTitleAndSnippet(search_result_t docMetadata, int delta);

    void rank(search_results& input);

    IndexMessage _handleSearch(IndexMessage msg);

    Server _server;

    MasterChunk _master;

    std::unique_ptr<IndexChunk> _primaryIndexChunk;
    std::unique_ptr<IndexChunk> _secondaryIndexChunk;
    std::unique_ptr<MetadataChunk> _primaryMetadataChunk;
    std::unique_ptr<MetadataChunk> _secondaryMetadataChunk;

    std::vector<std::unique_ptr<IndexChunk>> _indexChunks;
    std::vector<std::unique_ptr<MetadataChunk>> _metadataChunks;

    std::string _htmlDir;

    int _matchCount;
    int _waitTimeMS;
    int _numChunksLoaded;
};
