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
    virtual PostEntry Next();

    virtual PostEntry NextDocument();

    virtual PostEntry Seek(size_t target);

    virtual size_t GetStartLocation();

    virtual size_t GetEndLocation();

   private:
};