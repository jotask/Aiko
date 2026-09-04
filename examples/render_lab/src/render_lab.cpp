#include "render_lab.h"

#include "components/camera_component.h"
#include "components/compute_shader_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/model_component.h"
#include "components/particle_emitter_component.h"
#include "components/sprite_component.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"

#include "models/camera.h"
#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "models/texture_factory.h"

#include "systems/render_system.h"
#include "systems/system_connector.h"

#include "types/color.h"
#include "types/render_state.h"

#include <core/random.h>

#include <chrono>
#include <cmath>

namespace aiko::lab
{
    namespace
    {
        constexpr bool EnableModelTests = true;
        constexpr bool EnableComponentTests = true;
        constexpr bool EnablePrimitiveTests = true;
        constexpr bool EnableMaterialTests = true;
        constexpr bool EnableRenderStateTests = true;
        constexpr bool EnableRuntimeTextureTests = true;
        constexpr bool EnableInstancingTests = true;
        constexpr bool EnableLightTests = true;
        constexpr bool EnableParticleTests = true;
        constexpr bool EnableComputeTests = true;
    }

    void RenderLab::connect(SystemConnector& systemConnector)
    {
        BIND_SYSTEM_REQUIRED_REF(RenderSystem, systemConnector, m_renderSystem);
    }

    void RenderLab::init()
    {
        initCamera();

        if constexpr (EnableModelTests)
        {
            initModels();
        }

        if constexpr (EnableComponentTests)
        {
            initComponents();
        }

        if constexpr (EnableMaterialTests || EnableRenderStateTests)
        {
            initMaterials();
        }

        if constexpr (EnableRuntimeTextureTests)
        {
            initRuntimeTextures();
        }

        if constexpr (EnableInstancingTests)
        {
            initInstancing();
        }

        if constexpr (EnableLightTests)
        {
            initLights();
        }

        if constexpr (EnableParticleTests)
        {
            initParticles();
        }

        if constexpr (EnableComputeTests)
        {
            initCompute();
        }
    }

    void RenderLab::update()
    {
        if constexpr (EnableComponentTests)
        {
            updateComponents();
        }

        if constexpr (EnableRuntimeTextureTests)
        {
            updateRuntimeTexture();
        }

        if constexpr (EnableLightTests)
        {
            updateLights();
        }

        if constexpr (EnableComputeTests)
        {
            updateCompute();
        }
    }

    void RenderLab::render()
    {
        if constexpr (EnablePrimitiveTests)
        {
            renderPrimitives();
        }

        if constexpr (EnableMaterialTests)
        {
            renderMaterialTests();
        }

        if constexpr (EnableRenderStateTests)
        {
            renderRenderStateTests();
        }

        if constexpr (EnableRuntimeTextureTests)
        {
            renderRuntimeTextureTests();
        }

        if constexpr (EnableInstancingTests)
        {
            renderInstancing();
        }

        if constexpr (EnableLightTests)
        {
            renderLights();
        }
    }

    // --------------------------------------------------
    // Camera
    // --------------------------------------------------

    void RenderLab::initCamera()
    {
        GameObject* camera = Instantiate("Camera");

        CameraComponent* cameraComponent =
            camera->addComponent<CameraComponent>(
                camera::CameraController::Fly
            );

        camera->transform().position =
        {
            0.0f,
            2.5f,
            8.0f
        };

        cameraComponent->getCamera().position =
            camera->transform().position;
    }

    // --------------------------------------------------
    // Imported model / asset validation
    // --------------------------------------------------

