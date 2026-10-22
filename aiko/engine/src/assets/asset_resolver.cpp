
#include "asset_resolver.h"

#include <iresource_resolver.h>

#include <utility>

namespace aiko
{
    namespace
    {
        enum class RequestedOwner
        {
            Automatic,
            Project,
            Engine,
            Renderer
        };

        struct ParsedRequest
        {
            RequestedOwner owner = RequestedOwner::Automatic;
            std::filesystem::path path;
        };

        AssetResolveError convertError(ResourceError error)
        {
            switch (error)
            {
                case ResourceError::None:
                    return AssetResolveError::None;

                case ResourceError::InvalidReference:
                case ResourceError::OutsideMount:
                    return AssetResolveError::InvalidPath;

                case ResourceError::UnknownNamespace:
                case ResourceError::InvalidMount:
                    return AssetResolveError::InvalidConfiguration;

                case ResourceError::NotFound:
                    return AssetResolveError::NotFound;

                case ResourceError::FilesystemError:
                    return AssetResolveError::FilesystemError;
            }

            return AssetResolveError::FilesystemError;
        }

        AssetResolveResult failure(AssetResolveError error, std::string message)
        {
            return {
                .error = error,
                .message = std::move(message)
            };
        }

        bool parseRequest(std::string_view source, ParsedRequest& request)
        {
            constexpr std::string_view projectPrefix = "project://";
            constexpr std::string_view enginePrefix = "engine://";
            constexpr std::string_view rendererPrefix = "renderer://";

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
            else if (source.starts_with(rendererPrefix))
            {
                request.owner = RequestedOwner::Renderer;
                source.remove_prefix(rendererPrefix.size());
            }
            else if (source.find("://") != std::string_view::npos)
            {
                return false;
            }

            if (source.empty() || source.find('\\') != std::string_view::npos || source.find("://") != std::string_view::npos)
            {
                return false;
            }

            request.path = std::filesystem::path(source);

            if (request.path.empty() || request.path.is_absolute() || request.path.has_root_name() || request.path.has_root_directory())
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

        AssetResolveResult resolveMounted(const IResourceResolver& resources, std::string_view namespaceName, const std::filesystem::path& relative, AssetOwner owner)
        {
            const std::string logicalPath = relative.generic_string();

            const std::string reference = std::string(namespaceName) + "://" + logicalPath;

            const ResourceResult result = resources.resolve(reference);

            if (!result)
            {
                return failure(convertError(result.error), result.message);
            }

            std::string_view identityPrefix;

            switch (owner)
            {
                case AssetOwner::Project:
                    identityPrefix = "project://";
                    break;

                case AssetOwner::Engine:
                    identityPrefix = "engine://";
                    break;

                case AssetOwner::Renderer:
                    identityPrefix = "renderer://";
                    break;
            }

            const std::string identity = std::string(identityPrefix) + logicalPath;

            return {
                .asset = {
                    .owner = owner,
                    .logicalPath = logicalPath,
                    .identity = identity,
                    .physicalPath = result.path
                }
            };
        }
    }

    AssetResolver::AssetResolver(const IResourceResolver& resources, const AssetConfiguration& configuration)
        : m_resources(resources)
        , m_hasProjectAssets(!configuration.projectRoot.empty())
        , m_hasProjectShaders(!configuration.projectShaderBinaryRoot.empty())
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
            if (!m_hasProjectAssets)
            {
                return failure(AssetResolveError::InvalidConfiguration, "Project asset root is not configured");
            }

            return resolveMounted(m_resources, "project-assets", request.path, AssetOwner::Project);
        }

        if (request.owner == RequestedOwner::Engine)
        {
            return resolveMounted(m_resources, "engine-assets", request.path, AssetOwner::Engine);
        }

        if (request.owner == RequestedOwner::Renderer)
        {
            return failure(AssetResolveError::InvalidPath, "Renderer namespace is reserved for internal shader resources");
        }

        if (m_hasProjectAssets)
        {
            AssetResolveResult project = resolveMounted(m_resources, "project-assets", request.path, AssetOwner::Project);

            if (project.error != AssetResolveError::NotFound)
            {
                return project;
            }
        }

