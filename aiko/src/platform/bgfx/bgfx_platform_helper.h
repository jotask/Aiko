#pragma once

#include <bgfx/bgfx.h>

namespace aiko
{
	namespace bgfx
	{

		#define AIKO_DISABLE_CODE if constexpr(false)

		#define AIKO_TO_VBH(idx) (::bgfx::VertexBufferHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_IBH(idx) (::bgfx::IndexBufferHandle{ (static_cast<uint16_t>(idx)) })

		#define AIKO_TO_PH(idx)  (::bgfx::ProgramHandle{ (static_cast<uint16_t>(idx)) })

	}
}