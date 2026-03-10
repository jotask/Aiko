#include "compute_shader_component.h"

#include "models/mesh_factory.h"
#include "assets/icomponent_assetaccess.h"
#include "models/game_object.h"

namespace aiko
{
    ComputeShaderComponent::ComputeShaderComponent()
        : Component("ComputeShaderComponent")
    {
    }

    void ComputeShaderComponent::update()
    {
        // FIXME
        // DEBUG remove this
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
    }

    void ComputeShaderComponent::load(string path)
    {
        AIKO_ASSERT(path.empty() == false, "Attempting to load empty compute shader path");
        IComponentAssetAccess* accessor = gameobject->getAiko()->getComponentAssetAccess();
        AIKO_ASSERT(accessor != nullptr, "No AssetSystem available");
        setShaderId(accessor->registerComputeShader(path));
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
}
