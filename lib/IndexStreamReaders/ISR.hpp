#pragma once

#include <optional>
#include <string>
#include "PostEntry.hpp"

/**
 * @class ISR
 * @brief A base/parent class that other ISRs will derive from.
 *
 * This is the base ISR class that allows for searching through
 * the inverted word index.
 */
class ISR {
   public:
    virtual ~ISR() = default;

    /**
     * @brief Returns the starting location of whatever the ISR is currently on.
     * 
     * @return The starting location of whatever the ISR is currently on.
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual int GetStartLocation() = 0;

    /**
     * @brief Returns the ending location of whatever the ISR is currently on.
     * 
     * @return The ending location of whatever the ISR is currently on.
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual int GetEndLocation() = 0;

    /**
     * @brief Returns the current PostEntry this ISR is pointing at.
     * 
     * @return A std::optional containing the current PostEntry or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> GetCurrentPostEntry() = 0;

    /**
     * @brief Returns the document ID this ISR is pointing at.
     * 
     * @return The ID of the document this ISR is currently on.
     * 
     * @pre The ISR must currently be pointing to a valid PostEntry.
     */
    virtual uint32_t GetDocumentID() = 0;

    virtual size_t GetDocumentStart() = 0;

    /**
     * @brief Returns the next PostEntry that matches the occurrence of
     * this term that this ISR is looking for and advances the ISR forward.
     * 
     * @return A std::optional containing the next PostEntry or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> Next() = 0;

    /**
     * @brief Returns the next PostEntry that belongs in a brand new document
     *        that matches the occurrence of this term that this ISR is looking for
     *        and advances the ISR forward.
     * 
     * @return A std::optional containing the next PostEntry in the next document or std::nullopt if there is none.
     */
    virtual std::optional<PostEntry> NextDocument() = 0;

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
    virtual std::optional<PostEntry> Seek(size_t target) = 0;
};
