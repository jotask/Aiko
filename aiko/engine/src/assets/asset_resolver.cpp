#include "asset_resolver.h"

#include <system_error>
#include <utility>

namespace aiko
{
    namespace
    {

        enum class RequestedOwner
        {
            Automatic,
            Project,
            Engine
        };

        struct ParsedRequest
        {
            RequestedOwner owner = RequestedOwner::Automatic;
            std::filesystem::path path;
        };

        AssetResolveResult failure(AssetResolveError error, std::string message)
        {
            AssetResolveResult result;
            result.error = error;
            result.message = std::move(message);
            return result;
        }

        bool isWithin(const std::filesystem::path& path, const std::filesystem::path& root)
        {
            auto pathIt = path.begin();
            auto rootIt = root.begin();

            for (; rootIt != root.end(); ++rootIt, ++pathIt)
            {
                if (pathIt == path.end() || *pathIt != *rootIt)
                {
                    return false;
                }
            }

            return true;
        }

        bool parseRequest(std::string_view source, ParsedRequest& request)
        {
            constexpr std::string_view projectPrefix = "project://";
            constexpr std::string_view enginePrefix = "engine://";

            if (source.starts_with(projectPrefix))
            {
                request.owner = RequestedOwner::Project;
                source.remove_prefix(projectPrefix.size());
            }
            else if (source.starts_with(enginePrefix))
            {
                request.owner = RequestedOwner::Engine;
                source.remove_prefix(enginePrefix.size());
            }
            else if (source.find("://") != std::string_view::npos)
            {
                return false;
            }

            if (source.empty() || source.find('\\') != std::string_view::npos)
            {
                return false;
            }

            // Reject traversal instead of silently normalizing it.
            request.path = std::filesystem::path(source);

            if (request.path.is_absolute() || request.path.has_root_name() || request.path.has_root_directory())
            {
                return false;
            }

            for (const auto& part : request.path)
            {
                if (part == "..")
                {
                    return false;
                }
            }

            request.path = request.path.lexically_normal();

            return request.path != "." && !request.path.empty();
        }

        AssetResolveResult resolveFromRoot(const std::filesystem::path& root, const std::filesystem::path& relative, AssetOwner owner)
        {
            if (root.empty())
            {
                return failure(AssetResolveError::InvalidConfiguration, "Asset root is not configured");
            }

            std::error_code error;

            const std::filesystem::path canonicalRoot = std::filesystem::canonical(root, error);

            if (error)
            {
                return failure(AssetResolveError::InvalidConfiguration, "Invalid asset root '" + root.string() + "': " + error.message());
            }

            if (!std::filesystem::is_directory(canonicalRoot, error) || error)
            {
                return failure(AssetResolveError::InvalidConfiguration, "Asset root is not a directory: " + canonicalRoot.string());
            }

            const std::filesystem::path candidate = canonicalRoot / relative;

            const std::filesystem::file_status status = std::filesystem::symlink_status(candidate, error);

            if (error == std::errc::no_such_file_or_directory)
            {
                return failure(AssetResolveError::NotFound, "Asset not found: " + candidate.string());
            }

            if (error)
            {
                return failure(AssetResolveError::FilesystemError, "Cannot inspect asset '" + candidate.string() + "': " + error.message());
            }

            if (status.type() == std::filesystem::file_type::not_found)
            {
                return failure(AssetResolveError::NotFound, "Asset not found: " + candidate.string());
            }

            // Existing entries, including symlinks, must resolve
            // successfully. Broken symlinks must not trigger fallback.
            const std::filesystem::path canonicalFile = std::filesystem::canonical(candidate, error);

            if (error)
            {
                return failure(AssetResolveError::FilesystemError, "Cannot resolve asset '" + candidate.string() + "': " + error.message());
            }

            if (!isWithin(canonicalFile, canonicalRoot))
            {
                return failure(AssetResolveError::InvalidPath, "Asset escapes its root: " + candidate.string());
            }

            if (!std::filesystem::is_regular_file(canonicalFile, error) || error)
            {
                return failure(AssetResolveError::InvalidPath, "Asset is not a regular file: " + canonicalFile.string());
            }

            const std::string logicalPath = relative.generic_string();

            const std::string prefix = owner == AssetOwner::Project ? "project://" : "engine://";

            AssetResolveResult result;
            result.asset.owner = owner;
            result.asset.logicalPath = logicalPath;
            result.asset.identity = prefix + logicalPath;
            result.asset.physicalPath = canonicalFile;

            return result;
        }
    }

    AssetResolver::AssetResolver(AssetConfiguration configuration)
        : m_configuration(std::move(configuration))
    {
    }

    AssetResolveResult AssetResolver::resolve(std::string_view source) const
    {
        ParsedRequest request;

        if (!parseRequest(source, request))
        {
            return failure(AssetResolveError::InvalidPath, "Invalid asset reference: " + std::string(source));
        }

        if (request.owner == RequestedOwner::Project)
        {
            return resolveFromRoot(m_configuration.projectRoot, request.path, AssetOwner::Project);
        }

        if (request.owner == RequestedOwner::Engine)
        {
            return resolveFromRoot(m_configuration.engineRoot, request.path, AssetOwner::Engine);
        }

        // An application without a project root may still use
        // the engine's built-in assets.
        if (!m_configuration.projectRoot.empty())
        {
            AssetResolveResult project = resolveFromRoot(m_configuration.projectRoot, request.path, AssetOwner::Project);

            if (project.error != AssetResolveError::NotFound)
            {
                return project;
            }
        }

        return resolveFromRoot(m_configuration.engineRoot, request.path, AssetOwner::Engine);
    }
}
