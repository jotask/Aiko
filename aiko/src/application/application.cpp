#include "application/application.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"


namespace aiko
{

    Application::Application()
        : m_aiko( std::make_unique<Aiko>( this ) )
    {
        m_aiko->run();
    }

    void Application::initEditor()
    {
        GameObject* camera = m_aiko->createGameObject();
        auto cam = camera->addComponent<CameraComponent>();

        GameObject* lights = m_aiko->createGameObject("Lights");
        auto light1 = lights->addComponent<LightComponent>();
        light1->setPrimitive(Light::Type::Ambient);
        light1->color = { 1.f, 1.f, 1.f, 1.f };

        auto light2 = lights->addComponent<LightComponent>();
        light2->setPrimitive(Light::Type::Point);
        light2->color = { 0.25f, 0.5f, 0.75f, 1.f };

        auto light3 = lights->addComponent<LightComponent>();
        light3->setPrimitive(Light::Type::Directional);
        light3->color = { 0.25f, 0.5f, 0.75f, 1.f };

        GameObject* ball = m_aiko->createGameObject("Sphere");
        ball->transform()->position = { 0.f, 0.f, 0.f };
        auto ballMesh = ball->addComponent<MeshComponent>();
        ballMesh->setPrimitive(Mesh::Sphere);
    
        GameObject* pyramid = m_aiko->createGameObject("Cone");
        pyramid->transform()->position = {1.1f, 0.f, 0.f};
        auto pyramidMesh = pyramid->addComponent<MeshComponent>();
        pyramidMesh->setPrimitive(Mesh::Cone);

        GameObject* cube = m_aiko->createGameObject("Cylinder");
        cube->transform()->position = {-1.1f, 0.f, 0.f};
        auto cubeMesh = cube->addComponent<MeshComponent>();
        cubeMesh->setPrimitive(Mesh::Cylinder);


    }

}
