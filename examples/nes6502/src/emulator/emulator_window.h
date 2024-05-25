#pragma once

#include <string>

namespace nes
{
    class Naiko;
    class EmulatorWindow
    {
    public:
        EmulatorWindow(Naiko* naiko, std::string);
        virtual ~EmulatorWindow() = default;
        virtual void update() = 0;
        virtual void render() = 0;
    protected:
        Naiko* naiko;
        bool is_open;
        const std::string name;

    };
}
