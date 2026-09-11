#pragma once

namespace aiko
{
    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class DepthCompare
    {
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        Always
    };

    enum class FillMode
    {
        Solid,
        Wireframe,
        Point
    };

    struct RenderState
    {
        CullMode cullMode = CullMode::None;
        FillMode fillMode = FillMode::Solid;

        bool depthTest = true;
        bool depthWrite = true;
        DepthCompare depthCompare = DepthCompare::LessEqual;

        bool blend = false;
    };
}
