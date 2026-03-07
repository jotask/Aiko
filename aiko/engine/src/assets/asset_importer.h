#pragma once

#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"

#include <aiko_types.h>

#include "assets/types/shader_asset.h"

namespace aiko
{
    class AssetManager;
    class AssetImporter
    {
    public:
        static TextureAsset loadTexture(const string& path, AssetManager* manager);
        static MeshAsset loadMesh(const string& path, AssetManager* manager);
        static ModelAsset loadModel(const string& path, AssetManager* manager);
        static ShaderAsset loadShader(const string& path, AssetManager* manager);
    };

}