        return resolveMounted(m_resources, "engine-assets", request.path, AssetOwner::Engine);
    }

    AssetResolveResult AssetResolver::resolveShaderBinary(std::string_view source) const
    {
        ParsedRequest request;

        if (!parseRequest(source, request))
        {
            return failure(AssetResolveError::InvalidPath, "Invalid shader reference: " + std::string(source));
        }

        if (request.owner == RequestedOwner::Project)
        {
            if (!m_hasProjectShaders)
            {
                return failure(AssetResolveError::InvalidConfiguration, "Project shader root is not configured");
            }

            return resolveMounted(m_resources, "project-shaders", request.path, AssetOwner::Project);
        }

        if (request.owner == RequestedOwner::Engine)
        {
            return resolveMounted(m_resources, "engine-shaders", request.path, AssetOwner::Engine);
        }

        if (request.owner == RequestedOwner::Renderer)
        {
            return resolveMounted(m_resources, "renderer", request.path, AssetOwner::Renderer);
        }

        if (m_hasProjectShaders)
        {
            AssetResolveResult project = resolveMounted(m_resources, "project-shaders", request.path, AssetOwner::Project);

            if (project.error != AssetResolveError::NotFound)
            {
                return project;
            }
        }

        return resolveMounted(m_resources, "engine-shaders", request.path, AssetOwner::Engine);
    }

    ShaderPairResolveResult AssetResolver::resolveShaderPair(std::string_view vertexSource, std::string_view fragmentSource) const
    {
        ParsedRequest vertexRequest;
        ParsedRequest fragmentRequest;

        if (!parseRequest(vertexSource, vertexRequest) || !parseRequest(fragmentSource, fragmentRequest))
        {
            return {
                .error = AssetResolveError::InvalidPath,
                .message = "Invalid graphics shader reference"
            };
        }

        if (vertexRequest.owner != fragmentRequest.owner)
        {
            return {
                .error = AssetResolveError::InvalidPath,
                .message = "Graphics shader stages must use the same ownership"
            };
        }

        auto resolvePair = [&](std::string_view namespaceName, AssetOwner owner) -> ShaderPairResolveResult
        {
            const AssetResolveResult vertex = resolveMounted(m_resources, namespaceName, vertexRequest.path, owner);

            const AssetResolveResult fragment = resolveMounted(m_resources, namespaceName, fragmentRequest.path, owner);

            if (!vertex || !fragment)
            {
                const AssetResolveResult& failed = !vertex ? vertex : fragment;

                return {
                    .error = failed.error,
                    .message = failed.message
                };
            }

            return {
                .vertex = vertex.asset,
                .fragment = fragment.asset
            };
        };

        if (vertexRequest.owner == RequestedOwner::Renderer)
        {
            return resolvePair("renderer", AssetOwner::Renderer);
        }

        if (vertexRequest.owner == RequestedOwner::Engine)
        {
            return resolvePair("engine-shaders", AssetOwner::Engine);
        }

        if (vertexRequest.owner == RequestedOwner::Project)
        {
            if (!m_hasProjectShaders)
            {
                return {
                    .error = AssetResolveError::InvalidConfiguration,
                    .message = "Project shader root is not configured"
                };
            }

            return resolvePair("project-shaders", AssetOwner::Project);
        }

        if (m_hasProjectShaders)
        {
            const AssetResolveResult vertex = resolveMounted(m_resources, "project-shaders", vertexRequest.path, AssetOwner::Project);

            const AssetResolveResult fragment = resolveMounted(m_resources, "project-shaders", fragmentRequest.path, AssetOwner::Project);

            if (vertex.error == AssetResolveError::NotFound && fragment.error == AssetResolveError::NotFound)
            {
                return resolvePair("engine-shaders", AssetOwner::Engine);
            }

            if (!vertex || !fragment)
            {
                const AssetResolveResult& failed = !vertex ? vertex : fragment;

                return {
                    .error = failed.error,
                    .message = failed.message
                };
            }

            return {
                .vertex = vertex.asset,
                .fragment = fragment.asset
            };
        }

        return resolvePair("engine-shaders", AssetOwner::Engine);
    }
}
