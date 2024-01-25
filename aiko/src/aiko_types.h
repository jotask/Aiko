#pragma once

#include <memory>
#include <type_traits>

namespace aiko
{

    template<class T>
    using AikoPtr = std::shared_ptr<T>;

    template<class T>
    using AikoUPtr = std::unique_ptr<T>;

}