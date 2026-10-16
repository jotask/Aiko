#pragma once

#include "assets/import/model_import_result.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "assets/asset_import_result.h"

#include <aiko_types.h>

#include "assets/types/shader_asset.h"

namespace aiko
{

    class AssetImporter
    {
    public:
        static AssetImportResult<TextureAsset> loadTexture(const string& path);
        static AssetImportResult<MeshAsset> loadMesh(const string& path);
        static AssetImportResult<ImportedModel> loadModel(const string& path);

        static ShaderAsset loadShader(const string& path);
        static ComputeShaderAsset loadComputeShader(const string& path);
    };

}
