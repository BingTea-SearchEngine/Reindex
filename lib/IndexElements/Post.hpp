#pragma once

#include <vector>

#include "PostEntry.hpp"

/**
 * @class Post
 * @brief Represents a collection of word occurrences (PostEntry) per parsed document.
 *
 * This class allows serialization and deserialization of a Post object, which includes the document's
 * name and a collection of word occurrences (PostEntry objects). It provides functionality for adding words,
 * getting word entries, and serializing or deserializing the post's data.
 */
class Post {
   public:
    /**
         * @brief Default constructor for a Post.
         */
    Post();

    /**
         * @brief Constructs a Post with a specified ID.
         * 
         * @param docID The ID of the document.
        */
    Post(uint32_t docID, size_t earliestOccurrenceInDoc);

    /**
         * @brief Returns the ID of this document that this Post represents.
         * 
         * @return The document's ID.
         */
    uint32_t GetDocumentID() const;

    // gets the earliest start variable
    size_t GetEarliestStart() const;

    /**
         * @brief Returns a vector containing all word occurrences (PostEntry objects) in the post.
         * 
         * @return A vector of PostEntry objects.
         */
    std::vector<PostEntry> GetEntries() const;

    /**
         * @brief Adds a word occurrence (PostEntry) to the post.
         * 
         * @param word The word occurrence (PostEntry) to add.
         */
    void AddWord(PostEntry word);

    /*
     * @brief Prints the contents of the PostEntry
     * */
    void Print() const;

    /**
         * @brief Serializes a given Post object into a specific region of memory.
         * 
         * This function writes the Post object's data into the memory region starting at the given
         * offset, updating the offset to point to the next available region of memory.
         * 
         * @param base_region A pointer to the beginning of the contiguous memory region 
         *                    where serialization will occur. This is determined by the
         *                    uppermost parent that wants serialization. This contiguous
         *                    memory region is memory-mapped to disk.
         * @param offset A reference to an offset value. After serialization, 
         *               this will be updated to point to the next available memory region.
         * @param post The Post object to serialize.
         * 
         * @pre `base_region` must be a valid pointer to a memory region that is mmap'ed.
         * @pre `offset` must be a valid number such that `base_region + offset`
         *       points to the target serialization location.
         * 
         * @post Writes the bytes of the Post object into memory at the calculated region.
         * @post Updates `offset` to the next available memory location.
         */
    static void Serialize(char* base_region, size_t& offset, const Post& post);

    /**
         * @brief Deserializes a Post object from a specific region of memory.
         * 
         * This function reconstructs a Post object from the bytes in the specified memory region, 
         * starting at the given offset, and returns the deserialized Post object.
         * 
         * @param base_region A pointer to the beginning of the memory region containing the object.
         *                    This memory region is memory-mapped to disk.
         * @param offset A reference to an offset value. After deserialization, 
         *               this will be updated to point to the next available memory region.
         * @return The deserialized Post object.
         * 
         * @pre `base_region + offset` must point to a valid serialized Post object.
         * @post A Post object is created and the offset is updated.
         */
    static Post Deserialize(char* base_region, size_t& offset);

   private:
    /// The ID of the document.
    uint32_t docID;

    /// The absolute word location of the first word in this document.
    size_t docStart;

    /// A vector containing the word occurrences (PostEntry objects) in the document that
    /// pertain to the specifc word this PostingList represents.
    std::vector<PostEntry> entries;
};