    void RenderLab::initModels()
    {
        GameObject* root = Instantiate("Models");

        const float off = 30.0f;

        GameObject* church = Instantiate(root, "Church");
        church->transform().position = {off, 0.0f, -28.0f};

        ModelComponent* churchModel =
            church->addComponent<ModelComponent>();

        churchModel->load("church.obj");


        GameObject* barracks = Instantiate(root, "Barracks");
        barracks->transform().position = {0.0f, 0.0f, -28.0f};

        ModelComponent* barracksModel =
            barracks->addComponent<ModelComponent>();

        barracksModel->load("barracks.obj");


        GameObject* watermill = Instantiate(root, "Watermill");
        watermill->transform().position = {off, 0.0f, -28.0f};

        ModelComponent* watermillModel =
            watermill->addComponent<ModelComponent>();

        watermillModel->load("watermill.obj");


        GameObject* robot = Instantiate(root, "Robot");
        robot->transform().position = {0.0f, 0.0f, -10.0f};

        constexpr float RobotScale = 0.25f;

        robot->transform().scale =
        {
            RobotScale,
            RobotScale,
            RobotScale
        };

        ModelComponent* robotModel =
            robot->addComponent<ModelComponent>();

        robotModel->load("robot.glb");
    }

    // --------------------------------------------------
    // Standard engine components
    // --------------------------------------------------

    void RenderLab::initComponents()
    {
        GameObject* root = Instantiate("Components");

        const MeshAsset cube =
            mesh::factory::generateCube();

        m_cubeA = Instantiate(root, "CubeA");
        m_cubeA->transform().position =
        {
            -1.5f,
            0.0f,
            1.5f
        };

        MeshComponent* meshA =
            m_cubeA->addComponent<MeshComponent>();

        meshA->load(cube);

        meshA->getMaterial().m_baseColor = YELLOW;


        m_cubeB = Instantiate(root, "CubeB");
        m_cubeB->transform().position =
        {
            1.5f,
            0.0f,
            1.5f
        };

        MeshComponent* meshB =
            m_cubeB->addComponent<MeshComponent>();

        meshB->load(cube);

        meshB->getMaterial().m_baseColor = BLUE;


        GameObject* assetTexture =
            Instantiate(root, "AssetTexture");

        assetTexture->transform().position =
        {
            -1.25f,
            -1.5f,
            1.0f
        };

        SpriteComponent* assetSprite =
            assetTexture->addComponent<SpriteComponent>();

        assetSprite->load("texel_checker.png");

        assetSprite->getMaterial().m_lit = false;


        m_dynamicTextureObject =
            Instantiate(root, "DynamicTexture");

        m_dynamicTextureObject->transform().position =
        {
            1.25f,
            -1.5f,
            1.0f
        };

        SpriteComponent* dynamicSprite =
            m_dynamicTextureObject->addComponent<SpriteComponent>();

        dynamicSprite->load(
            texture::factory::generateBlank(
                128,
                128,
                BLACK
            )
        );

        Material& material =
            dynamicSprite->getMaterial();

        material.m_lit = false;

        const SamplerState nearestSampler =
        {
            .minFilter = TextureFilter::Nearest,
            .magFilter = TextureFilter::Nearest,
            .mipFilter = TextureMipFilter::None,
        };

        material.setTextureSampler(
            "u_texture",
            nearestSampler
        );
    }

    // --------------------------------------------------
    // Materials / uniforms / render states
    // --------------------------------------------------

