
#include "assets_manager_module.h"

#include "aiko.h"
#include "constants.h"

#include <filesystem>

namespace aiko
{
    namespace
    {
        AssetConfiguration makeAssetConfiguration(const AikoConfig& config)
        {
            AssetConfiguration result = config.assets;

            // Temporary development compatibility:
            // existing applications have not yet configured
            // project-local asset roots.
            if (result.engineRoot.empty())
            {
                result.engineRoot = std::filesystem::path(global::GLOBAL_ASSET_PATH);
            }

            return result;
        }

        std::filesystem::path registryPath(const AssetConfiguration& configuration)
        {
            // The project registry takes precedence when a
            // project root has explicitly been configured.
            const auto& root = configuration.projectRoot.empty() ? configuration.engineRoot : configuration.projectRoot;

            return root / ".aiko" / "asset_registry.json";
        }
    }

    AssetsManagerModule::AssetsManagerModule(Aiko* aiko)
        : BaseModule(aiko)
        , m_configuration(makeAssetConfiguration(aiko->getConfig()))
        , m_resolver(m_resourceResolver, m_configuration)
        , m_registryPath(registryPath(m_configuration))
        , m_manager(m_resolver)
    {
        const bool engineAssets = m_resourceResolver.mount("engine-assets", m_configuration.engineRoot);
        const bool engineShaders = m_resourceResolver.mount("engine-shaders", m_configuration.engineShaderBinaryRoot);
        const bool renderer = m_resourceResolver.mount("renderer", m_configuration.engineShaderBinaryRoot);
        AIKO_ASSERT(engineAssets && engineShaders && renderer, "Failed to configure engine resource mounts");

        if (!m_configuration.projectRoot.empty())
        {
            AIKO_ASSERT(m_resourceResolver.mount( "project-assets", m_configuration.projectRoot ), "Failed to configure project asset mount");
        }

        if (!m_configuration.projectShaderBinaryRoot.empty())
        {
            AIKO_ASSERT(m_resourceResolver.mount( "project-shaders", m_configuration.projectShaderBinaryRoot ), "Failed to configure project shader mount");
        }
    }

    void AssetsManagerModule::preInit()
    {
        std::error_code error;

        const bool exists = std::filesystem::exists(m_registryPath, error);

        AIKO_ASSERT(!error, "Failed to inspect asset registry path");

        if (error || !exists)
        {
            return;
        }

        const bool loaded = m_manager.loadRegistry(m_registryPath);

        AIKO_ASSERT(loaded, "Failed to load asset registry");
    }

    void AssetsManagerModule::init()
    {
    }

    void AssetsManagerModule::update()
    {
    }

    void AssetsManagerModule::beginFrame()
    {
    }

    void AssetsManagerModule::endFrame()
    {
    }

    void AssetsManagerModule::dispose()
    {
        const bool saved = m_manager.saveRegistry(m_registryPath);
        AIKO_ASSERT(saved, "Failed to save asset registry");
        m_manager.clear();
    }
}
