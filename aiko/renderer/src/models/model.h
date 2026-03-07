#pragma once

#include "mesh.h"
#include "material.h"

namespace aiko
{
    class Model
    {
        // Runtime model resource.
        // Built from a ModelAsset and owned by RenderResourceManager.
    public:

        Model();
        ~Model() = default;
        void upload(const ModelAsset&);
        void unload();

        const AssetId& getModelId() const { return m_modelId; }
        void setModelId(const AssetId& id) { m_modelId = id; }

    private:

		AssetId m_modelId;

    };
}
