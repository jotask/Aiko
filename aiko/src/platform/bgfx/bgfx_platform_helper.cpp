#include "bgfx_platform_helper.h"

namespace aiko
{
	namespace bgfx
	{
		u32 convertColorToBgfx(float r, float g, float b, float a)
		{
			return u32();
		}

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