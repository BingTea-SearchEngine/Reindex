#pragma once

#include <vector>

#include "ISR.hpp"

/**
 * @class ISROr
 * @brief An ISR class that uses the boolean OR operation on its children.
 *
 * This ISR class searches for *any* instance of its children terms in the
 * inverted word index.
 */
class ISROr : public ISR {
   public:
    /**
     * @brief Constructs an ISROr for a group of child ISRs.
     *
     * @param children A vector of the children ISRs.
     */
    ISROr(std::vector<ISR*> children);

    /**
     * @brief Deletes all children ISRs
    */
    ~ISROr() override;

    /**
     * @brief Returns the absolute location of the earliest occurrence
     *        among the child ISRs at this moment.
     * 
     * @return int.
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual int GetStartLocation() override;

    /**
     * @brief Returns the absolute location of the latest occurrence
     *        among the child ISRs at this moment.
     * 
     * @return int.
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual int GetEndLocation() override;

    /**
      * @brief Returns the current PostEntry this ISR is pointing at.
      * 
      * @return A std::optional containing the current PostEntry or std::nullopt if there is none.
      */
    virtual std::optional<PostEntry> GetCurrentPostEntry() override;

    /**
     * @brief Returns the document ID this ISR is pointing at.
     * 
     * @return The ID of the document this ISR is currently on.
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual uint32_t GetDocumentID() override;

    virtual size_t GetDocumentStart() override;

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
     * @return A std::optional containing the next PostEntry in the next document or std::nullopt if there is none.
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
     * @return A std::optional containing the next PostEntry located at >= target or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> Seek(size_t target) override;

   private:
    /// the children ISRs that this ISR OR's on
    std::vector<ISR*> childISRs;

    /// a bucket of booleans to signify which childISR has no more entries left in them
    std::vector<bool> whichChildFinished;

    /// The current PostEntry this ISR is pointing at.
    std::optional<PostEntry> currentPostEntry;

    /// among the children ISRs, which has the smallest absolute location? is it child 0, child 1, etc.?
    int nearestTerm;

    /// at the current state of the ISR, what is the absolute location of the earliest sub-ISR?
    int nearestStartLocation;

    /// at the current state of the ISR, what is the absolute location of the latest sub-ISR?
    int nearestEndLocation;

    /// internal helper function
    void UpdateMarkers();

    /// internal helper function
    bool AllChildrenFinished();
};