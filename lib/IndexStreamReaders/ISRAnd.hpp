#pragma once

#include <vector>

#include "ISR.hpp"

/**
 * @class ISRAnd
 * @brief An ISR class that uses the boolean AND operation on its children.
 *
 * This ISR class searches for documents where *all* of the children ISRs occur
 * within that document.
 */
class ISRAnd : public ISR {
   public:
    /**
     * @brief Constructs an ISRAnd for a group of child ISRs.
     *
     * @param children A vector of the children ISRs.
     */
    ISRAnd(std::vector<ISR*> children);

    /**
     * @brief Returns the absolute location of the earliest occurrence
     *        among the child ISRs at this moment.
     * 
     * @return int or -1 if this ISR is not pointing to anything.
     */
    virtual int GetStartLocation() override;

    /**
     * @brief Returns the absolute location of the latest occurrence
     *        among the child ISRs at this moment.
     * 
     * @return int or -1 if this ISR is not pointing to anything.
     */
    virtual int GetEndLocation() override;

    /**
      * @brief Returns the current PostEntry this ISR is pointing at.
      * 
      * @return A std::optional containing the current PostEntry or std::nullopt if there is none.
      */
    virtual std::optional<PostEntry> GetCurrentPostEntry() override;

    /**
      * @brief Returns the document name this ISR is pointing at.
      * 
      * @return The name of the document this ISR is currently on or an empty string if this ISR is not pointing to anything.
      */
    virtual std::string GetDocumentName() override;

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

    /// among the children ISRs, which has the smallest absolute location? is it child 0, child 1, etc.?
    int nearestTerm;

    /// among the children ISRs, which has the largest absolute location? is it child 0, child 1, etc.?
    int farthestTerm;

    /// at the current state of the ISR, what is the absolute location of the earliest sub-ISR?
    int nearestStartLocation;

    /// at the current state of the ISR, what is the absolute location of the latest sub-ISR?
    int nearestEndLocation;

    /// internal helper function
    void UpdateMarkers();

    /// internal helper function
    bool ChildrenOnSameDocument();

    /// internal helper function
    bool CatchUpStragglerISRs();
};