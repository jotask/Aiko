#pragma once

#include <memory>
#include <string>
#include <cassert>
#include <vector>
#include <stdexcept>

namespace aiko
{

    using uint = uint32_t;
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using string = std::string;
    using string_view = std::string_view;

    template <typename T>
    using vector = std::vector<T>;

    template<class T>
    using AikoPtr = std::shared_ptr<T>;

    template<class T>
    using AikoUPtr = std::unique_ptr<T>;

}

#include "aiko_macros.h"

