#include "asset_system.h"

#include "modules/assets_manager_module.h"
#include "modules/module_connector.h"
#include "modules/render_module.h"
#include "systems/job_system.h"
#include "system_connector.h"
#include "assets/asset_importer.h"

#include <logger/logger.h>

#include <functional>
#include <future>
#include <type_traits>
#include <utility>
#include <filesystem>

namespace aiko
{

    namespace
    {
        template<typename Work, typename Completion>
        void submitJob(JobSystem& jobs, std::unordered_map<AssetId, AssetLoadState>& loadStates, AssetId id, Work&& work, Completion&& completion)
        {
            using WorkType = std::decay_t<Work>;
            using CompletionType = std::decay_t<Completion>;
            using Result = std::invoke_result_t<WorkType&>;

            auto* states = &loadStates;

            jobs.submit(
                JobSystem::BackgroundJob(
                    [&jobs, states, id, work = WorkType(std::forward<Work>(work)), completion = CompletionType(std::forward<Completion>(completion))]
                    () mutable
                    {
                        Result result = std::invoke(work);

                        jobs.dispatchMainThread(
                            JobSystem::MainThreadJob(
                                [states, id, result = std::move(result), completion = std::move(completion)]
                                () mutable
                                {
                                    if (!result)
                                    {
                                        (*states)[id] = AssetLoadState::Failed;
                                        return;
                                    }

                                    std::invoke(completion, std::move(*result));

                                }
                            )
                        );
                    }
                )
            );
        }

    }

    AssetSystem::AssetSystem()
        : m_assetModule(nullptr)
    {

    }

    AssetResolveResult AssetSystem::resolveAsset(std::string_view source) const
    {
        return m_assetModule->getResolver().resolve(source);
    }

    AssetId AssetSystem::registerShader(string_view vertexSource, string_view fragmentSource)
    {
        return m_assetModule->getManager()->registerShader(vertexSource, fragmentSource);
    }

