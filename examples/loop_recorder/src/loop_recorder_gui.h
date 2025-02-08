#pragma once

#include "application/application.h"

#include <vector>

class PaDeviceInfo;
class PaHostApiInfo;

namespace aiko::recorder
{

    class LoopRecorderGui : public Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;
    private:

        struct AudioDevices
        {
            const PaDeviceInfo* info;
            const PaHostApiInfo* host;
        };

        std::vector<AudioDevices> m_audioDevices;

    };

}

