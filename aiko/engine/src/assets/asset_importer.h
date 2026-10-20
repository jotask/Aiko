#pragma once

#include "assets/import/model_import_result.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "assets/asset_import_result.h"
#include "assets/types/shader_asset.h"

#include <aiko_types.h>

#include <filesystem>

namespace aiko
{

    class AssetImporter
    {
    public:
        static AssetImportResult<TextureAsset> loadTexture(const std::filesystem::path& path, TextureColorSpace colorSpace = TextureColorSpace::Linear);
        static AssetImportResult<MeshAsset> loadMesh(const std::filesystem::path& path);
        static AssetImportResult<ImportedModel> loadModel(const std::filesystem::path& path);

        static ShaderAsset loadShader(const string& path);
        static ComputeShaderAsset loadComputeShader(const string& path);
    };

}
