#pragma once

namespace aiko
{

	enum class EXIT_CODE
	{
		GLFW_INIT,
	};

	inline void exitWithCode(EXIT_CODE code) { std::exit(static_cast<int>(code)); }

}