    void AssetSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetModule);
        BIND_SYSTEM_REQUIRED(JobSystem, systemConnector, m_jobSystem);
    }

    void AssetSystem::update()
    {
        for (auto it = m_finalizingModels.begin(); it != m_finalizingModels.end();)
        {
            const AssetId id = *it;

            const ModelAsset& model = m_assetModule->getManager()->getModelAsset(id);

            bool ready = true;
            bool failed = false;

            for (const ModelAsset::SubMesh& submesh : model.submeshes)
            {
                const AssetLoadState meshState = getLoadState<MeshAsset>(submesh.meshId);

                const AssetLoadState shaderState = getLoadState<ShaderAsset>(submesh.material.shaderId);

                if (meshState == AssetLoadState::Failed || shaderState == AssetLoadState::Failed)
                {
                    failed = true;
                    break;
                }

                if (meshState != AssetLoadState::Ready || shaderState != AssetLoadState::Ready)
                {
                    ready = false;
                }

                if (submesh.material.diffuseTextureId != InvalidAssetId)
                {
                    const AssetLoadState textureState = getLoadState<TextureAsset>(submesh.material.diffuseTextureId);

                    if (textureState == AssetLoadState::Failed)
                    {
                        failed = true;
                        break;
                    }

                    if (textureState != AssetLoadState::Ready)
                    {
                        ready = false;
                    }
                }
            }

            if (failed)
            {
                m_loadStates[id] = AssetLoadState::Failed;
                it = m_finalizingModels.erase(it);
                continue;
            }

            if (ready)
            {
                m_loadStates[id] = AssetLoadState::Ready;
                it = m_finalizingModels.erase(it);
                continue;
            }

            ++it;
        }
    }

    AssetId AssetSystem::registerAsset(AssetType type, string_view source)
    {
        AssetManager* manager = m_assetModule->getManager();
        switch (type)
        {
            case AssetType::Texture:        return manager->registerTexture(source);
            case AssetType::Mesh:           return manager->registerMesh(source);
            case AssetType::Model:          return manager->registerModel(source);
            case AssetType::Shader:         return manager->registerShader(source);
            case AssetType::ComputeShader:  return manager->registerComputeShader(source);

            case AssetType::Material:
            case AssetType::Unknown:
            default:
                AIKO_ASSERT(false, "Unsupported asset type");
                return InvalidAssetId;
        }
    }

    const TextureAsset& AssetSystem::get(const AssetId& id, std::type_identity<TextureAsset>)
    {
        return m_assetModule->getManager()->getTextureAsset(id);
    }

    const MeshAsset& AssetSystem::get(const AssetId& id, std::type_identity<MeshAsset>)
    {
        return m_assetModule->getManager()->getMeshAsset(id);
    }

    const ModelAsset& AssetSystem::get(const AssetId& id, std::type_identity<ModelAsset>)
    {
        return m_assetModule->getManager()->getModelAsset(id);
    }

    const ShaderAsset& AssetSystem::get(const AssetId& id, std::type_identity<ShaderAsset>)
    {
        return m_assetModule->getManager()->getShaderAsset(id);
    }

    const ComputeShaderAsset& AssetSystem::get(const AssetId& id, std::type_identity<ComputeShaderAsset>)
    {
        return m_assetModule->getManager()->getComputeShaderAsset(id);
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<TextureAsset>) const
    {
        return getLoadState(id, std::type_identity<TextureAsset>{}) == AssetLoadState::Ready;
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<MeshAsset>) const
    {
        return getLoadState(id, std::type_identity<MeshAsset>{}) == AssetLoadState::Ready;
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<ModelAsset>) const
    {
        return getLoadState(id, std::type_identity<ModelAsset>{}) == AssetLoadState::Ready;
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<ShaderAsset>) const
    {
        return getLoadState(id, std::type_identity<ShaderAsset>{}) == AssetLoadState::Ready;
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<ComputeShaderAsset>) const
    {
        return getLoadState(id, std::type_identity<ComputeShaderAsset>{}) == AssetLoadState::Ready;
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<TextureAsset>)
    {
        const AssetLoadState state = getLoadState<TextureAsset>(id);

        if (state != AssetLoadState::Unloaded && state != AssetLoadState::Failed)
        {
            return;
        }

        AssetManager* manager = m_assetModule->getManager();

        const AssetRecord* record = manager->m_registry.find(id);

        AIKO_ASSERT(record != nullptr && record->type == AssetType::Texture, "Texture asset is not registered");

        const TextureColorSpace colorSpace = record->textureColorSpace;

        const AssetResolveResult resolved = manager->resolveAsset(record->source);

        if (!resolved)
        {
            logger::Log::errorFmt("Failed to resolve texture '{}': {}", record->source, resolved.message);
            m_loadStates[id] = AssetLoadState::Failed;
            return;
        }

        const std::filesystem::path path = resolved.asset.physicalPath;

        m_loadStates[id] = AssetLoadState::Queued;

        submitJob(
            *m_jobSystem,
            m_loadStates,
            id,
            [path, colorSpace]()
            {
                return AssetImporter::loadTexture(path, colorSpace);
            },
            [this, id](TextureAsset asset)
            {
                m_loadStates[id] = AssetLoadState::Finalizing;
                m_assetModule->getManager()->installTextureAsset(id, std::move(asset));
                m_loadStates[id] = AssetLoadState::Ready;
            }
        );
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<MeshAsset>)
    {
        const AssetLoadState state = getLoadState<MeshAsset>(id);

        if (state != AssetLoadState::Unloaded && state != AssetLoadState::Failed)
        {
            return;
        }

        AssetManager* manager = m_assetModule->getManager();

        const string source = manager->getAssetSource(id, AssetType::Mesh);

        const AssetResolveResult resolved = manager->resolveAsset(source);

        if (!resolved)
        {
            logger::Log::errorFmt("Failed to resolve mesh '{}': {}", source, resolved.message);
            m_loadStates[id] = AssetLoadState::Failed;
            return;
        }

        const std::filesystem::path path = resolved.asset.physicalPath;

        m_loadStates[id] = AssetLoadState::Queued;

        submitJob(
            *m_jobSystem,
            m_loadStates,
            id,
            [path]()
            {
                return AssetImporter::loadMesh(path);
            },
            [this, id](MeshAsset asset)
            {
                m_loadStates[id] = AssetLoadState::Finalizing;
                m_assetModule->getManager()->installMeshAsset(id, std::move(asset));
                m_loadStates[id] = AssetLoadState::Ready;
            }
        );
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<ModelAsset>)
    {
        const AssetLoadState state = getLoadState<ModelAsset>(id);

        if (state != AssetLoadState::Unloaded && state != AssetLoadState::Failed)
        {
            return;
        }

        AssetManager* manager = m_assetModule->getManager();

        const string source = manager->getAssetSource(id, AssetType::Model);

        const AssetResolveResult resolved = manager->resolveAsset(source);

        if (!resolved)
        {
            logger::Log::errorFmt("Failed to resolve model '{}': {}", source, resolved.message);
            m_loadStates[id] = AssetLoadState::Failed;
            return;
        }

        const std::filesystem::path path = resolved.asset.physicalPath;

        m_loadStates[id] = AssetLoadState::Queued;

        submitJob(
            *m_jobSystem,
            m_loadStates,
            id,
            [path]()
            {
                return AssetImporter::loadModel(path);
            },
            [this, id, modelSource = resolved.asset](ImportedModel imported)
            {
                m_loadStates[id] = AssetLoadState::Finalizing;

                AssetManager* manager = m_assetModule->getManager();

                ModelAsset model{};

                for (ImportedModelSubMesh& importedSubmesh : imported.submeshes)
                {
                    ModelAsset::SubMesh submesh{};
                    submesh.meshId = manager->registerMesh(importedSubmesh.mesh);
                    submesh.material.useVertexColor = importedSubmesh.useVertexColor;
                    submesh.material.lit = importedSubmesh.lit;
                    submesh.material.baseColor = importedSubmesh.baseColor;
                    submesh.material.shaderId = manager->registerShader(importedSubmesh.shaderSource);

                    submesh.material.diffuseTextureId = InvalidAssetId;
                    if (!importedSubmesh.diffuseTextureSource.empty())
                    {
                        submesh.material.diffuseTextureId = manager->registerModelTexture(modelSource, importedSubmesh.diffuseTextureSource );

                        if (submesh.material.diffuseTextureId == InvalidAssetId)
                        {
                            logger::Log::errorFmt("Failed to register texture '{}' for model '{}'", importedSubmesh.diffuseTextureSource, modelSource.identity);
                            m_loadStates[id] = AssetLoadState::Failed;
                            return;
                        }
                    }

                    model.submeshes.push_back(std::move(submesh));
                }

                manager->installModelAsset(id, std::move(model));

                const ModelAsset& installed = manager->getModelAsset(id);

                for (const ModelAsset::SubMesh& submesh : installed.submeshes)
                {
                    loadAsset<MeshAsset>(submesh.meshId);
                    loadAsset<ShaderAsset>(submesh.material.shaderId);
                    if (submesh.material.diffuseTextureId != InvalidAssetId)
                    {
                        loadAsset<TextureAsset>(submesh.material.diffuseTextureId);
                    }
                }

                m_finalizingModels.insert(id);
            }
        );
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<ShaderAsset>)
    {
        if (id == InvalidAssetId)
        {
            return;
        }

        const AssetLoadState state = getLoadState<ShaderAsset>(id);

        if (state == AssetLoadState::Ready || state == AssetLoadState::Queued || state == AssetLoadState::Finalizing)
        {
            return;
        }

        AssetManager* manager = m_assetModule->getManager();

        // Explicit shader pairs are already installed at registration.
        if (manager->isShaderAssetLoaded(id))
        {
            m_loadStates[id] = AssetLoadState::Ready;
            return;
        }

        m_loadStates[id] = AssetLoadState::Finalizing;

        manager->loadShaderAsset(id);

        m_loadStates[id] = manager->isShaderAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Failed;
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<ComputeShaderAsset>)
    {
        if (id == InvalidAssetId)
        {
            return;
        }

        const AssetLoadState state = getLoadState<ComputeShaderAsset>(id);

        if (state == AssetLoadState::Ready || state == AssetLoadState::Queued || state == AssetLoadState::Finalizing)
        {
            return;
        }

        AssetManager* manager = m_assetModule->getManager();

        if (manager->isComputeShaderAssetLoaded(id))
        {
            m_loadStates[id] = AssetLoadState::Ready;
            return;
        }

        m_loadStates[id] = AssetLoadState::Finalizing;

        manager->loadComputeShaderAsset(id);

        m_loadStates[id] = manager->isComputeShaderAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Failed;
    }

    AssetLoadState AssetSystem::getLoadState(const AssetId& id, std::type_identity<TextureAsset>) const
    {
        const auto it = m_loadStates.find(id);
        if (it != m_loadStates.end())
        {
            return it->second;
        }
        return m_assetModule->getManager()->isTextureAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Unloaded;
    }

    AssetLoadState AssetSystem::getLoadState(const AssetId& id, std::type_identity<MeshAsset>) const
    {
        const auto it = m_loadStates.find(id);
        if (it != m_loadStates.end())
        {
            return it->second;
        }
        return m_assetModule->getManager()->isMeshAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Unloaded;
    }

    AssetLoadState AssetSystem::getLoadState(const AssetId& id, std::type_identity<ModelAsset>) const
    {
        const auto it = m_loadStates.find(id);
        if (it != m_loadStates.end())
        {
            return it->second;
        }
        return m_assetModule->getManager()->isModelAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Unloaded;
    }

    AssetLoadState AssetSystem::getLoadState(const AssetId& id, std::type_identity<ShaderAsset>) const
    {
        const auto it = m_loadStates.find(id);
        if (it != m_loadStates.end())
        {
            return it->second;
        }
        return m_assetModule->getManager()->isShaderAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Unloaded;
    }

    AssetLoadState AssetSystem::getLoadState(const AssetId& id, std::type_identity<ComputeShaderAsset>) const
    {
        const auto it = m_loadStates.find(id);
        if (it != m_loadStates.end())
        {
            return it->second;
        }
        return m_assetModule->getManager()->isComputeShaderAssetLoaded(id) ? AssetLoadState::Ready : AssetLoadState::Unloaded;
    }

    AssetId AssetSystem::create(const TextureAsset& asset)
    {
        return m_assetModule->getManager()->registerTexture(asset);
    }

    AssetId AssetSystem::create(const MeshAsset& asset)
    {
        return m_assetModule->getManager()->registerMesh(asset);
    }

    AssetId AssetSystem::registerTexture(string_view source, TextureColorSpace colorSpace)
    {
        return m_assetModule->getManager()->registerTexture(source, colorSpace);
    }

    TextureAsset& AssetSystem::getMutableTextureAsset(const AssetId& id)
    {
        return m_assetModule->getManager()->getMutableTextureAsset(id);
    }

    void AssetSystem::invalidateTexture(const AssetId& id)
    {
        m_renderModule->updateTexture(id);
    }

    void AssetSystem::invalidateMesh(const AssetId& id)
    {
        m_renderModule->unloadMesh(id);
    }

    void AssetSystem::invalidateModel(const AssetId& id)
    {
        m_renderModule->unloadModel(id);
    }

    void AssetSystem::invalidateShader(const AssetId& id)
    {
        m_renderModule->unloadShader(id);
    }
}
