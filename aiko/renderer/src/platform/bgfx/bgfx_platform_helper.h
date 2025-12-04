#pragma once

#include "types/color.h"

namespace aiko
{
	namespace bgfx
	{

		#define AIKO_DISABLE_CODE if constexpr(false)

		#define GET_BACKEND_IMPL(obj, Type, name) \
			Type* name = static_cast<Type*>(obj);\
			AIKO_ASSERT(name != nullptr, "Invalid native");

		#define AIKO_VSYNC_MACRO (false ? BGFX_RESET_VSYNC : BGFX_RESET_NONE)

		u32 convertColorToBgfx(Color c)
		{
			uint8_t red = static_cast<uint8_t>(c.r * 255.0f);
			uint8_t green = static_cast<uint8_t>(c.g * 255.0f);
			uint8_t blue = static_cast<uint8_t>(c.b * 255.0f);
			uint8_t alpha = static_cast<uint8_t>(c.a * 255.0f);
			uint32_t rgba = (red << 24) | (green << 16) | (blue << 8) | alpha;
			return rgba;
		}

	}
}