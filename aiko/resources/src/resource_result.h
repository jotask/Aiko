#pragma once

#include <filesystem>
#include <string>

namespace aiko
{
    enum class ResourceError
    {
        None,
        InvalidReference,
        UnknownNamespace,
        InvalidMount,
        NotFound,
        OutsideMount,
        FilesystemError
    };

    struct ResourceResult
    {
        ResourceError error = ResourceError::None;
        std::filesystem::path path;
        std::string message;

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return error == ResourceError::None;
        }
    };
}