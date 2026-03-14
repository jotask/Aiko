#pragma once

#include <map>

#include <aiko_types.h>

#include "constants.h"
#include "types/render_types.h"
#include <magic_enum/magic_enum.hpp>

namespace aiko::renderer
{
	namespace bgfx
	{
		static string getShaderRootDir()
		{
			std::string base = global::GLOBAL_PATH + "/build/assets/shaders/";

			switch (::bgfx::getRendererType())
			{
			case ::bgfx::RendererType::Direct3D11:
			case ::bgfx::RendererType::Direct3D12:  base += "dx11/"; break;
			case ::bgfx::RendererType::Metal:       base += "metal/"; break;
			case ::bgfx::RendererType::OpenGL:      base += "glsl/"; break;
			case ::bgfx::RendererType::OpenGLES:    base += "essl/"; break;
			case ::bgfx::RendererType::Vulkan:      base += "spirv/"; break;
			case ::bgfx::RendererType::Agc:
			case ::bgfx::RendererType::Gnm:         base += "pssl/"; break;
			case ::bgfx::RendererType::Nvn:         base += "nvn/"; break;
			case ::bgfx::RendererType::Noop:
			case ::bgfx::RendererType::Count:
			default:
				auto str = magic_enum::enum_name(::bgfx::getRendererType());
				logger::Log::critical("Unsupported BGFX renderer [%s]", str.data());
				AIKO_ASSERT(false, "Unsupported BGFX renderer");
			}

			return base;
		}

		static void dumpShaderUniforms(::bgfx::ShaderHandle shader, std::map<string, ::bgfx::UniformHandle>& m_uniforms, const char* str)
		{
			constexpr uint16_t MAX_UNIFORMS = 128;

			::bgfx::UniformHandle uniforms[MAX_UNIFORMS];
			uint16_t count = ::bgfx::getShaderUniforms(shader, uniforms, MAX_UNIFORMS);

			for (uint16_t i = 0; i < count; ++i)
			{
				::bgfx::UniformInfo info;
				::bgfx::getUniformInfo(uniforms[i], info);

				logger::Log::info("[%d] %s  -->  %s (type=%d, num=%d)", shader.idx, str, info.name, info.type, info.num);

				// Only create uniform if it doesn't exist
				if (m_uniforms.find(info.name) == m_uniforms.end())
				{
					::bgfx::UniformHandle u = ::bgfx::createUniform(info.name, info.type, info.num);
					m_uniforms.emplace(info.name, u);
				}

			}
		}

		static ::bgfx::Access::Enum toBgfxAccess(ComputeAccess access)
		{
			switch (access)
			{
			case ComputeAccess::Read:      return ::bgfx::Access::Read;
			case ComputeAccess::Write:     return ::bgfx::Access::Write;
			case ComputeAccess::ReadWrite: return ::bgfx::Access::ReadWrite;
			}
			AIKO_ASSERT(false, "Not supported compute access");
			return ::bgfx::Access::Read;
		}

	}
}
