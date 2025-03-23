#pragma once

#include "PostEntry.hpp"

/**
 * @class Post
 * @brief Represents a collection of word occurrences (PostEntry) per parsed document.
 *
 * This class allows serialization and deserialization of a Post object, which includes the document's
 * name and a collection of word occurrences (PostEntry objects). It provides functionality for adding words,
 * getting word entries, and serializing or deserializing the post's data.
 */
class Post
{
    public:
        /**
         * @brief Default constructor for a Post.
         */
        Post();

        /**
         * @brief Constructs a Post with a specified name.
         * 
         * @param name The name of the document.
         */
        Post(std::string name);

        /**
         * @brief Returns a vector containing all word occurrences (PostEntry objects) in the post.
         * 
         * @return A vector of PostEntry objects.
         */
        std::vector<PostEntry> getEntries();

        /**
         * @brief Adds a word occurrence (PostEntry) to the post.
         * 
         * @param word The word occurrence (PostEntry) to add.
         */
        void addWord(PostEntry word);

        /**
         * @brief Serializes a given Post object into a specific region of memory.
         * 
         * This function writes the Post object's data into the memory region starting at the given
         * offset, updating the offset to point to the next available region of memory.
         * 
         * @param base_region The beginning of the memory region where the Post will be serialized.
         *                    This region is memory-mapped.
         * @param offset The offset within the memory region where the serialization should begin.
         * @param post The Post object to serialize.
         */
        static void Serialize(char* base_region, size_t &offset, const Post& post);

        /**
         * @brief Deserializes a Post object from a specific region of memory.
         * 
         * This function reconstructs a Post object from the bytes in the specified memory region, 
         * starting at the given offset, and returns the deserialized Post object.
         * 
         * @param base_region The memory region containing the serialized Post object.
         *                    This region is memory-mapped.
         * @param offset The offset within the memory region where the deserialization should begin.
         * 
         * @return The deserialized Post object.
         */
        static Post Deserialize(char* base_region, size_t &offset);

    private:
        /// The name of the document.
        std::string document_name;

        /// A vector containing the word occurrences (PostEntry objects) in the document.
        std::vector<PostEntry> entries;
};
