#pragma once

#include <cstdint>

namespace aiko::renderer::vulkan::abi
{

    static constexpr uint32_t GraphicsFrameSet = 0;
    static constexpr uint32_t GraphicsMaterialSet = 1;
    static constexpr uint32_t GraphicsGpuReadSet = 2;

    static constexpr uint32_t GraphicsFrameBinding = 0;

    static constexpr uint32_t MaterialUboBinding = 0;
    static constexpr uint32_t MaterialTextureBinding = 1;

    static constexpr uint32_t MaxGpuReadBindings = 16;

    static constexpr uint32_t ComputeSet = 0;

    static constexpr uint32_t MaxComputeBufferBindings = 4;
    static constexpr uint32_t MaxComputeImageBindings = 8;

    static constexpr uint32_t ComputeBufferBindingBase = 0;

    static constexpr uint32_t ComputeImageBindingBase = ComputeBufferBindingBase + MaxComputeBufferBindings;
    static constexpr uint32_t ComputeFrameBinding = ComputeImageBindingBase + MaxComputeImageBindings;

}