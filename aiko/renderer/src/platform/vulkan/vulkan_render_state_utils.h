#pragma once

#include "aiko_macros.h"
#include "vulkan_device_capabilities.h"
#include "types/render_state.h"

namespace aiko::renderer::vulkan
{
    inline VkCullModeFlags toVulkanCullMode(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None:  return VK_CULL_MODE_NONE;
            case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back:  return VK_CULL_MODE_BACK_BIT;
        }

        AIKO_ASSERT(false, "Unsupported CullMode");
        return VK_CULL_MODE_NONE;
    }

    inline VkCompareOp toVulkanDepthCompare(DepthCompare compare)
    {
        switch (compare)
        {
            case DepthCompare::Less:         return VK_COMPARE_OP_LESS;
            case DepthCompare::LessEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
            case DepthCompare::Equal:        return VK_COMPARE_OP_EQUAL;
            case DepthCompare::Greater:      return VK_COMPARE_OP_GREATER;
            case DepthCompare::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case DepthCompare::Always:       return VK_COMPARE_OP_ALWAYS;
        }

        AIKO_ASSERT(false, "Unsupported DepthCompare");
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    }

    inline VkPolygonMode toVulkanPolygonMode(FillMode mode, const VulkanDeviceCapabilities& capabilities)
    {
        switch (mode)
        {
        case FillMode::Solid:
            return VK_POLYGON_MODE_FILL;

        case FillMode::Wireframe:
            AIKO_ASSERT(capabilities.nonSolidFill, "Wireframe fill mode is not supported by this Vulkan device");
            return VK_POLYGON_MODE_LINE;

        case FillMode::Point:
            AIKO_ASSERT(capabilities.nonSolidFill, "Point fill mode is not supported by this Vulkan device");
            return VK_POLYGON_MODE_POINT;
        }

        AIKO_ASSERT(false, "Unsupported FillMode");
        return VK_POLYGON_MODE_FILL;
    }
}
