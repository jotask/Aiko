#pragma once

namespace aiko
{

    // Declaration order defines execution order.
    enum class SystemUpdatePhase
    {
        EarlyUpdate,
        Input,
        UIInteraction,
        Gameplay,
        Physics,
        LateUpdate,
    };

    // Declaration order defines execution order.
    enum class SystemRenderPhase
    {
        RenderPreparation,
        UI
    };

}
