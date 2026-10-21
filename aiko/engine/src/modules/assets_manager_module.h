#pragma once

#include <filesystem_resource_resolver.h>

#include "assets/asset_manager.h"
#include "modules/base_module.h"
#include "assets/asset_configuration.h"
#include "assets/asset_resolver.h"

namespace aiko
{

    class AssetsManagerModule : public BaseModule
    {
    
    public:

        AssetsManagerModule(Aiko* aiko);
        virtual ~AssetsManagerModule() = default;

        AssetManager* getManager() { return &m_manager; }

        const AssetResolver& getResolver() const { return m_resolver; }

        const IResourceResolver& getResourceResolver() const { return m_resourceResolver; }

    protected:

        virtual void preInit() override;
        virtual void init() override;
        virtual void update() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

    private:

        FilesystemResourceResolver m_resourceResolver;

        AssetConfiguration m_configuration;
        AssetResolver m_resolver;
        std::filesystem::path m_registryPath;

        AssetManager m_manager;

    };

}