    void RenderLab::initMaterials()
    {
        const AssetId modelShader =
            assets().loadShader("model");

        // --------------------------------------------------
        // Basic material
        // --------------------------------------------------

        m_defaultMaterial.m_shaderId = modelShader;
        m_defaultMaterial.m_baseColor = WHITE;
        m_defaultMaterial.m_lit = false;
        m_defaultMaterial.m_useVertexColor = false;


        // --------------------------------------------------
        // Typed uniform storage
        // --------------------------------------------------

        m_uniformMaterial.m_shaderId = modelShader;
        m_uniformMaterial.m_baseColor = WHITE;
        m_uniformMaterial.m_lit = false;
        m_uniformMaterial.m_useVertexColor = false;

        m_uniformMaterial.setBool(
            "u_labBool",
            true
        );

        m_uniformMaterial.setInt(
            "u_labInt",
            -42
        );

        m_uniformMaterial.setUInt(
            "u_labUInt",
            42u
        );

        m_uniformMaterial.setFloat(
            "u_labFloat",
            0.5f
        );

        m_uniformMaterial.setVec2(
            "u_labVec2",
            vec2{0.25f, 0.75f}
        );

        m_uniformMaterial.setVec3(
            "u_labVec3",
            vec3{1.0f, 0.5f, 0.25f}
        );

        m_uniformMaterial.setVec4(
            "u_labVec4",
            vec4{1.0f, 0.5f, 0.25f, 1.0f}
        );

        m_uniformMaterial.setMat4(
            "u_labMat4",
            mat4(1.0f)
        );


        // --------------------------------------------------
        // Fill modes
        // --------------------------------------------------

        m_solidMaterial.m_shaderId = modelShader;
        m_solidMaterial.m_baseColor = GREEN;
        m_solidMaterial.m_lit = false;

        m_solidMaterial.m_renderState.fillMode =
            FillMode::Solid;


        m_wireframeMaterial.m_shaderId = modelShader;
        m_wireframeMaterial.m_baseColor = YELLOW;
        m_wireframeMaterial.m_lit = false;

        m_wireframeMaterial.m_renderState.fillMode =
            FillMode::Wireframe;


        m_pointMaterial.m_shaderId = modelShader;
        m_pointMaterial.m_baseColor = MAGENTA;
        m_pointMaterial.m_lit = false;

        m_pointMaterial.m_renderState.fillMode =
            FillMode::Point;


        // --------------------------------------------------
        // Culling
        // --------------------------------------------------

        m_cullNoneMaterial.m_shaderId = modelShader;
        m_cullNoneMaterial.m_baseColor = WHITE;
        m_cullNoneMaterial.m_lit = false;

        m_cullNoneMaterial.m_renderState.cullMode =
            CullMode::None;


        m_cullFrontMaterial.m_shaderId = modelShader;
        m_cullFrontMaterial.m_baseColor = RED;
        m_cullFrontMaterial.m_lit = false;

        m_cullFrontMaterial.m_renderState.cullMode =
            CullMode::Front;


        m_cullBackMaterial.m_shaderId = modelShader;
        m_cullBackMaterial.m_baseColor = BLUE;
        m_cullBackMaterial.m_lit = false;

        m_cullBackMaterial.m_renderState.cullMode =
            CullMode::Back;


        // --------------------------------------------------
        // Depth state
        // --------------------------------------------------

        m_depthMaterial.m_shaderId = modelShader;
        m_depthMaterial.m_baseColor = GREEN;
        m_depthMaterial.m_lit = false;

        m_depthMaterial.m_renderState.depthTest = true;
        m_depthMaterial.m_renderState.depthWrite = true;
        m_depthMaterial.m_renderState.depthCompare =
            DepthCompare::LessEqual;


        m_noDepthMaterial.m_shaderId = modelShader;
        m_noDepthMaterial.m_baseColor = MAGENTA;
        m_noDepthMaterial.m_lit = false;

        m_noDepthMaterial.m_renderState.depthTest = false;
        m_noDepthMaterial.m_renderState.depthWrite = false;


        // --------------------------------------------------
        // Blend state
        // --------------------------------------------------

        m_blendMaterial.m_shaderId = modelShader;
        m_blendMaterial.m_baseColor = WHITE;
        m_blendMaterial.m_lit = false;

        m_blendMaterial.m_renderState.blend = true;
    }

    // --------------------------------------------------
    // Runtime textures + named bindings + samplers
    // --------------------------------------------------

