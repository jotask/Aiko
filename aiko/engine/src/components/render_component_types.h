#pragma once

namespace aiko
{
    
    class IRender
    {
    public:
        virtual void render() = 0;
    };
    
    class IUpdatable
    {
    public:
        virtual void update() = 0;
    };

    }
