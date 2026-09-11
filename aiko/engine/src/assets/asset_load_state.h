#pragma once

namespace aiko
{
    enum class AssetLoadState
    {
        Unloaded,
        Queued,
        Loading,
        Finalizing,
        Ready,
        Failed
    };
}