    void RenderLab::initRuntimeTextures()
    {
        m_runtimeTextureA.upload(
            texture::factory::generateBlank(
                64,
                64,
                RED
            )
        );

        m_runtimeTextureB.upload(
            texture::factory::generateBlank(
                64,
                64,
                BLUE
            )
        );

        m_runtimeTextureMesh.upload(
            mesh::factory::generateQuad()
        );


        const AssetId modelShader =
            assets().loadShader("model");


        const SamplerState nearestSampler =
        {
            .minFilter = TextureFilter::Nearest,
            .magFilter = TextureFilter::Nearest,
            .mipFilter = TextureMipFilter::None,
        };

        const SamplerState linearSampler =
        {
            .minFilter = TextureFilter::Linear,
            .magFilter = TextureFilter::Linear,
            .mipFilter = TextureMipFilter::Linear,
        };


        // --------------------------------------------------
        // Direct runtime Texture*
        // --------------------------------------------------

        m_runtimeTextureMaterial.m_shaderId = modelShader;
        m_runtimeTextureMaterial.m_lit = false;
        m_runtimeTextureMaterial.m_useVertexColor = false;
        m_runtimeTextureMaterial.m_baseColor = WHITE;

        m_runtimeTextureMaterial.setTexture(
            "u_texture",
            &m_runtimeTextureA,
            nearestSampler
        );


        // --------------------------------------------------
        // Multiple named texture bindings
        //
        // This validates Material's named-binding storage,
        // sampler persistence and Vulkan material-key path.
        //
        // A shader must actually declare both descriptors
        // before both become visible in the final fragment.
        // --------------------------------------------------

        m_multiTextureMaterial.m_shaderId = modelShader;
        m_multiTextureMaterial.m_lit = false;
        m_multiTextureMaterial.m_useVertexColor = false;
        m_multiTextureMaterial.m_baseColor = WHITE;

        m_multiTextureMaterial.setTexture(
            "u_texture",
            &m_runtimeTextureA,
            nearestSampler
        );

        m_multiTextureMaterial.setTexture(
            "u_secondaryTexture",
            &m_runtimeTextureB,
            linearSampler
        );

        // Verify sampler-before-texture semantics too.
        m_multiTextureMaterial.setTextureSampler(
            "u_futureTexture",
            nearestSampler
        );

        m_multiTextureMaterial.setTexture(
            "u_futureTexture",
            &m_runtimeTextureB
        );
    }

    // --------------------------------------------------
    // CPU instancing
    // --------------------------------------------------

    void RenderLab::initInstancing()
    {
        m_instancingMesh.upload(
            mesh::factory::generateCube()
        );

        m_instancingMaterial.m_shaderId =
            assets().loadShader("model");

        m_instancingMaterial.m_useVertexColor = true;
        m_instancingMaterial.m_lit = false;
        m_instancingMaterial.m_baseColor = WHITE;
    }

    // --------------------------------------------------
    // Lighting
    // --------------------------------------------------

    void RenderLab::initLights()
    {
        GameObject* root =
            Instantiate("Lights");

        constexpr float SpawnRadius = 6.0f;
        constexpr size_t LightCount = 8;

        for (size_t i = 0; i < LightCount; ++i)
        {
            GameObject* object =
                Instantiate(root, "PointLight");

            object->transform().position =
            {
                utils::getRandomValue(
                    -SpawnRadius,
                    SpawnRadius
                ),

                utils::getRandomValue(
                    0.0f,
                    SpawnRadius
                ),

                utils::getRandomValue(
                    -SpawnRadius,
                    SpawnRadius
                ),
            };

            LightComponent* light =
                object->addComponent<LightComponent>();

            light->setPointLight(
                Color::getRandomColor(),
                1.0f
            );

            LightInstance instance;
            instance.object = object;
            instance.component = light;
            instance.angle =
                utils::getRandomValue(
                    0.0f,
                    360.0f
                );

            m_lights.push_back(instance);
        }
    }

    // --------------------------------------------------
    // GPU particle / billboard path
    // --------------------------------------------------

    void RenderLab::initParticles()
    {
        GameObject* root =
            Instantiate("Particles");

        GameObject* object =
            Instantiate(root, "ParticleEmitter");

        object->transform().position =
        {
            0.0f,
            0.0f,
            -4.0f
        };

        ParticleEmitterComponent* emitter =
            object->addComponent<ParticleEmitterComponent>();

        emitter->setMaxParticles(1024 * 64);

        emitter->setLifetime(2.0f);

        emitter->setDirection(
            {0.0f, 1.0f, 0.0f}
        );

        emitter->setDirectionRandomness(0.8f);

        emitter->setGravity(
            {0.0f, -1.5f, 0.0f}
        );

        emitter->setStartSpeed(2.0f);

        emitter->setSpawnRate(10000.0f);

        emitter->setSpawnShape(
            ParticleEmitterComponent::ParticleSpawnShape::Point
        );

        emitter->setSpawnRadius(1.0f);

        emitter->setStartLifeColor(YELLOW);
        emitter->setEndLifeColor(RED);

        emitter->setStartLifeScale(0.05f);
        emitter->setEndLifeScale(0.0f);

        emitter->setPlaying(true);
        emitter->requestReset();
    }

