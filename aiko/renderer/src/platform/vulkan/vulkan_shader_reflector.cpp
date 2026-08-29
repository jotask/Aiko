#include "vulkan_shader_reflector.h"

#include "vulkan_descriptor_abi.h"

#include <spirv_reflect.h>

#include <core/utils.h>
#include <math/math.h>

#include "vulkan_shader_reflection.h"

namespace aiko::renderer::vulkan
{
    namespace
    {
        VkDescriptorType toVkDescriptorType(SpvReflectDescriptorType type)
        {
            switch (type)
            {
                case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return VK_DESCRIPTOR_TYPE_SAMPLER;
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                default:
                    AIKO_ASSERT(false, "Unsupported reflected Vulkan descriptor type");
                    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
            }
        }

        void appendDescriptor(VulkanShaderReflection& reflection, const VulkanShaderDescriptorBinding& incoming)
        {
            for (VulkanShaderDescriptorBinding& existing : reflection.descriptorBindings)
            {
                if (existing.set != incoming.set || existing.binding != incoming.binding)
                {
                    continue;
                }
                AIKO_ASSERT(existing.descriptorType == incoming.descriptorType, "Shader stages disagree on descriptor type");
                AIKO_ASSERT(existing.descriptorCount == incoming.descriptorCount, "Shader stages disagree on descriptor count");
                existing.stageFlags |= incoming.stageFlags;
                return;
            }

            reflection.descriptorBindings.push_back(incoming);
        }

        void appendPushConstant(VulkanShaderReflection& reflection, const VulkanShaderPushConstantRange& incoming)
        {
            for (VulkanShaderPushConstantRange& existing : reflection.pushConstantRanges)
            {
                if (existing.offset != incoming.offset || existing.size != incoming.size)
                {
                    continue;
                }
                existing.stageFlags |= incoming.stageFlags;
                return;
            }

            reflection.pushConstantRanges.push_back(incoming);
        }
    }

    void reflectShaderSpirv(const std::vector<uint8_t>& code, VkShaderStageFlagBits stage, VulkanShaderReflection& reflection)
    {
        AIKO_ASSERT(code.empty() == false, "Cannot reflect empty SPIR-V");

        SpvReflectShaderModule module{};

        const SpvReflectResult createResult = spvReflectCreateShaderModule(code.size(), code.data(), &module);
        AIKO_ASSERT(createResult == SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect SPIR-V shader");

        uint32_t descriptorCount = 0;

        SpvReflectResult result = spvReflectEnumerateDescriptorBindings(&module, &descriptorCount, nullptr);
        AIKO_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "Failed to enumerate shader descriptors");

        std::vector<SpvReflectDescriptorBinding*> descriptors(descriptorCount);

        if (descriptorCount > 0)
        {
            result = spvReflectEnumerateDescriptorBindings(&module, &descriptorCount, descriptors.data());
            AIKO_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "Failed to read shader descriptors");
        }

        for (const SpvReflectDescriptorBinding* binding : descriptors)
        {
            AIKO_ASSERT(binding != nullptr, "Invalid reflected descriptor binding");

            appendDescriptor(
                reflection,
                {
                    .set = binding->set,
                    .binding = binding->binding,
                    .descriptorType =
                        toVkDescriptorType(
                            binding->descriptor_type),
                    .descriptorCount = binding->count,
                    .stageFlags = stage,
                });
        }

        uint32_t pushConstantCount = 0;

