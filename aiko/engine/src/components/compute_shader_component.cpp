#include "compute_shader_component.h"

#include "models/mesh_factory.h"
#include "models/game_object.h"

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
        /*
        if (hasReadback() == true)
        {
            const auto& readback = getLastReadback();
            const vec4* values = reinterpret_cast<const vec4*>(readback.data.data());
            logger::Log::info("compute[0] = (%f, %f, %f, %f)", values[0].x, values[0].y, values[0].z, values[0].w);
            logger::Log::info("compute[1] = (%f, %f, %f, %f)", values[1].x, values[1].y, values[1].z, values[1].w);
            logger::Log::info("compute[2] = (%f, %f, %f, %f)", values[2].x, values[2].y, values[2].z, values[2].w);
            logger::Log::info("compute[3] = (%f, %f, %f, %f)", values[3].x, values[3].y, values[3].z, values[3].w);
            clearReadback();
        }
        */
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

    void ComputeShaderComponent::resolveAssetBinding(AssetBindingContext& context)
    {
        if (m_shader.isRequested() == false)
        {
            return;
        }

        m_shader.markLoading();

        const AssetId id = context.load<ComputeShaderAsset>(m_shader.source());

        if (id == InvalidAssetId)
        {
            m_shader.fail();
            return;
        }

        m_shader.resolve(id);
    }
}
