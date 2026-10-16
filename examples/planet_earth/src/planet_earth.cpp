#include "planet_earth.h"

#include "layers/contexts/scene_context.h"

namespace pe
{

    void PlanetEarth::connect(aiko::SystemConnector& systemConnector)
    {

    }

    void PlanetEarth::init()
    {

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 1.0f, 2.5f };

    }

    void PlanetEarth::update()
    {

    }

    void PlanetEarth::render()
    {

    }

}

