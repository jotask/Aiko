#pragma once

#include "aiko_types.h"

#include <type_traits>

namespace aiko
{
    
    template < typename C, C beginVal, C endVal>
    class EnumIterator
    {
        typedef typename std::underlying_type<C>::type val_t;
        int val;
    public:
        EnumIterator(const C& f) : val(static_cast<val_t>(f)) {}
        EnumIterator() : val(static_cast<val_t>(beginVal)) {}
        EnumIterator operator++() {
            ++val;
            return *this;
        }
        C operator*() { return static_cast<C>(val); }
        EnumIterator begin() { return *this; } //default ctor is good
        EnumIterator end() {
            static const EnumIterator endIter = ++EnumIterator(endVal); // cache it
            return endIter;
        }
        bool operator!=(const EnumIterator& i) { return val != i.val; }
    };


}
