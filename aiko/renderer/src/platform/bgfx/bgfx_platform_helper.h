#pragma once

#include <map>

#include <aiko_types.h>

#include "constants.h"
#include "types/render_types.h"

#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>

#if defined(AIKO_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
	#define GLFW_EXPOSE_NATIVE_WAYLAND
	#include <GLFW/glfw3native.h>
#elif defined(AIKO_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif

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
				const auto renderer = ::bgfx::getRendererName(::bgfx::getRendererType());
				logger::Log::critical("Unsupported BGFX renderer [%s]", renderer);
				AIKO_ASSERT(false, "Unsupported BGFX renderer");
			}

			return base;
		}

		static void dumpShaderUniforms(::bgfx::ShaderHandle shader, std::unordered_map<string, ::bgfx::UniformHandle>& m_uniforms, const char* str)
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

		struct BgfxPlatformSetupResult
		{
			bool ok = false;
			::bgfx::RendererType::Enum preferredRenderer = ::bgfx::RendererType::Count;
		};

		inline BgfxPlatformSetupResult setupBgfxPlatformData(::bgfx::Init& init, GLFWwindow* window)
		{
			BgfxPlatformSetupResult result{};

			if (window == nullptr)
			{
				return result;
			}

			#if defined(AIKO_WINDOWS)
			init.platformData.nwh = glfwGetWin32Window(window);
			init.platformData.ndt = nullptr;
			result.preferredRenderer = ::bgfx::RendererType::Direct3D12;
			result.ok = (init.platformData.nwh != nullptr);
			return result;
			#elif defined(AIKO_LINUX)

			const int platform = glfwGetPlatform();

			if (platform == GLFW_PLATFORM_X11)
			{
				init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
				init.platformData.ndt = glfwGetX11Display();
				init.platformData.type = ::bgfx::NativeWindowHandleType::Default;
				result.preferredRenderer = ::bgfx::RendererType::Vulkan;
				result.ok = (init.platformData.nwh != nullptr && init.platformData.ndt != nullptr);
				return result;
			}

			if (platform == GLFW_PLATFORM_WAYLAND)
			{
				init.platformData.nwh = glfwGetWaylandWindow(window);
				init.platformData.ndt = glfwGetWaylandDisplay();
				init.platformData.type = ::bgfx::NativeWindowHandleType::Wayland;
				// Safer fallback for now.
				result.preferredRenderer = ::bgfx::RendererType::OpenGL;
				result.ok = (init.platformData.nwh != nullptr && init.platformData.ndt != nullptr);
				return result;
			}

			return result;

#else
			#error Not supported platform
			return result;
#endif
		}

	}
}
