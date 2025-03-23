#include "MasterChunk.hpp"

MasterChunk::MasterChunk() {

}

void MasterChunk::addDocument(std::string doc, std::vector<PostEntry> words) {
    // Check if index will become too big
    // If too big write to disk and reinitialize _currIndexChunk
    
    _currIndexChunk.addDocument(doc, words);
}