    // --------------------------------------------------
    // Compute
    // --------------------------------------------------

    void RenderLab::initCompute()
    {
        GameObject* root =
            Instantiate("Compute");

        // --------------------------------------------------
        // Compute -> texture -> render
        // --------------------------------------------------

        GameObject* computeTexture =
            Instantiate(root, "ComputeTexture");

        computeTexture->transform().position =
        {
            0.0f,
            2.5f,
            0.0f
        };

        ComputeShaderComponent* compute =
            computeTexture->addComponent<ComputeShaderComponent>();

        compute->load("gradient");

        compute->setUseOutputTexture(true);

        compute->setOutputSize(
            512,
            512
        );

        compute->setExecutionMode(
            ComputeShaderComponent::ComputeExecutionMode::Continuous
        );

        compute->setUpdateInterval(0.0f);


        SpriteComponent* sprite =
            computeTexture->addComponent<SpriteComponent>();

        sprite->load(
            texture::factory::generateBlank(
                512,
                512,
                BLACK
            )
        );

        sprite->getMaterial().m_lit = false;
        sprite->getMaterial().m_useVertexColor = false;


        // --------------------------------------------------
        // Compute -> GPU buffer -> CPU readback
        // --------------------------------------------------

        GameObject* readbackObject =
            Instantiate(root, "ComputeReadback");

        m_computeReadback =
            readbackObject->addComponent<ComputeShaderComponent>();

        m_computeReadback->load("compute_test");

        m_computeReadback->setElementCount(64);

        m_computeReadback->setExecutionMode(
            ComputeShaderComponent::ComputeExecutionMode::Once
        );
    }

    // --------------------------------------------------
    // Updates
    // --------------------------------------------------

    void RenderLab::updateComponents()
    {
        static float angle = 0.0f;

        angle +=
            25.0f *
            getDeltaTime();

        angle =
            std::fmod(
                angle,
                360.0f
            );

        m_cubeA->transform().rotation =
        {
            angle,
            angle * 0.5f,
            0.0f
        };

        m_cubeB->transform().rotation =
        {
            -angle,
            angle,
            0.0f
        };
    }

    void RenderLab::updateRuntimeTexture()
    {
        if (m_dynamicTextureObject == nullptr)
        {
            return;
        }

        SpriteComponent* sprite =
            m_dynamicTextureObject
                ->getComponent<SpriteComponent>();

        if (sprite == nullptr)
        {
            return;
        }

        static constexpr double UpdateInterval =
            1.0 / 60.0;

        static auto previousTime =
            std::chrono::steady_clock::now();

        static double accumulatedTime = 0.0;

        const auto currentTime =
            std::chrono::steady_clock::now();

        const std::chrono::duration<double> delta =
            currentTime - previousTime;

        previousTime = currentTime;

        accumulatedTime += delta.count();

        if (accumulatedTime < UpdateInterval)
        {
            return;
        }

        accumulatedTime -= UpdateInterval;


        struct Particle
        {
            ivec2 position;
            ivec2 direction;
            Color color;
        };

        constexpr int Width = 128;
        constexpr int Height = 128;
        constexpr int MaxX = Width - 1;
        constexpr int MaxY = Height - 1;

        constexpr size_t ParticleCount = 100;

        static std::vector<Particle> particles;

        if (particles.empty())
        {
            particles.reserve(ParticleCount);

            for (size_t i = 0; i < ParticleCount; ++i)
            {
                Particle particle;

                particle.position =
                {
                    utils::getRandomValue(
                        0,
                        MaxX
                    ),

                    utils::getRandomValue(
                        0,
                        MaxY
                    )
                };

                particle.direction =
                {
                    utils::getRandomValue(
                        -1,
                        1
                    ),

                    utils::getRandomValue(
                        -1,
                        1
                    )
                };

                if (
                    particle.direction.x == 0 &&
                    particle.direction.y == 0
                )
                {
                    particle.direction.x = 1;
                }

                particle.color =
                    Color::getRandomColor();

                particles.push_back(particle);
            }
        }

        for (Particle& particle : particles)
        {
            if (
                particle.position.x <= 0 ||
                particle.position.x >= MaxX
            )
            {
                particle.direction.x *= -1;
            }

            if (
                particle.position.y <= 0 ||
                particle.position.y >= MaxY
            )
            {
                particle.direction.y *= -1;
            }

            particle.position.x += particle.direction.x;
            particle.position.y += particle.direction.y;

            particle.position.x =
                math::clamp(
                    particle.position.x,
                    0,
                    MaxX
                );

            particle.position.y =
                math::clamp(
                    particle.position.y,
                    0,
                    MaxY
                );

            sprite->setPixel(
                particle.position.x,
                particle.position.y,
                particle.color
            );
        }

        sprite->refresh();
    }

