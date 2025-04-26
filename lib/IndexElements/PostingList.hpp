#pragma once

#include <unordered_map>
#include <vector>

#include "Post.hpp"

struct SyncPoint {
     size_t position;    // Absolute word position
     size_t post_idx;    // Index into posts vector
     size_t entry_idx;   // Index into Post's PostEntry vector
};

/**
 * @class PostingList
 * @brief Represents a list of posts associated with a specific word in an index chunk.
 *
 * This class is used to store a collection of posts that relate to a specific word. Each post contains information about 
 * a document and its associated word occurrences. It supports serialization and deserialization to/from memory.
 */
class PostingList {
   public:
    /**
         * @brief Default constructor for a PostingList.
         *
         */
    PostingList();

    /**
         * @brief Constructs a PostingList object for a specific word.
         *
         * @param word The word that this PostingList represents.
         */
    PostingList(const std::string& word);

    /**
        * @brief Returns the bytes required to serialize just the word and size of posts in this
        * posting list
        *
        * @return size_t
        */
    size_t GetOverheadBytesRequired();

    /**
         * @brief Gets the word associated with this PostingList.
         *
         * @return The word associated with this PostingList.
         */
    std::string GetWord();

    std::vector<SyncPoint> GetSyncTable() const;

    /**
         * @brief Adds a new word (PostEntry) to the list of posts in this PostingList.
         *
         * @param docID The ID of the document where the word is found.
         * @param word The PostEntry object that contains the word occurrence information.
         */
    size_t AddWord(uint32_t docID,  size_t earliestOccurrenceInDoc, PostEntry word);

    /**
         * @brief Retrieves the list of posts associated with this PostingList.
         *
         * @return A vector of Post objects associated with this PostingList.
         */
    std::vector<Post> GetPosts() const;

    /*
     * @brief Prints the contents of the PostingList
     * */
    void Print() const;

    /**
         * @brief Serializes a given PostingList object into a specific region of memory.
         *
         * @param base_region A pointer to the beginning of the contiguous memory region 
         *                    where serialization will occur. This is determined by the
         *                    uppermost parent that wants serialization. This contiguous
         *                    memory region is memory-mapped to disk.
         * @param offset A reference to an offset value. After serialization, 
         *               this will be updated to point to the next available memory region.
         * @param postingList The PostingList object to serialize.
         *
         * @pre `base_region` must be a valid pointer to a memory region that is mmap'ed.
         * @pre `offset` must be a valid number such that `base_region + offset`
         *       points to the target serialization location.
         * 
         * @post Writes the bytes of the PostingList object into memory at the calculated region.
         * @post Updates `offset` to the next available memory location.
         */
    static void Serialize(char* base_region, size_t& offset,
                          const PostingList& postingList);
     
    static void OldSerialize(char* base_region, size_t& offset,
                             const PostingList& postingList);

     static void NewSerialize(char* base_region, size_t& offset,
     const PostingList& postingList);

    /**
         * @brief Deserializes a PostingList object from a specific region of memory.
         *
         * @param base_region A pointer to the beginning of the memory region containing the object.
         *                    This memory region is memory-mapped to disk.
         * @param offset A reference to an offset value. After deserialization, 
         *               this will be updated to point to the next available memory region.
         * @return The deserialized PostingList object.
         *
         * @pre `base_region + offset` must point to a valid serialized PostingList object.
         * @post A PostingList object is created and the offset is updated.
         */
    static PostingList Deserialize(char* base_region, size_t& offset);

    static PostingList OldDeserialize(char* base_region, size_t& offset);

    static PostingList NewDeserialize(char* base_region, size_t& offset);

   private:
    /// The word associated with this PostingList
    std::string word;

    /// A vector of Post objects associated with the word
    std::vector<Post> posts;

    /// A synchronization table for efficient access
    std::vector<SyncPoint> sync_table;
};
