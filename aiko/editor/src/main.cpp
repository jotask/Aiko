#include <stdlib.h>

#include <application/application.h>
#include "aiko_editor.h"

int main()
{
    aiko::Application app;
    app.pushOverlay(std::make_unique<aiko::editor::AikoEditor>());
    app.run();
    return EXIT_SUCCESS;
}
