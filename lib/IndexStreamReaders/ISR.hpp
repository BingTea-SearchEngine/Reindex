#pragma once

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
    /**
         * @brief Returns the next PostEntry that matches the occurrence of
         * this term that this ISR is looking for and advances the ISR forward.
         * 
         * @return A pointer to a PostEntry object.
         */
    virtual PostEntry* Next() = 0;

    /**
         * @brief Returns the next PostEntry that belongs in a brand new document
         *        that matches the occurrence of this term that this ISR is looking for
         *        and advances the ISR forward.
         * 
         * @return A pointer to a PostEntry object.
         */
    virtual PostEntry* NextDocument() = 0;

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
    virtual PostEntry* Seek(size_t target) = 0;

    /**
         * @brief Returns the starting location of whatever the ISR is currently on.
         * 
         * @return The starting location of whatever the ISR is currently on.
         */
    virtual size_t GetStartLocation() = 0;

    /**
         * @brief Returns the ending location of whatever the ISR is currently on.
         * 
         * @return The ending location of whatever the ISR is currently on.
         */
    virtual size_t GetEndLocation() = 0;
};