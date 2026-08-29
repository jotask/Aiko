#include "render_regression.h"

#include <application/application.h>

#include <components/camera_component.h>
#include <components/mesh_component.h>

namespace regression
{

    void RenderRegression::init()
    {
        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);

        camera->transform().position = { 0.0f, 1.0f, 3.0f };
        cam->getCamera().position = camera->transform().position;

        auto cubeA = app->Instantiate("CubeA");
        cubeA->transform().position = { -0.35f, 0.0f, 0.0f };
        cubeA->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto meshA = cubeA->addComponent<aiko::MeshComponent>();
        meshA->loadDebugCube();

        auto cubeB = app->Instantiate("CubeB");
        cubeB->transform().position = { 0.35f, 0.0f, -0.75f };
        cubeB->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto meshB = cubeB->addComponent<aiko::MeshComponent>();
        meshB->loadDebugCube();

        meshA->getMaterial().baseColor = aiko::RED;
        meshB->getMaterial().baseColor = aiko::BLUE;

    }

    void RenderRegression::update()
    {

    }

    void RenderRegression::render()
    {

    }
}

