#pragma once

#include "ISR.hpp"
#include "PostingList.hpp"
#include <optional>

/**
 * @class ISRWord
 * @brief An ISR class to search for a specific word.
 *
 * This is the most basic, atomic ISR. It searches for a singular
 * word in the inverted word index.
 */
class ISRWord : public ISR {
   public:
    /**
     * @brief Constructs an ISRWord for a particular word.
     *
     * @param pL A posting list for a particular word.
     */
    ISRWord(PostingList pL);

    /**
     * @brief Returns the absolute location of the earliest occurrence
     *        among the child ISRs at this moment.
     * 
     * @return size_t
     */
    virtual size_t GetStartLocation() override;

    /**
     * @brief Returns the absolute location of the latest occurrence
     *        among the child ISRs at this moment.
     * 
     * @return size_t
     */
    virtual size_t GetEndLocation() override;

    /**
     * @brief Returns the current PostEntry this ISR is pointing at.
     * 
     * @return A std::optional containing the current PostEntry or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> GetCurrentPostEntry() override;

    /**
     * @brief Returns the document name this ISR is pointing at.
     * 
     * @return The name of the document this ISR is currently on.
     */
    virtual std::string GetDocumentName() override;

    /**
     * @brief Returns the number of documents that this term appears in
     *        (ie the number of Posts).
     * 
     * @return size_t
     */
    size_t GetDocumentCount();

    /**
     * @brief Returns the number of occurrences that this term has
     *        (ie the number of PostEntries).
     * 
     * @return size_t
     */
    size_t GetNumberOfOccurrences();

    /**
     * @brief Returns the next PostEntry that matches the occurrence of
     * this word that this ISR is looking for and advances the ISR forward.
     * 
     * @return A std::optional containing the next PostEntry or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> Next() override;

    /**
     * @brief Returns the next PostEntry that belongs in a brand new document
     *        that matches the occurrence of this term that this ISR is looking for
     *        and advances the ISR forward.
     * 
     * @return A std::optional containing the next PostEntry or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> NextDocument() override;

    /**
     * @brief Returns the first PostEntry that matches the occurrence of
     *        this term that this ISR is looking for located at location >= target
     *        and advances the ISR forward.
     * 
     * @param target The location the ISR will start at to find the first
     *               matching PostEntry.
     * 
     * @return A std::optional containing the next PostEntry or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> Seek(size_t target) override;

   private:
    /// The postingList associated with this ISRWord
    PostingList postingList;

    /// The index of the current Post (document Post).
    size_t currentPostIdx;

    /// The current PostEntry this ISR is pointing at.
    std::optional<PostEntry> currentPostEntry;

    /// The index of the current PostEntry within all the PostEntries.
    size_t currentPostEntryIdx;

    /// The absolute location of whatever this ISR is pointing at, relative
    /// to the start of this index chunk.
    size_t absoluteLocation;

    /// The name of the document this ISR is currently pointing at.
    std::string documentName;
};
