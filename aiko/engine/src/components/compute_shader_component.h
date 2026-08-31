#pragma once

#include "models/component.h"
#include "assets/asset_reference.h"
#include "assets/types/shader_asset.h"
#include "assets/asset_binding.h"

namespace aiko
{

    class ComputeShaderComponent : public Component, public IAssetBinding
    {
    public:

        enum class ComputeExecutionMode
        {
            Once,
            Continuous,
            OnDemand,
        };

        ComputeShaderComponent();
        virtual ~ComputeShaderComponent() = default;

        void load(string path);

        const AssetId& getShaderId() const;

        void setElementCount(uint32_t count) { m_elementCount = count; }
        uint32_t getElementCount() const { return m_elementCount; }

        void setEnabled(bool enabled) { m_enabled = enabled; }
        bool isEnabled() const { return m_enabled; }

        void requestReadback();
        bool consumeReadbackRequest();

        void setLastReadback(ComputeReadbackResult result) { m_lastReadback = std::move(result); }
        const ComputeReadbackResult& getLastReadback() const { return m_lastReadback; }
        bool hasReadback() const { return m_lastReadback.ready; }
        void clearReadback() { m_lastReadback = {}; }

        void setOutputSize(uint32_t width, uint32_t height);

        uint32_t getOutputWidth() const { return m_outputWidth; }
        uint32_t getOutputHeight() const { return m_outputHeight; }

        void setUseOutputTexture(bool value) { m_useOutputTexture = value; }
        bool usesOutputTexture() const { return m_useOutputTexture; }

        void setExecutionMode(ComputeExecutionMode mode) { m_executionMode = mode; }
        ComputeExecutionMode getExecutionMode() const { return m_executionMode; }

        void setUpdateInterval(float seconds) { m_intervalStep = seconds;}
        float getUpdateInterval() const { return m_intervalStep; }

        void requestDispatch();
        bool consumeDispatchRequest();

    private:

        bool resolveAssetBinding(AssetBindingContext& context) override;

        AssetReference<ComputeShaderAsset> m_shader;
        uint32_t m_elementCount = 64;
        bool m_enabled = true;
        bool m_requestReadback = true;

        ComputeReadbackResult m_lastReadback;
        ComputeExecutionMode m_executionMode = ComputeExecutionMode::Once;
        float m_intervalStep = 0.0f;
        bool m_dispatchRequested = false;

        // Texture readback
        uint32_t m_outputWidth = 0;
        uint32_t m_outputHeight = 0;
        bool m_useOutputTexture = false;


    };

}
