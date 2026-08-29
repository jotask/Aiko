#pragma once

namespace aiko
{

    class AssetSystem;
    class InputSystem;
    class PhysicsSystem;

    struct ComponentContext
    {
        AssetSystem* assets = nullptr;
        InputSystem* input = nullptr;
        PhysicsSystem* physics = nullptr;
    };
}
