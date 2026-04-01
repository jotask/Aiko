 #pragma once

#include "models/shader.h"
#include "models/texture.h"
#include "types/color.h"

#include <unordered_map>

 namespace aiko
{
    class ComputeBuffer;

    class Material
    {
    public:

        friend class RenderModule;
        friend class RenderSystem;

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        Material(Material&&) noexcept = default;
        Material& operator=(Material&&) noexcept = default;

        Material();
        ~Material() = default;

        u64 id() const;

    public:

        AssetId m_shaderId = InvalidAssetId;

        const ComputeBuffer* m_gpuInstanceBuffer = nullptr;

        bool m_useVertexColor;
        bool m_lit;

		Color m_baseColor;
        AssetId m_diffuseTextureId = InvalidAssetId;
        const Texture* m_runtimeDiffuseTexture = nullptr;

        std::unordered_map<std::string, vec4> m_customVec4Uniforms;

    };

}
