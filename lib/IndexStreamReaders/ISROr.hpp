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
     * @brief Returns the next PostEntry that matches the occurrence of
     * this word that this ISR is looking for and advances the ISR forward.
     * 
     * @return A pointer to a PostEntry object or nullptr if there is none.
     */
    virtual PostEntry* Next() override;

    /**
     * @brief Returns the next PostEntry that belongs in a brand new document
     *        that matches the occurrence of this term that this ISR is looking for
     *        and advances the ISR forward.
     * 
     * @return A pointer to a PostEntry object or nullptr if there is none.
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
     * @return A pointer to a PostEntry object or nullptr if there is none.
     */
    virtual PostEntry* Seek(size_t target) override;

   private:
    /// the children ISRs that this ISR OR's on
    std::vector<ISR*> childISRs;

    /// among the children ISRs, which has the smallest absolute location? is it child 0, child 1, etc.?
    size_t nearestTerm;

    /// at the current state of the ISR, what is the absolute location of the earliest sub-ISR?
    size_t nearestStartLocation;

    /// at the current state of the ISR, what is the absolute location of the latest sub-ISR?
    size_t nearestEndLocation;

    /// internal helper function
    void UpdateMarkers();
};