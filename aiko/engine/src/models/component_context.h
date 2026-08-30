#pragma once

namespace aiko
{

    class AssetSystem;
    class InputSystem;

    struct ComponentContext
    {
        AssetSystem* assets = nullptr;
        InputSystem* input = nullptr;
    };
}
