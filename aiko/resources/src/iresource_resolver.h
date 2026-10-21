#pragma once

#include "resource_result.h"

#include <string_view>

namespace aiko
{
    class IResourceResolver
    {
    public:
        virtual ~IResourceResolver() = default;

        [[nodiscard]]
        virtual ResourceResult resolve(std::string_view reference) const = 0;
    };
}