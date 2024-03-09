#pragma once

#include <memory>
#include <type_traits>

#if defined(__cplusplus)
#define CLITERAL(type)      type
#else
#define CLITERAL(type)      (type)
#endif

namespace aiko
{

    template<class T>
    using AikoPtr = std::shared_ptr<T>;

    template<class T>
    using AikoUPtr = std::unique_ptr<T>;

    template < typename C, C beginVal, C endVal>
    class EnumIterator {
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
