#pragma once

#include "types/color.h"

#include <bgfx/bgfx.h>

namespace aiko
{
	namespace bgfx
	{

		#define AIKO_DISABLE_CODE if constexpr(false)

		u32 convertColorToBgfx(Color);

	}
}