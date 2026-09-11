#pragma once

#include <volk.h>

#include "aiko_types.h"
#include "interfaces/i_computebuffer.h"
#include "types/compute_pass.h"

namespace aiko::renderer::vulkan
{
    struct ReadbackRequest
    {
        ReadbackId id = InvalidReadbackId;

        AikoPtr<interfaces::IComputeBufferImpl> source;

        uint32_t byteSize = 0;
    };

    struct UploadArenaChunk
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        void* mapped = nullptr;

        VkDeviceSize capacity = 0;
        VkDeviceSize offset = 0;
    };

    struct UploadSlice
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceSize offset = 0;

        void* mapped = nullptr;
    };
}