    void RenderLab::updateLights()
    {
        constexpr float RotationSpeed = 0.5f;
        constexpr float Radius = 4.0f;

        for (LightInstance& light : m_lights)
        {
            light.angle +=
                RotationSpeed *
                getDeltaTime();

            light.object->transform().position =
            {
                std::sin(light.angle) * Radius,
                1.5f,
                std::cos(light.angle) * Radius
            };
        }
    }

    void RenderLab::updateCompute()
    {
        if (
            m_computeReadback == nullptr ||
            m_computeReadbackPrinted ||
            !m_computeReadback->hasReadback()
        )
        {
            return;
        }

        const ComputeReadbackResult& result =
            m_computeReadback->getLastReadback();

        constexpr uint32_t ElementCount = 64;

        constexpr size_t ExpectedSize =
            ElementCount *
            sizeof(vec4);

        AIKO_ASSERT(
            result.data.size() == ExpectedSize,
            "Invalid compute readback size"
        );

        const vec4* values =
            reinterpret_cast<const vec4*>(
                result.data.data()
            );

        for (
            uint32_t i = 0;
            i < ElementCount;
            ++i
        )
        {
            logger::Log::info(
                "Compute[{%zu}] = ({%.2f}, {%.2f}, {%.2f}, {%.2f})",
                i,
                values[i].x,
                values[i].y,
                values[i].z,
                values[i].w
            );
        }

        m_computeReadbackPrinted = true;
    }

    // --------------------------------------------------
    // Primitive path
    // --------------------------------------------------

    void RenderLab::renderPrimitives()
    {
        constexpr float Size = 0.5f;

        m_renderSystem->renderPoint(
            {-4.0f, 3.0f, 0.0f}
        );

        m_renderSystem->renderLine(
            {-4.0f, 2.5f, 0.0f},
            {-2.0f, 2.5f, 0.0f}
        );

        m_renderSystem->renderTriangle(
            {-4.0f, 2.0f, 0.0f},
            Size
        );

        m_renderSystem->renderRectangle(
            {-3.0f, 2.0f, 0.0f},
            Size
        );

        m_renderSystem->renderCircle(
            {-2.0f, 2.0f, 0.0f},
            Size,
            32
        );

        m_renderSystem->renderNgon(
            {-1.0f, 2.0f, 0.0f},
            Size,
            6
        );

        m_renderSystem->renderCube(
            {-4.0f, 1.0f, 0.0f},
            Size
        );

        m_renderSystem->renderPyramid(
            {-3.0f, 1.0f, 0.0f},
            Size
        );

        m_renderSystem->renderSphere(
            {-2.0f, 1.0f, 0.0f},
            Size
        );

        m_renderSystem->renderCylinder(
            {-1.0f, 1.0f, 0.0f},
            Size,
            16
        );

        m_renderSystem->renderTorus(
            {-4.0f, 0.0f, 0.0f},
            Size
        );

        m_renderSystem->renderKnot(
            {-3.0f, 0.0f, 0.0f},
            Size
        );

        m_renderSystem->renderGrid(
            {0.0f, -2.5f, 0.0f},
            0.5f,
            {10, 10}
        );
    }

