#include "compute_shader_component.h"

namespace aiko
{
    ComputeShaderComponent::ComputeShaderComponent()
        : Component("ComputeShaderComponent")
    {
    }

    void ComputeShaderComponent::load(string path)
    {
        m_shader.request(std::move(path));
        markAssetBindingDirty();
    }

    void ComputeShaderComponent::update()
    {

    }

    const AssetId& ComputeShaderComponent::getShaderId() const
    {
        return m_shader.isReady() ? m_shader.id() : InvalidAssetId;
    }

    void ComputeShaderComponent::requestReadback()
    {
        m_requestReadback = true;
    }

    bool ComputeShaderComponent::consumeReadbackRequest()
    {
        const bool value = m_requestReadback;
        m_requestReadback = false;
        return value;
    }

    void ComputeShaderComponent::setOutputSize(uint32_t width, uint32_t height)
    {
        m_outputWidth = width;
        m_outputHeight = height;
    }

    void ComputeShaderComponent::requestDispatch()
    {
        m_dispatchRequested = true;
    }

    bool ComputeShaderComponent::consumeDispatchRequest()
    {
        const bool value = m_dispatchRequested;
        m_dispatchRequested = false;
        return value;
    }

    bool ComputeShaderComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_shader.isRequested())
        {
            const AssetId id = context.load<ComputeShaderAsset>(m_shader.source());

            if (id == InvalidAssetId)
            {
                m_shader.fail();
                return false;
            }

            m_shader.markLoading(id);
            context.loadAsset<ComputeShaderAsset>(id);

            return true;
        }

        if (m_shader.isLoading())
        {
            const AssetId& id = m_shader.id();

            if (context.isLoaded<ComputeShaderAsset>(id) == false)
            {
                return true;
            }

            m_shader.resolve(id);
        }

        return false;
    }
}
