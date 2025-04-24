#pragma once

#include <unordered_set>
#include <vector>

#include <ISR.hpp>
#include <ISRWord.hpp>

/**
 * @class ISRContainer
 * @brief An ISR class that has included and excluded terms.
 *
 * This ISR class searches for the included ISR's occurrences,
 * but with the caveat that it skips over documents that the
 * excluded terms are also in.
 */
class ISRContainer : public ISR {
   public:
    /**
     * @brief Constructs an ISRContainer for an included ISR structure and
     *        a list of excluded ISRWords.
     *
     * @param included An ISR structure for the included terms.
     * 
     * @param excluded An ISR structure for the excluded terms.
     */
    ISRContainer(ISR* included, ISR* excluded);

    /**
     * @brief Deletes all children ISRs
    */
    ~ISRContainer() override;

    /**
     * @brief Returns the absolute location of the earliest occurrence
     *        among the included ISR at this moment.
     * 
     * @return int
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual int GetStartLocation() override;

    /**
     * @brief Returns the absolute location of the latest occurrence
     *        among the included ISR at this moment.
     * 
     * @return int
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
    /// the included ISR structure this ISR is primarily trying to find occurrences of
    ISR* included;

    /// The excluded ISR structure.
    ISR* excluded;

    /// private internal helper function
    std::optional<PostEntry> MatchNotOnExcluded();
};