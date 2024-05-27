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
        using Windows = std::vector<AikoUPtr<Window>>;

        AikoEditor();
        aiko::Aiko* getAiko() const;

        const Windows& getWindows() { return m_windows; }

    protected:
        virtual void init() override;
        virtual void render() override;

        Windows m_windows;

    };

}

