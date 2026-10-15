#pragma once

namespace aiko::renderer::vulkan
{
    struct VulkanDeviceCapabilities
    {
        bool nonSolidFill = false;

        bool largePoints = false;

        bool wideLines = false;
        float minLineWidth = 1.0f;
        float maxLineWidth = 1.0f;
    };
}
