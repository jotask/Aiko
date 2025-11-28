#pragma once

#include "types/color.h"

#include <bgfx/bgfx.h>

namespace aiko
{
	namespace bgfx
	{

		#define AIKO_DISABLE_CODE if constexpr(false)

		#define AIKO_TO_VBH(idx) (::bgfx::VertexBufferHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_IBH(idx) (::bgfx::IndexBufferHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_PH(idx)  (::bgfx::ProgramHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_TH(idx) (::bgfx::TextureHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_UH(idx) (::bgfx::UniformHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_SH(idx) (::bgfx::ShaderHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_FBH(idx) (::bgfx::FrameBufferHandle{ (static_cast<uint16_t>(idx)) })
		#define AIKO_TO_VLH(idx) (::bgfx::VertexLayoutHandle{ (static_cast<uint16_t>(idx)) })

		u32 convertColorToBgfx(Color);

	}
}