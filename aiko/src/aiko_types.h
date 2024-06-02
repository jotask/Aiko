#pragma once

#include <memory>
#include <string>

#include <type_traits>

#include <memory>

#include <stdexcept>

#if defined(__cplusplus)
#define CLITERAL(type)      type
#else
#define CLITERAL(type)      (type)
#endif

namespace aiko
{

    using uint = unsigned int;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using string = std::string;

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

    template<typename ... Args>
    string string_format(const string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

}
