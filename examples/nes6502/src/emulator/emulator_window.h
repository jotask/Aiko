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
        std::string getName() const { return name; };
        bool& getIsOpen() { return is_open; };
        void toggle() { is_open = !is_open; }
    protected:
        Naiko* naiko;
        bool is_open;
        const std::string name;

    };
}
