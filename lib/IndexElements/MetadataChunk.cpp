#include <cassert>
#include <cstring>

#include "MetadataChunk.hpp"

MetadataChunk::MetadataChunk() : _bytesRequired(0) {}

size_t MetadataChunk::GetBytesRequired() {
    return _bytesRequired;
}

void MetadataChunk::Print() const {
    cout << _docMetadata.size() << " Documents: ";
    for (std::string doc : _documents) {
        cout << doc << " ";
    }
    cout << endl;
    cout << "Metadata" << endl;
    for (auto [doc, metadata] : _docMetadata) {
        cout << doc << " stats:\n" << metadata << "\n";
    }
}

void MetadataChunk::AddDocument(std::string doc, metadata_t metadata) {

    _documents.push_back(doc);

    // Add to set of documents
    if (_docMetadata.find(doc) == _docMetadata.end()) {
        _docMetadata.insert(make_pair(doc, metadata));
    }

    // figure this out
    _bytesRequired += doc.size() + 1 + metadata.getBytes();
}

std::vector<std::string> MetadataChunk::GetDocuments() {
    return _documents;
}

metadata_t MetadataChunk::GetMetadata(std::string doc) {
    return _docMetadata[doc];
}

void MetadataChunk::Serialize(char* base_region, size_t& offset,
                              MetadataChunk& chunk) {
    assert(offset == 0);

    // Serialize document list size
    size_t num_documents = chunk._documents.size();
    std::memcpy(base_region + offset, &num_documents, sizeof(num_documents));
    offset += sizeof(num_documents);

    // Serialize document list
    // Serialize metadata immediately after
    for (std::string& docname : chunk._documents) {
        size_t docname_size = docname.size() + 1;
        std::memcpy(base_region + offset, docname.c_str(), docname_size);
        offset += docname_size;

        metadata_t& metadata = chunk._docMetadata[docname];
        // size_t metadata_size = index._docMetadata[docname].getBytes();
        // std::memcpy(base_region + offset, &index._docMetadata[docname], metadata_size);
        // offset+=metadata_size;

        std::memcpy(base_region + offset, &metadata.numWords,
                    sizeof(metadata.numWords));
        offset += sizeof(metadata.numWords);

        std::memcpy(base_region + offset, &metadata.numTitleWords,
                    sizeof(metadata.numTitleWords));
        offset += sizeof(metadata.numTitleWords);

        std::memcpy(base_region + offset, &metadata.pageRank,
                    sizeof(metadata.pageRank));
        offset += sizeof(metadata.pageRank);

        std::memcpy(base_region + offset, &metadata.cheiRank,
                    sizeof(metadata.cheiRank));
        offset += sizeof(metadata.cheiRank);

        uint32_t numOutLinks = metadata.numOutLinks;
        std::memcpy(base_region + offset, &numOutLinks, sizeof(numOutLinks));
        offset += sizeof(numOutLinks);
    }

    // Serailize metadata for each document
    // for (auto& [doc, metadata] : index._docMetadata) {
    //     size_t metadata_size = metadata.getBytes();
    //     std::memcpy(base_region + offset, &metadata, metadata_size);
    //     offset+=metadata_size;
    // }
}

MetadataChunk MetadataChunk::Deserailize(char* base_region, size_t& offset) {
    assert(offset == 0);
    MetadataChunk chunk;

    // Read document list size
    size_t num_documents = 0;
    std::memcpy(&num_documents, base_region + offset, sizeof(num_documents));
    offset += sizeof(num_documents);

    // Read chunk list
    for (size_t i = 0; i < num_documents; ++i) {
        std::string docname = std::string(base_region + offset);
        offset += docname.size() + 1;
        chunk._documents.push_back(docname);

        // Read number of words
        uint32_t numWords = 0, numTitleWords = 0, numOutLinks = 0;
        float pageRank = 0.0, cheiRank = 0.0;

        std::memcpy(&numWords, base_region + offset, sizeof(numWords));
        offset += sizeof(numWords);

        std::memcpy(&numTitleWords, base_region + offset,
                    sizeof(numTitleWords));
        offset += sizeof(numTitleWords);

        std::memcpy(&pageRank, base_region + offset, sizeof(pageRank));
        offset += sizeof(pageRank);

        std::memcpy(&cheiRank, base_region + offset, sizeof(cheiRank));
        offset += sizeof(cheiRank);

        std::memcpy(&numOutLinks, base_region + offset, sizeof(numOutLinks));
        offset += sizeof(numOutLinks);

        metadata_t metadata{numWords, numTitleWords, numOutLinks, pageRank, cheiRank};
        chunk._docMetadata.insert(make_pair(docname, metadata));
    }

    // size_t numWords;
    // size_t numTitleWords;
    // float pageRank;
    // float cheiRank;
    // size_t numOutLinks;
    // std::vector<std::string> outLinks;

    // for (size_t i = 0; i < num_documents; ++i) {

    //     // Read number of words
    //     size_t numWords = 0, numTitleWords = 0, numOutLinks = 0;
    //     float pageRank = 0.0, cheiRank = 0.0;

    //     std::memcpy(&numWords, base_region+offset, sizeof(numWords));
    //     offset+=sizeof(numWords);

    //     std::memcpy(&numTitleWords, base_region+offset, sizeof(numTitleWords));
    //     offset+=sizeof(numTitleWords);

    //     std::memcpy(&pageRank, base_region+offset, sizeof(pageRank));
    //     offset+=sizeof(pageRank);

    //     std::memcpy(&cheiRank, base_region+offset, sizeof(cheiRank));
    //     offset+=sizeof(cheiRank);

    //     std::memcpy(&numOutLinks, base_region+offset, sizeof(numOutLinks));
    //     offset+=sizeof(numOutLinks);

    //     //Read outward links
    //     std::vector<std::string> outLinks;
    //     for (size_t j = 0; j < numOutLinks; ++j) {
    //         std::string link = std::string(base_region+offset);
    //         std::memcpy(&numOutLinks, base_region+offset, sizeof(numOutLinks));
    //         offset+=link.size()+1;
    //         outLinks.push_back(link);
    //     }

    //     metadata_t metadata{numWords, numTitleWords, pageRank, cheiRank, numOutLinks, outLinks};
    //     chunk._docMetadata.insert(make_pair(chunk._documents[i], metadata));
    // }

    return chunk;
}
