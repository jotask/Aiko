#include "layers/layer_context.h"

#include <time/time.h>

namespace aiko
{
    LayerContext::LayerContext(SystemConnector& connector)
        : m_input(connector)
        , m_render(connector)
        , m_scene(connector)
        , m_assets(connector)
    {

    }

    float LayerContext::getDeltaTime() const
    {
        return Time::it().getDeltaTime();
    }

}
