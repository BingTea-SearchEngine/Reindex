#pragma once

#include <stdint.h>
#include <iostream>

#include "WordLocation.hpp"

/** 
 * @class PostEntry
 * @brief Represents a single occurrence of a word in the corpus.
 *
 * This class is the most basic, atomic part of the inverted word index.
 * It provides capabilities to serialize and deserialize to/from storage.
 */
class PostEntry {
   public:
    /** 
         * @brief Default constructor for a PostEntry.
         */
    PostEntry();

    /** 
         * @brief Constructs a PostEntry object.
         * @param delta The relative position of this word in the index chunk.
         * @param location_found The section of the page where the word was found.
         */
    PostEntry(uint32_t delta, wordlocation_t location_found);

    // Friend function to overload the == operator for PostEntry comparison
    // friend bool operator==(const PostEntry& lhs, const PostEntry& rhs);

    // Friend function to overload the << operator for PostEntry printing
    friend std::ostream& operator<<(std::ostream& os, const PostEntry& obj);

    /**
         * @brief Returns the delta of this PostEntry.
         * 
         * @return A uint32_t of this PostEntry's delta.
         */
    uint32_t GetDelta();

    /**
         * @brief Returns the location of where this word occurred.
         * 
         * @return A wordlocation_t of where this word occurred.
         */
    wordlocation_t GetLocationFound();

    /*
        * @brief Returns the bytes required to serialize this post entry
        *
        * @return A size_t of the bytes required to serialize this entry
        */
    size_t GetBytesRequired();

    /**
         * @brief Serializes a given PostEntry object into a specific region of memory.
         *
         * @param base_region A pointer to the beginning of the contiguous memory region 
         *                    where serialization will occur. This is determined by the
         *                    uppermost parent that wants serialization. This contiguous
         *                    memory region is memory-mapped to disk.
         * @param offset A reference to an offset value. After serialization, 
         *               this will be updated to point to the next available memory region.
         * @param word_occurrence The PostEntry object to serialize.
         *
         * @pre `base_region` must be a valid pointer to a memory region that is mmap'ed.
         * @pre `offset` must be a valid number such that `base_region + offset`
         *       points to the target serialization location.
         * 
         * @post Writes the bytes of the PostEntry object into memory at the calculated region.
         * @post Updates `offset` to the next available memory location.
         */
    static void Serialize(char* base_region, size_t& offset,
                          const PostEntry& word_occurrence);

    /**
         * @brief Deserializes a PostEntry object from a specific region of memory.
         *
         * @param base_region A pointer to the beginning of the memory region containing the object.
         *                    This memory region is memory-mapped to disk.
         * @param offset A reference to an offset value. After deserialization, 
         *               this will be updated to point to the next available memory region.
         * @return The deserialized PostEntry object.
         *
         * @pre `base_region + offset` must point to a valid serialized PostEntry object.
         * @post A PostEntry object is created and the offset is updated.
         */
    static PostEntry Deserialize(char* base_region, size_t& offset);

   private:
    /// The relative position of this word in the index chunk.
    /// @todo Convert this to use relative deltas.
    uint32_t delta;

    /// The section of the page where the word was found.
    wordlocation_t location_found;
};
