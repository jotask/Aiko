#include "filesystem_resource_resolver.h"

#include <algorithm>
#include <system_error>

namespace aiko
{
    namespace
    {

        bool validNamespace(std::string_view name)
        {
            if (name.empty())
            {
                return false;
            }

            return std::all_of(name.begin(), name.end(),
                [](char ch)
                {
                    return (ch >= 'a' && ch <= 'z') ||
                           (ch >= 'A' && ch <= 'Z') ||
                           (ch >= '0' && ch <= '9') ||
                           ch == '_' || ch == '-';
                });
        }

        bool validRelativePath(std::string_view input)
        {
            if (input.empty() || input.find('\\') != std::string_view::npos || input.find("://") != std::string_view::npos)
            {
                return false;
            }

            const std::filesystem::path path{std::string(input)};

            if (path.empty() || path.is_absolute() || path.has_root_name() || path.has_root_directory())
            {
                return false;
            }

            for (const auto& part : path)
            {
                if (part == "..")
                {
                    return false;
                }
            }

            return path.lexically_normal() != ".";
        }

        bool containedIn(const std::filesystem::path& file, const std::filesystem::path& root)
        {
            auto fileIt = file.begin();

            for (auto rootIt = root.begin(); rootIt != root.end(); ++rootIt)
            {
                if (fileIt == file.end() || *fileIt != *rootIt)
                {
                    return false;
                }

                ++fileIt;
            }

            return true;
        }

        ResourceResult resolveWithin(const std::filesystem::path& root, std::string_view relative)
        {

            const std::filesystem::path candidate = root / std::string(relative);

            std::error_code ec;

            const std::filesystem::file_status status = std::filesystem::symlink_status(candidate, ec);

            if (ec == std::errc::no_such_file_or_directory || status.type() == std::filesystem::file_type::not_found)
            {
                return {
                    .error = ResourceError::NotFound,
                    .message = "Resource not found: " + candidate.string()
                };
            }

            if (ec)
            {
                return {
                    .error = ResourceError::FilesystemError,
                    .message = ec.message()
                };
            }

            const std::filesystem::path canonicalFile = std::filesystem::canonical(candidate, ec);

            if (ec)
            {
                return {
                    .error = ResourceError::FilesystemError,
                    .message = "Failed to canonicalize resource '" + candidate.string() + "': " + ec.message()
                };
            }

            if (!containedIn(canonicalFile, root))
            {
                return {
                    .error = ResourceError::OutsideMount,
                    .message = "Resource escapes mount: " + candidate.string()
                };
            }

            if (!std::filesystem::is_regular_file(canonicalFile, ec))
            {
                return {
                    .error = ec ? ResourceError::FilesystemError : ResourceError::InvalidReference,
                    .message = ec ? ec.message() : "Resource is not a regular file"
                };
            }

            return {
                .error = ResourceError::None,
                .path = canonicalFile
            };
        }
    }

    bool FilesystemResourceResolver::mount(std::string_view namespaceName, const std::filesystem::path& root, bool searchable)
    {
        if (!validNamespace(namespaceName) || root.empty())
        {
            return false;
        }

        if (std::any_of(m_mounts.begin(), m_mounts.end(),
            [&](const Mount& existing)
            {
                return existing.name == namespaceName;
            }))
        {
            return false;
        }

        std::error_code ec;
        const auto canonicalRoot = std::filesystem::canonical(root, ec);

        if (ec)
        {
            return false;
        }

        if (!std::filesystem::is_directory(canonicalRoot, ec) || ec)
        {
            return false;
        }

        m_mounts.push_back({
            .name = std::string(namespaceName),
            .root = canonicalRoot,
            .searchable = searchable
        });

        return true;
    }

    ResourceResult FilesystemResourceResolver::resolve(std::string_view reference) const
    {
        const size_t separator = reference.find("://");

        if (separator != std::string_view::npos)
        {
            const std::string_view name = reference.substr(0, separator);

            const std::string_view relative = reference.substr(separator + 3);

            if (!validNamespace(name) || !validRelativePath(relative))
            {
                return {
                    .error = ResourceError::InvalidReference,
                    .message = "Invalid resource reference: " + std::string(reference)
                };
            }

            const auto it = std::find_if(
                m_mounts.begin(), m_mounts.end(),
                [&](const Mount& entry)
                {
                    return entry.name == name;
                });

            if (it == m_mounts.end())
            {
                return {
                    .error = ResourceError::UnknownNamespace,
                    .message = "Unknown resource namespace: " + std::string(name)
                };
            }

            return resolveWithin(it->root, relative);
        }

        if (!validRelativePath(reference))
        {
            return {
                .error = ResourceError::InvalidReference,
                .message = "Invalid resource reference: " + std::string(reference)
            };
        }

        for (const Mount& entry : m_mounts)
        {
            if (!entry.searchable)
            {
                continue;
            }

            ResourceResult result = resolveWithin(entry.root, reference);

            if (result || result.error != ResourceError::NotFound)
            {
                return result;
            }
        }

        return {
            .error = ResourceError::NotFound,
            .message = "Resource not found: " + std::string(reference)
        };
    }
}