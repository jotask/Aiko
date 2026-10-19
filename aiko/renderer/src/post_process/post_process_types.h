#pragma once

namespace aiko
{

    enum class PostProcessOrder : int
    {
        HDR_Early   = 100,
        HDR_Default = 200,
        HDR_Late    = 300,

        LDR_Early   = 400,
        LDR_Default = 500,
        LDR_Late    = 600
    };

}
