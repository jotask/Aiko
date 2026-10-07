#pragma once

#include <aiko_types.h>

namespace aiko
{

    struct UIPadding
    {
        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;
    };

    enum class UICrossAxisAlignment
    {
        Start,
        Center,
        End
    };

}
