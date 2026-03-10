#pragma once

#include "models/component.h"

namespace aiko
{

    class ComputeShaderComponent : public Component , public IUpdate
    {
    public:
        ComputeShaderComponent();
        virtual ~ComputeShaderComponent() = default;

        virtual void update() override;

        void load(string path);

        void setShaderId(const AssetId& id) { m_shaderId = id; }
        const AssetId& getShaderId() const { return m_shaderId; }

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

    private:

        AssetId m_shaderId = InvalidAssetId;
        uint32_t m_elementCount = 64;
        bool m_enabled = true;
        bool m_requestReadback = true;

        ComputeReadbackResult m_lastReadback;


    };

}
