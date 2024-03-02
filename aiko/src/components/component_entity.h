#pragma once

namespace aiko
{

    class IUpdate
    {
    public:
        virtual void update() = 0;
    };

    class IRender
    {
    public:
        virtual void render() = 0;

    };

}
