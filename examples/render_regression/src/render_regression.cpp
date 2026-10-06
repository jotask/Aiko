#include "render_regression.h"

#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>

namespace regression
{

    void RenderRegression::init()
    {

        aiko::CameraComponent* camera = scene().createCamera();
        camera->getCamera().position = { 0.0f, 1.0f, 3.0f };

        const aiko::MeshAsset defaultCube = aiko::mesh::factory::generateCube();

        aiko::GameObject* cubeA = Instantiate("CubeA");
        cubeA->transform().position = { -0.35f, 0.0f, 0.0f };
        cubeA->transform().scale = { 1.0f, 1.0f, 1.0f };

        aiko::MeshComponent* meshA = cubeA->addComponent<aiko::MeshComponent>();
        meshA->load(defaultCube);
        meshA->getMaterial().m_baseColor = aiko::RED;

        aiko::GameObject* cubeB = Instantiate("CubeB");
        cubeB->transform().position = { 0.35f, 0.0f, -0.75f };
        cubeB->transform().scale = { 1.0f, 1.0f, 1.0f };

        aiko::MeshComponent* meshB = cubeB->addComponent<aiko::MeshComponent>();
        meshB->load(defaultCube);
        meshB->getMaterial().m_baseColor = aiko::BLUE;


    }

}

