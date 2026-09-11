#include "performance_smoke.h"

#include <aiko_config.h>
#include <application/application.h>

#include <stdlib.h>

int main()
{
    aiko::AikoConfig config{
        "Aiko Performance Smoke",
        1600,
        900
    };

    config.renderer.vsync = false;

    aiko::Application app{config};
    app.pushLayer(std::make_unique<aiko::perf::PerformanceSmoke>());
    app.run();

    return EXIT_SUCCESS;
}
