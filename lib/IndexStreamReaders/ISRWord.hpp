#pragma once

#include "ISR.hpp"
#include "PostingList.hpp"

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
         * @param pL A pointer to a posting list for a particular word.
         */
    ISRWord(PostingList* pL);
    
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
         * @brief Returns the current PostEntry this ISR is pointing at.
         * 
         * @return pointer to the current PostEntry.
         */
    PostEntry* GetCurrentPost();

    /**
         * @brief Returns the next PostEntry that matches the occurrence of
         * this word that this ISR is looking for and advances the ISR forward.
         * 
         * @return A pointer to a PostEntry object.
         */
    virtual PostEntry* Next() override;

    /**
         * @brief Returns the next PostEntry that belongs in a brand new document
         *        that matches the occurrence of this term that this ISR is looking for
         *        and advances the ISR forward.
         * 
         * @return A pointer to a PostEntry object.
         */
    virtual PostEntry* NextDocument() override;

    /**
         * @brief Returns the first PostEntry that matches the occurrence of
         *        this term that this ISR is looking for located at location >= target
         *        and advances the ISR forward.
         * 
         * @param target The location the ISR will start at to find the first
         *               matching PostEntry.
         * 
         * @return A pointer to a PostEntry object.
         * @todo What if can't find anything?
         */
    virtual PostEntry* Seek(size_t target) override;

   private:
    /// The postingList associated with this ISRWord
    PostingList* postingList;

    /// The index of the currentPost (document Post) this ISR is pointing at.
    size_t currentPostIdx;

    /// The current PostEntry this ISR is pointing at.
    PostEntry* currentPostEntry;

    /// The index of the currentPostEntry within all the PostEntries.
    size_t currentPostEntryIdx;

    /// The absolute location of whatever this ISR is pointing at, relative
    /// to the start of this index chunk.
    size_t absoluteLocation;
};