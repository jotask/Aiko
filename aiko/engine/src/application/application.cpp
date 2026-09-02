#include "application/application.h"

#include "aiko.h"
#include "layers/layer_stack.h"

namespace aiko
{

    Application::~Application() = default;

    Application::Application()
        : m_aiko(std::make_unique<Aiko>(this))
        , m_layers(std::make_unique<LayerStack>())
    {

    }

    Application::Application(AikoConfig cfg)
        : m_aiko(std::make_unique<Aiko>(this, cfg))
        , m_layers(std::make_unique<LayerStack>())
    {

    }

    void Application::run()
    {
        m_aiko->run();
    }

    void Application::registerSystems(SystemRegistry& registry)
    {
        for (auto& layer : *m_layers)
        {
            layer->registerSystems(registry);
        }
    }

    void Application::init()
    {
        for (auto& layer : *m_layers)
        {
            layer->init();
        }
    }

    void Application::update()
    {
        for (auto& layer : *m_layers)
        {
            layer->update();
        }
    }

    void Application::render()
    {
        for (auto& layer : *m_layers)
        {
            layer->render();
        }
    }

    void Application::dispose()
    {
        for (auto& layer : *m_layers)
        {
            layer->dispose();
        }
    }

    void Application::pushLayer(AikoUPtr<Layer> layer)
    {
        m_layers->pushLayer(std::move(layer));
    }

    void Application::pushOverlay(AikoUPtr<Layer> layer)
    {
        m_layers->pushOverlay(std::move(layer));
    }

    void Application::connect(SystemConnector& connector, LayerContext& context)
    {
        for (auto& layer : *m_layers)
        {
            layer->m_context = &context;
            layer->connect(connector);
        }
    }

    void Application::onEvent(Event& e)
    {
        for (auto it = m_layers->rbegin(); it != m_layers->rend(); ++it)
        {
            (*it)->onEvent(e);
            if (e.handled == true)
            {
                break;
            }
        }
    }
}
