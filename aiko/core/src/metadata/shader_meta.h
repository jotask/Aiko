#pragma once

#include "aiko_types.h"
#include "assets/asset_id.h"

namespace aiko
{

    enum class ShaderStage { Vertex, Fragment, Compute};
    struct ShaderMeta
    {
        ShaderStage stage;
        AssetId assetId= InvalidAssetId;
        string path;
    };

    struct ProgramMeta
    {
        AssetId vertexShader= InvalidAssetId;
        AssetId fragmentShader= InvalidAssetId;
    };

}