    // --------------------------------------------------
    // Material + uniform path
    // --------------------------------------------------

    void RenderLab::renderMaterialTests()
    {
        m_renderSystem->renderCube(
            {-5.0f, 0.0f, -5.0f},
            0.75f,
            &m_defaultMaterial
        );

        m_renderSystem->renderCube(
            {-3.0f, 0.0f, -5.0f},
            0.75f,
            &m_uniformMaterial
        );
    }

    // --------------------------------------------------
    // Render state pipeline variants
    // --------------------------------------------------

    void RenderLab::renderRenderStateTests()
    {
        // Fill modes

        m_renderSystem->renderCube(
            {-5.0f, 2.0f, -7.0f},
            0.75f,
            &m_solidMaterial
        );

        m_renderSystem->renderCube(
            {-3.0f, 2.0f, -7.0f},
            0.75f,
            &m_wireframeMaterial
        );

        m_renderSystem->renderCube(
            {-1.0f, 2.0f, -7.0f},
            0.75f,
            &m_pointMaterial
        );


        // Culling

        m_renderSystem->renderCube(
            {1.0f, 2.0f, -7.0f},
            0.75f,
            &m_cullNoneMaterial
        );

        m_renderSystem->renderCube(
            {3.0f, 2.0f, -7.0f},
            0.75f,
            &m_cullFrontMaterial
        );

        m_renderSystem->renderCube(
            {5.0f, 2.0f, -7.0f},
            0.75f,
            &m_cullBackMaterial
        );


        // Depth test

        m_renderSystem->renderCube(
            {-1.0f, 0.0f, -6.0f},
            1.0f,
            &m_depthMaterial
        );

        m_renderSystem->renderCube(
            {-0.75f, 0.25f, -6.5f},
            0.75f,
            &m_noDepthMaterial
        );


        // Blend-enabled pipeline

        m_renderSystem->renderCube(
            {2.0f, 0.0f, -6.0f},
            1.0f,
            &m_blendMaterial
        );
    }

    // --------------------------------------------------
    // Runtime texture + named binding path
    // --------------------------------------------------

    void RenderLab::renderRuntimeTextureTests()
    {
        Transform transformA;

        transformA.position =
        {
            3.5f,
            -1.5f,
            -4.0f
        };

        transformA.scale =
        {
            1.5f,
            1.5f,
            1.5f
        };

        renderer().drawMesh(
            transformA,
            m_runtimeTextureMesh,
            m_runtimeTextureMaterial
        );


        Transform transformB;

        transformB.position =
        {
            5.5f,
            -1.5f,
            -4.0f
        };

        transformB.scale =
        {
            1.5f,
            1.5f,
            1.5f
        };

        renderer().drawMesh(
            transformB,
            m_runtimeTextureMesh,
            m_multiTextureMaterial
        );
    }

    // --------------------------------------------------
    // CPU instancing
    // --------------------------------------------------

    void RenderLab::renderInstancing()
    {
        const InstanceData instances[] =
        {
            {
                .position = {-2.0f, -1.5f, -3.0f},
                .rotation = {0.0f, 0.0f, 0.0f},
                .scale = {0.5f, 0.5f, 0.5f},
                .color = RED,
            },
            {
                .position = {0.0f, -1.5f, -3.0f},
                .rotation = {0.0f, 45.0f, 0.0f},
                .scale = {0.5f, 1.0f, 0.5f},
                .color = GREEN,
            },
            {
                .position = {2.0f, -1.5f, -3.0f},
                .rotation = {30.0f, 30.0f, 45.0f},
                .scale = {1.0f, 0.5f, 0.5f},
                .color = BLUE,
            }
        };

        renderer().drawMeshInstanced(
            m_instancingMesh,
            m_instancingMaterial,
            instances,
            3
        );
    }

    // --------------------------------------------------
    // Light debug visualization
    // --------------------------------------------------

    void RenderLab::renderLights()
    {
        for (
            const LightInstance& light :
            m_lights
        )
        {
            m_renderSystem->renderSphere(
                light.object->transform().position,
                0.1f,
                12
            );
        }
    }
}
