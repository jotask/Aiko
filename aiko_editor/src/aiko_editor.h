#pragma once

#include <vector>

#include "aiko_types.h"
#include "application/application.h"

#include "windows/window.h"

namespace aiko::editor
{

    class AikoEditor : public aiko::Application
    {
    public:
        aiko::Aiko* getAiko() const;

    protected:
        virtual void init() override;
        virtual void render() override;

        using Windows = std::vector<AikoUPtr<Window>>;
        Windows m_windows;

    };

}

