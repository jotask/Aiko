#pragma once

#include "assets/import/model_import_result.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"

#include <aiko_types.h>

#include "assets/types/shader_asset.h"

namespace aiko
{

    class AssetImporter
    {
    public:
        static TextureAsset loadTexture(const string& path);
        static MeshAsset loadMesh(const string& path);
        static ImportedModel loadModel(const string& path);
        static ShaderAsset loadShader(const string& path);
        static ComputeShaderAsset loadComputeShader(const string& path);
    };

}
