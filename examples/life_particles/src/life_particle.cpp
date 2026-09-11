#include "life_particle.h"

#include "layers/layer_context.h"

#include <components/camera_component.h>
#include <core/random.h>
#include <magic_enum/magic_enum.hpp>
#include <models/camera.h>
#include <models/game_object.h>

#include <aiko_includes.h>

namespace lp
{

    void LifeParticles::init()
    {
        auto camera = Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        camera->transform().position = { 0.0f, 1.0f, 3.0f };
        cam->getCamera().position = camera->transform().position;

        aiko::MeshAsset asset = aiko::mesh::factory::generateMeshSphere( 7, 7);
        m_mesh.upload(asset);

        m_material.m_shaderId = context().assets().loadShader("model");
        m_material.m_lit = false;
        m_material.m_useVertexColor = false;
        m_material.m_baseColor = aiko::RED;

        auto getRandomPosition = []() -> aiko::vec3
        {
            const float SIZE = 100.0f;
            return {aiko::utils::getRandomValue(-SIZE, SIZE), aiko::utils::getRandomValue(-SIZE, SIZE), aiko::utils::getRandomValue(-SIZE, SIZE)};
        };

        auto getRandomType = []() -> ParticleType
        {
            auto rnd = aiko::utils::getRandomValue(0, magic_enum::enum_count<ParticleType>() - 1);
            return magic_enum::enum_value<ParticleType>(rnd);
        };

        for (uint64_t i = 0 ; i < c_particles_amount; ++i)
        {
            m_particles[i].position = getRandomPosition();
            m_particles[i].velocity = {0.0f, 0.0f, 0.0f};
            m_particles[i].type = getRandomType();
        }

    }

    void LifeParticles::update()
    {
        for (Particle& lp : m_particles)
        {

        }
    }

    void LifeParticles::render()
    {
        for (Particle& lp : m_particles)
        {
            aiko::Transform trans;
            trans.position = lp.position;
            context().render().drawMesh(trans, m_mesh, m_material);
        }
    }

}