        result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, nullptr);

        AIKO_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "Failed to enumerate push constants");

        std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);

        if (pushConstantCount > 0)
        {
            result = spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, pushConstants.data());
            AIKO_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, "Failed to read push constants");
        }

        for (const SpvReflectBlockVariable* block : pushConstants)
        {
            AIKO_ASSERT(block != nullptr, "Invalid reflected push constant block");

            appendPushConstant(
                reflection,
                {
                    .offset = block->offset,
                    .size = block->size,
                    .stageFlags = stage,
                });
        }

        spvReflectDestroyShaderModule(&module);
    }


    void validateComputeShaderAbi(const VulkanShaderReflection& reflection)
    {
        for (const VulkanShaderDescriptorBinding& descriptor : reflection.descriptorBindings)
        {
            AIKO_ASSERT(descriptor.set == abi::ComputeSet, "Compute shader uses descriptor outside compute set");
            AIKO_ASSERT(descriptor.descriptorCount == 1, "Compute descriptor arrays are not currently supported");

            VkDescriptorType expectedType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

            if (descriptor.binding >= abi::ComputeBufferBindingBase && descriptor.binding < abi::ComputeImageBindingBase)
            {
                expectedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            }
            else if (descriptor.binding >= abi::ComputeImageBindingBase && descriptor.binding < abi::ComputeFrameBinding)
            {
                expectedType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            }
            else if (descriptor.binding == abi::ComputeFrameBinding)
            {
                expectedType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            }
            else
            {
                AIKO_ASSERT(false, "Compute shader binding is outside Vulkan compute ABI");
                continue;
            }

            AIKO_ASSERT(descriptor.descriptorType == expectedType, "Compute shader descriptor type does not match Vulkan ABI");
        }
    }

    void validateModelShaderAbi(const VulkanShaderReflection& reflection)
    {
        for (const VulkanShaderDescriptorBinding& descriptor : reflection.descriptorBindings)
        {
            AIKO_ASSERT(descriptor.descriptorCount == 1, "Graphics descriptor arrays are not currently supported");

            if (descriptor.set == abi::GraphicsFrameSet)
            {
                AIKO_ASSERT(descriptor.binding == abi::GraphicsFrameBinding, "Invalid graphics frame descriptor binding");
                AIKO_ASSERT(descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, "Graphics frame descriptor must be a uniform buffer");
                continue;
            }

            if (descriptor.set == abi::GraphicsMaterialSet)
            {
                if (descriptor.binding == abi::MaterialUboBinding)
                {
                    AIKO_ASSERT(descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, "Material UBO binding has invalid descriptor type");
                    continue;
                }

                if (descriptor.binding == abi::MaterialTextureBinding)
                {
                    AIKO_ASSERT(descriptor.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, "Material texture binding has invalid descriptor type");
                    continue;
                }

                AIKO_ASSERT(false, "Unknown material descriptor binding");

                continue;
            }

            if (descriptor.set == abi::GraphicsGpuReadSet)
            {
                AIKO_ASSERT(descriptor.binding < abi::MaxGpuReadBindings, "GPU-read descriptor exceeds Vulkan ABI limit");
                AIKO_ASSERT(descriptor.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, "GPU-read descriptor must be a storage buffer");
                continue;
            }

            AIKO_ASSERT(false, "Graphics shader uses descriptor set outside model ABI");
        }
    }

    void validateModelPushConstants(const VulkanShaderReflection& reflection)
    {
        AIKO_ASSERT(reflection.pushConstantRanges.size() <= 1, "Model shaders currently support one push constant range");
        if (reflection.pushConstantRanges.empty())
        {
            return;
        }
        const VulkanShaderPushConstantRange& range = reflection.pushConstantRanges[0];
        AIKO_ASSERT(range.offset == 0, "Model push constants must start at offset 0");
        AIKO_ASSERT(range.size == sizeof(mat4) * 2, "Model shader push constant size does not match renderer ABI");
        AIKO_ASSERT((range.stageFlags & (VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)) != 0, "Model push constants use invalid shader stages");
    }

    void validateComputePushConstants(const VulkanShaderReflection& reflection, uint32_t maxSize)
    {
        AIKO_ASSERT(reflection.pushConstantRanges.size() <= 1, "Compute shaders currently support one push constant range");
        if (reflection.pushConstantRanges.empty())
        {
            return;
        }
        const VulkanShaderPushConstantRange& range = reflection.pushConstantRanges[0];
        AIKO_ASSERT(range.offset == 0, "Compute push constants must start at offset 0");
        AIKO_ASSERT(range.size <= maxSize, "Compute shader push constants exceed Vulkan pipeline capacity");
        AIKO_ASSERT(range.stageFlags == VK_SHADER_STAGE_COMPUTE_BIT, "Compute push constants use invalid shader stage");
    }

    void validateScreenShaderAbi(const VulkanShaderReflection& reflection)
    {
        AIKO_ASSERT(reflection.descriptorBindings.size() <= 1, "Screen shader supports one descriptor");
        if (reflection.descriptorBindings.empty())
        {
            return;
        }
        const VulkanShaderDescriptorBinding& descriptor = reflection.descriptorBindings[0];
        AIKO_ASSERT(descriptor.set == 0, "Screen descriptor must use set 0");
        AIKO_ASSERT(descriptor.binding == 0, "Screen descriptor must use binding 0");
        AIKO_ASSERT(descriptor.descriptorCount == 1, "Screen descriptor arrays are not supported");
        AIKO_ASSERT(descriptor.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, "Screen descriptor must be a combined image sampler");
    }

    void validateScreenPushConstants(const VulkanShaderReflection& reflection)
    {
        AIKO_ASSERT(reflection.pushConstantRanges.size() <= 1, "Screen shader supports one push constant range");
        AIKO_ASSERT(reflection.pushConstantRanges.size() == 1, "Screen shader must declare its MVP push constant");
        const VulkanShaderPushConstantRange& range = reflection.pushConstantRanges[0];
        AIKO_ASSERT(range.offset == 0, "Screen push constants must start at offset 0");
        AIKO_ASSERT(range.size == sizeof(mat4), "Screen push constant size must match mat4");
        AIKO_ASSERT(range.stageFlags == VK_SHADER_STAGE_VERTEX_BIT, "Screen push constants must be vertex-stage only");
    }

}
