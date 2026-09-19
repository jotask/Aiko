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
#include "layers/contexts/scene_context.h"
#include "models/camera.h"
#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "models/texture_factory.h"
#include "models/sprite_sheet.h"
#include "systems/render_system.h"
#include "systems/system_connector.h"
#include "types/builtin_shaders.h"
#include "types/color.h"
#include "types/render_state.h"

#include <core/random.h>

#include <chrono>
#include <array>
#include <cmath>

#include "layers/contexts/ui_context.h"

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
        constexpr bool EnableGpuVertexTests = true;
        constexpr bool EnableRenderTargetTests = true;
        constexpr bool EnableUI = true;
        constexpr bool EnableRenderTargetResizeStress = true;
    }

    void RenderLab::connect(SystemConnector& systemConnector)
    {
        BIND_SYSTEM_REQUIRED_REF(RenderSystem, systemConnector, m_renderSystem);
    }

    void RenderLab::init()
    {
        scene().clearColor() = RAYWHITE;
        scene().ambientLight().color = WHITE;
        scene().ambientLight().intensity = 0.15f;

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

        if constexpr (EnableGpuVertexTests)
        {
            initGpuVertices();
        }

        if constexpr (EnableRenderTargetTests)
        {
            initRenderTarget();
        }

        if constexpr (EnableUI)
        {
            initUI();
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

        if constexpr (EnableRenderTargetTests && EnableRenderTargetResizeStress)
        {
            updateRenderTargetStress();
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

        if constexpr (EnableGpuVertexTests)
        {
            renderGpuVertices();
        }

        if constexpr (EnableRenderTargetTests)
        {
            renderRenderTargetTest();
        }

        if constexpr (EnableUI)
        {
            renderUI();
        }

    }

    // --------------------------------------------------
    // Camera
    // --------------------------------------------------

    void RenderLab::initCamera()
    {
        GameObject* camera = Instantiate("Camera");

        CameraComponent* cameraComponent = camera->addComponent<CameraComponent>(camera::CameraController::Fly);

        camera->transform().position =
        {
            0.0f,
            2.5f,
            8.0f
        };

        cameraComponent->getCamera().position = camera->transform().position;
    }

    // --------------------------------------------------
    // Imported model / asset validation
    // --------------------------------------------------

    void RenderLab::initModels()
    {
        GameObject* root = Instantiate("Models");

        const float modelOffset = 30.0f;

        GameObject* church = Instantiate(root, "Church");
        church->transform().position = {-modelOffset, 0.0f, -28.0f};
        ModelComponent* churchModel = church->addComponent<ModelComponent>();
        churchModel->load("church.obj");

        GameObject* barracks = Instantiate(root, "Barracks");
        barracks->transform().position = {0.0f, 0.0f, -28.0f};
        ModelComponent* barracksModel = barracks->addComponent<ModelComponent>();
        barracksModel->load("barracks.obj");

        GameObject* watermill = Instantiate(root, "Watermill");
        watermill->transform().position = {modelOffset, 0.0f, -28.0f};
        ModelComponent* watermillModel = watermill->addComponent<ModelComponent>();
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

        ModelComponent* robotModel = robot->addComponent<ModelComponent>();
        robotModel->load("robot.glb");
    }

    // --------------------------------------------------
    // Standard engine components
    // --------------------------------------------------

    void RenderLab::initComponents()
    {
        GameObject* root = Instantiate("Components");

        const MeshAsset cube = mesh::factory::generateCube();

        m_cubeA = Instantiate(root, "CubeA");
        m_cubeA->transform().position =
        {
            -1.5f,
            0.0f,
            1.5f
        };

        MeshComponent* meshA = m_cubeA->addComponent<MeshComponent>();

        meshA->load(cube);

        meshA->getMaterial().m_baseColor = YELLOW;

        m_cubeB = Instantiate(root, "CubeB");
        m_cubeB->transform().position =
        {
            1.5f,
            0.0f,
            1.5f
        };

        MeshComponent* meshB = m_cubeB->addComponent<MeshComponent>();

        meshB->load(cube);

        meshB->getMaterial().m_baseColor = BLUE;

        GameObject* assetTexture = Instantiate(root, "AssetTexture");

        assetTexture->transform().position =
        {
            -1.25f,
            -1.5f,
            1.0f
        };

        m_uiTestSprite = assetTexture->addComponent<SpriteComponent>();

        m_uiTestSprite->load("texel_checker.png");

        const SpriteSheet spriteSheet(256, 256, 64, 64);
        m_uiTestSprite->setTextureRegion(spriteSheet.region(1));

        m_uiTestSprite->getMaterial().m_lit = false;

        m_dynamicTextureObject = Instantiate(root, "DynamicTexture");

        m_dynamicTextureObject->transform().position =
        {
            1.25f,
            -1.5f,
            1.0f
        };

        SpriteComponent* dynamicSprite = m_dynamicTextureObject->addComponent<SpriteComponent>();

        dynamicSprite->load(texture::factory::generateBlank(128, 128, BLACK));

        Material& material = dynamicSprite->getMaterial();

        material.m_lit = false;

        const SamplerState nearestSampler =
        {
            .minFilter = TextureFilter::Nearest,
            .magFilter = TextureFilter::Nearest,
            .mipFilter = TextureMipFilter::None,
        };

        material.setTextureSampler("u_texture", nearestSampler);
    }

    // --------------------------------------------------
    // Materials / uniforms / render states
    // --------------------------------------------------

    void RenderLab::initMaterials()
    {
        const AssetId modelShader = assets().loadShader(renderer::BuiltinShader::Model);

        // --------------------------------------------------
        // Basic material
        // --------------------------------------------------

        m_defaultMaterial.m_shaderId = modelShader;
        m_defaultMaterial.m_baseColor = WHITE;
        m_defaultMaterial.m_lit = false;
        m_defaultMaterial.m_useVertexColor = false;

        // --------------------------------------------------
        // Primitive colors
        // --------------------------------------------------

        m_primitiveRedMaterial.m_shaderId = modelShader;
        m_primitiveRedMaterial.m_baseColor = RED;
        m_primitiveRedMaterial.m_lit = false;
        m_primitiveRedMaterial.m_useVertexColor = false;

        m_primitiveGreenMaterial.m_shaderId = modelShader;
        m_primitiveGreenMaterial.m_baseColor = GREEN;
        m_primitiveGreenMaterial.m_lit = false;
        m_primitiveGreenMaterial.m_useVertexColor = false;

        m_primitiveBlueMaterial.m_shaderId = modelShader;
        m_primitiveBlueMaterial.m_baseColor = BLUE;
        m_primitiveBlueMaterial.m_lit = false;
        m_primitiveBlueMaterial.m_useVertexColor = false;

        m_primitiveYellowMaterial.m_shaderId = modelShader;
        m_primitiveYellowMaterial.m_baseColor = YELLOW;
        m_primitiveYellowMaterial.m_lit = false;
        m_primitiveYellowMaterial.m_useVertexColor = false;

        // --------------------------------------------------
        // Lighting comparison
        // --------------------------------------------------

        m_litMaterial.m_shaderId = modelShader;
        m_litMaterial.m_baseColor = WHITE;
        m_litMaterial.m_lit = true;
        m_litMaterial.m_useVertexColor = false;

        m_unlitMaterial.m_shaderId = modelShader;
        m_unlitMaterial.m_baseColor = WHITE;
        m_unlitMaterial.m_lit = false;
        m_unlitMaterial.m_useVertexColor = false;

        // --------------------------------------------------
        // Fill modes
        // --------------------------------------------------

        m_solidMaterial.m_shaderId = modelShader;
        m_solidMaterial.m_baseColor = GREEN;
        m_solidMaterial.m_lit = false;
        m_solidMaterial.m_renderState.fillMode = FillMode::Solid;

        m_wireframeMaterial.m_shaderId = modelShader;
        m_wireframeMaterial.m_baseColor = YELLOW;
        m_wireframeMaterial.m_lit = false;
        m_wireframeMaterial.m_renderState.fillMode = FillMode::Wireframe;


        m_pointMaterial.m_shaderId = modelShader;
        m_pointMaterial.m_baseColor = MAGENTA;
        m_pointMaterial.m_lit = false;
        m_pointMaterial.m_renderState.fillMode = FillMode::Point;

        // --------------------------------------------------
        // Culling
        // --------------------------------------------------

        m_cullNoneMaterial.m_shaderId = modelShader;
        m_cullNoneMaterial.m_baseColor = WHITE;
        m_cullNoneMaterial.m_lit = false;
        m_cullNoneMaterial.m_renderState.cullMode = CullMode::None;

        m_cullFrontMaterial.m_shaderId = modelShader;
        m_cullFrontMaterial.m_baseColor = RED;
        m_cullFrontMaterial.m_lit = false;
        m_cullFrontMaterial.m_renderState.cullMode = CullMode::Front;

        m_cullBackMaterial.m_shaderId = modelShader;
        m_cullBackMaterial.m_baseColor = BLUE;
        m_cullBackMaterial.m_lit = false;
        m_cullBackMaterial.m_renderState.cullMode = CullMode::Back;

        // --------------------------------------------------
        // Depth state
        // --------------------------------------------------

        m_depthMaterial.m_shaderId = modelShader;
        m_depthMaterial.m_baseColor = GREEN;
        m_depthMaterial.m_lit = false;
        m_depthMaterial.m_renderState.depthTest = true;
        m_depthMaterial.m_renderState.depthWrite = true;
        m_depthMaterial.m_renderState.depthCompare = DepthCompare::LessEqual;

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

        // --------------------------------------------------
        // Typed uniform validation
        // --------------------------------------------------

        const AssetId uniformShader = assets().loadShader("uniform_validation");
        m_uniformMaterial.m_shaderId = uniformShader;
        m_uniformMaterial.m_baseColor = WHITE;
        m_uniformMaterial.m_lit = false;
        m_uniformMaterial.m_useVertexColor = false;

        m_uniformMaterial.setBool("u_labBool", true);
        m_uniformMaterial.setInt("u_labInt", -42);
        m_uniformMaterial.setUInt("u_labUInt", 42u);
        m_uniformMaterial.setFloat("u_labFloat", 0.5f);
        m_uniformMaterial.setVec2("u_labVec2", vec2{0.25f, 0.75f});
        m_uniformMaterial.setVec3("u_labVec3", vec3{1.0f, 0.5f, 0.25f});
        m_uniformMaterial.setVec4("u_labVec4", vec4{1.0f, 0.5f, 0.25f, 1.0f});
        m_uniformMaterial.setMat4("u_labMat4", mat4(1.0f));

        // --------------------------------------------------
        // Custom shader selection validation
        // --------------------------------------------------

        const AssetId customShader = assets().loadShader("custom_shader_validation");

        m_customShaderMaterial.m_shaderId = customShader;
        m_customShaderMaterial.m_baseColor = RED;
        m_customShaderMaterial.m_lit = true;
        m_customShaderMaterial.m_useVertexColor = true;

    }

    // --------------------------------------------------
    // Runtime textures + named bindings + samplers
    // --------------------------------------------------

    void RenderLab::initRuntimeTextures()
    {
        m_runtimeTextureA.upload(texture::factory::generateBlank(64, 64, RED));
        m_runtimeTextureB.upload(texture::factory::generateBlank(64, 64, BLUE) );
        m_runtimeTextureMesh.upload(mesh::factory::generateQuad());

        const AssetId modelShader = assets().loadShader(renderer::BuiltinShader::Model);

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
        m_runtimeTextureMaterial.setTexture("u_texture", &m_runtimeTextureA, nearestSampler);

        // --------------------------------------------------
        // Multiple named texture bindings
        //
        // This validates Material's named-binding storage,
        // sampler persistence and Vulkan material-key path.
        //
        // A shader must actually declare both descriptors
        // before both become visible in the final fragment.
        // --------------------------------------------------

        const AssetId multiTextureShader = assets().loadShader("multi_texture_validation");
        m_multiTextureMaterial.m_shaderId = multiTextureShader;
        m_multiTextureMaterial.m_lit = false;
        m_multiTextureMaterial.m_useVertexColor = false;
        m_multiTextureMaterial.m_baseColor = WHITE;

        m_multiTextureMaterial.setTexture("u_texture", &m_runtimeTextureA, nearestSampler);
        m_multiTextureMaterial.setTexture("u_secondaryTexture", &m_runtimeTextureB, linearSampler);

    }

    // --------------------------------------------------
    // CPU instancing
    // --------------------------------------------------

    void RenderLab::initInstancing()
    {
        m_instancingMesh.upload(mesh::factory::generateCube());
        m_instancingMaterial.m_shaderId = assets().loadShader(renderer::BuiltinShader::Model);
        m_instancingMaterial.m_useVertexColor = true;
        m_instancingMaterial.m_lit = false;
        m_instancingMaterial.m_baseColor = WHITE;
    }

    // --------------------------------------------------
    // Lighting
    // --------------------------------------------------

    void RenderLab::initLights()
    {

        constexpr float LightRange = 8.0f;
        constexpr float LightIntensity = 1.0f;

        GameObject* root = Instantiate("Lights");

        constexpr float SpawnRadius = 6.0f;
        constexpr size_t LightCount = 8;

        for (size_t i = 0; i < LightCount; ++i)
        {
            GameObject* object = Instantiate(root, "PointLight");

            object->transform().position =
            {
                utils::getRandomValue(-SpawnRadius, SpawnRadius),
                utils::getRandomValue(0.0f, SpawnRadius),
                -12.0f + utils::getRandomValue(-SpawnRadius, SpawnRadius),
            };

            LightComponent* light = object->addComponent<LightComponent>();

            light->setPointLight(Color::getRandomColor(), LightRange);
            light->intensity = LightIntensity;

            LightInstance instance;
            instance.object = object;
            instance.component = light;
            instance.angle = utils::getRandomValue(0.0f, 360.0f);

            m_lights.push_back(instance);
        }
    }

    // --------------------------------------------------
    // GPU particle / billboard path
    // --------------------------------------------------

    void RenderLab::initParticles()
    {
        GameObject* root = Instantiate("Particles");

        GameObject* object = Instantiate(root, "ParticleEmitter");

        object->transform().position =
        {
            0.0f,
            0.0f,
            -4.0f
        };

        ParticleEmitterComponent* emitter = object->addComponent<ParticleEmitterComponent>();
        emitter->setMaxParticles(1024 * 64);
        emitter->setLifetime(2.0f);
        emitter->setDirection({0.0f, 1.0f, 0.0f});
        emitter->setDirectionRandomness(0.8f);
        emitter->setGravity({0.0f, -1.5f, 0.0f});
        emitter->setStartSpeed(2.0f);
        emitter->setSpawnRate(10000.0f);
        emitter->setSpawnShape(ParticleEmitterComponent::ParticleSpawnShape::Point);
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
        GameObject* root = Instantiate("Compute");

        // --------------------------------------------------
        // Compute -> texture -> render
        // --------------------------------------------------

        GameObject* computeTexture = Instantiate(root, "ComputeTexture");

        computeTexture->transform().position =
        {
            0.0f,
            2.5f,
            0.0f
        };

        ComputeShaderComponent* compute = computeTexture->addComponent<ComputeShaderComponent>();
        compute->load("gradient");
        compute->setUseOutputTexture(true);
        compute->setOutputSize(512, 512);
        compute->setExecutionMode(ComputeShaderComponent::ComputeExecutionMode::Continuous);
        compute->setUpdateInterval(0.0f);

        SpriteComponent* sprite = computeTexture->addComponent<SpriteComponent>();
        sprite->load(texture::factory::generateBlank(512, 512, BLACK));

        sprite->getMaterial().m_lit = false;
        sprite->getMaterial().m_useVertexColor = false;


        // --------------------------------------------------
        // Compute -> GPU buffer -> CPU readback
        // --------------------------------------------------

        GameObject* readbackObject = Instantiate(root, "ComputeReadback");

        m_computeReadback = readbackObject->addComponent<ComputeShaderComponent>();
        m_computeReadback->load("compute_test");
        m_computeReadback->setElementCount(64);
        m_computeReadback->setExecutionMode(ComputeShaderComponent::ComputeExecutionMode::Once);
    }

    void RenderLab::initGpuVertices()
    {
        const std::array<vec4, 4> vertices =
        {
            vec4{-1.0f, -1.0f, -8.0f, 1.0f},
            vec4{ 1.0f, -1.0f, -8.0f, 1.0f},
            vec4{ 1.0f,  1.0f, -8.0f, 1.0f},
            vec4{-1.0f,  1.0f, -8.0f, 1.0f},
        };

        const std::array<uint32_t, 6> indices =
        {
            0, 1, 2,
            0, 2, 3,
        };

        // VkDrawIndexedIndirectCommand:
        // indexCount, instanceCount, firstIndex, vertexOffset, firstInstance
        const std::array<uint32_t, 5> indirect =
        {
            6,
            1,
            0,
            0,
            0,
        };

        m_gpuVertexBuffer.create(
            {
                .format = ComputeBufferFormat::Vec4f,
                .count = static_cast<uint32_t>(vertices.size()),
                .usage = ComputeBufferUsage::Vertex | ComputeBufferUsage::TransferDst,
            },
            vertices.data()
        );

        m_gpuIndexBuffer.create(
            {
                .format = ComputeBufferFormat::Uint32,
                .count = static_cast<uint32_t>(indices.size()),
                .usage = ComputeBufferUsage::Index | ComputeBufferUsage::TransferDst,
            },
            indices.data()
        );

        m_gpuIndirectBuffer.create(
            {
                .format = ComputeBufferFormat::Uint32,
                .count = static_cast<uint32_t>(indirect.size()),
                .usage = ComputeBufferUsage::Indirect | ComputeBufferUsage::TransferDst,
            },
            indirect.data()
        );

        m_gpuVertexMaterial.m_shaderId = assets().loadShader("gpu_vertex_validation");

        m_gpuVertexMaterial.m_lit = false;
        m_gpuVertexMaterial.m_useVertexColor = false;
        m_gpuVertexMaterial.m_baseColor = WHITE;

    }

    void RenderLab::initRenderTarget()
    {
        m_validationRenderTarget.create(640, 360);

        AIKO_ASSERT(m_validationRenderTarget.isValid(), "RenderLab validation render target is invalid");

        m_secondaryCamera.position =
        {
            10.0f,
            6.0f,
            4.0f
        };

        m_secondaryCamera.target =
        {
            0.0f,
            0.0f,
            -8.0f
        };

        m_renderTargetMesh.upload(mesh::factory::generateQuad());

        m_renderTargetMaterial.m_shaderId = assets().loadShader(renderer::BuiltinShader::Model);

        m_renderTargetMaterial.m_baseColor = WHITE;
        m_renderTargetMaterial.m_lit = false;
        m_renderTargetMaterial.m_useVertexColor = false;

        const SamplerState sampler =
        {
            .minFilter = TextureFilter::Linear,
            .magFilter = TextureFilter::Linear,
            .mipFilter = TextureMipFilter::None,
        };

        m_renderTargetMaterial.setTexture("u_texture", &m_validationRenderTarget.colorTexture(), sampler);
    }

    void RenderLab::initUI()
    {
       // --------------------------------------------------
        // Canvas 0 - main retained UI
        // --------------------------------------------------

        GameObject* canvasObject = scene().Instantiate("UI Test Canvas");

        CanvasComponent* canvas = canvasObject->addComponent<CanvasComponent>();

        canvas->setScaleMode(CanvasScaleMode::ScaleWithScreenSize);
        canvas->setReferenceResolution({1920.0f, 1080.0f});
        canvas->setMatchWidthOrHeight(0.5f);
        canvas->setSortingOrder(0);

        canvas->getTheme().image.appearance.color = MAGENTA;

        // --------------------------------------------------
        // Center panel
        //
        // Fixed-size centered RectTransform.
        // --------------------------------------------------

        GameObject* centerPanel = scene().Instantiate(canvasObject, "Center Panel");

        RectTransformComponent* centerRect = centerPanel->addComponent<RectTransformComponent>();

        centerRect->setAnchors({0.5f, 0.5f}, {0.5f, 0.5f});

        centerRect->setPivot({0.5f, 0.5f});
        centerRect->setAnchoredPosition({0.0f, 0.0f});
        centerRect->setSizeDelta({500.0f, 300.0f});

        ImageComponent* centerImage = centerPanel->addComponent<ImageComponent>();

        centerImage->setColor(RED);
        centerImage->clearColorOverride();

        // --------------------------------------------------
        // Nested child
        //
        // Stretches inside the red parent with a logical
        // 50-unit inset on every side.
        // --------------------------------------------------

        GameObject* nestedPanel = scene().Instantiate(centerPanel, "Nested Panel");

        RectTransformComponent* nestedRect = nestedPanel->addComponent<RectTransformComponent>();

        nestedRect->setAnchors({0.0f, 0.0f}, {1.0f, 1.0f});

        nestedRect->setPivot({0.5f, 0.5f});
        nestedRect->setAnchoredPosition({0.0f, 0.0f});
        nestedRect->setSizeDelta({-100.0f, -100.0f});

        ImageComponent* nestedImage = nestedPanel->addComponent<ImageComponent>();

        nestedImage->setColor(GREEN);

        // --------------------------------------------------
        // Stretch test
        //
        // Anchored to the bottom area of the entire Canvas.
        // 100 logical-unit horizontal margins.
        // --------------------------------------------------

        GameObject* stretchPanel = scene().Instantiate(canvasObject, "Stretch Panel");

        RectTransformComponent* stretchRect = stretchPanel->addComponent<RectTransformComponent>();

        stretchRect->setAnchors({0.0f, 1.0f}, {1.0f, 1.0f});

        stretchRect->setPivot({0.5f, 1.0f});
        stretchRect->setAnchoredPosition({0.0f, -50.0f});
        stretchRect->setSizeDelta({-200.0f, 100.0f});

        ImageComponent* stretchImage = stretchPanel->addComponent<ImageComponent>();

        stretchImage->setColor(BLUE);

        // --------------------------------------------------
        // Horizontal layout test
        //
        // Validates:
        // - padding
        // - spacing
        // - preferred sizes
        // - flexible surplus distribution
        // - cross-axis centering
        // --------------------------------------------------

        GameObject* layoutPanel = scene().Instantiate(canvasObject, "Horizontal Layout");

        RectTransformComponent* layoutRect = layoutPanel->addComponent<RectTransformComponent>();

        layoutRect->setAnchors(
            {0.5f, 0.5f},
            {0.5f, 0.5f});

        layoutRect->setPivot({0.5f, 0.5f});
        layoutRect->setAnchoredPosition({0.0f, -300.0f});
        layoutRect->setSizeDelta({800.0f, 100.0f});

        ImageComponent* layoutBackground = layoutPanel->addComponent<ImageComponent>();

        layoutBackground->setColor(GRAY);

        HorizontalLayoutComponent* horizontalLayout = layoutPanel->addComponent<HorizontalLayoutComponent>();

        horizontalLayout->setPadding(
            {
                .left = 20.0f,
                .right = 20.0f,
                .top = 20.0f,
                .bottom = 20.0f
            });

        horizontalLayout->setSpacing(20.0f);
        horizontalLayout->setChildAlignment(UICrossAxisAlignment::Center);

        // Child A
        // Preferred 120 wide, never consumes surplus.

        GameObject* layoutChildA = scene().Instantiate(layoutPanel, "Layout Child A");

        layoutChildA->addComponent<RectTransformComponent>();

        ImageComponent* layoutImageA = layoutChildA->addComponent<ImageComponent>();

        layoutImageA->setColor(RED);

        LayoutElementComponent* layoutElementA = layoutChildA->addComponent<LayoutElementComponent>();

        layoutElementA->setMinSize({80.0f, 60.0f});
        layoutElementA->setPreferredSize({120.0f, 80.0f});

        // Child B
        // Flexible weight 1.

        GameObject* layoutChildB = scene().Instantiate(layoutPanel, "Layout Child B");

        layoutChildB->addComponent<RectTransformComponent>();

        ImageComponent* layoutImageB = layoutChildB->addComponent<ImageComponent>();

        layoutImageB->setColor(GREEN);

        LayoutElementComponent* layoutElementB = layoutChildB->addComponent<LayoutElementComponent>();

        layoutElementB->setMinSize({100.0f, 80.0f});
        layoutElementB->setPreferredSize({160.0f, 100.0f});
        layoutElementB->setFlexibleWeight({1.0f, 0.0f});

        // Child C
        // Flexible weight 2, so it receives twice B's surplus.

        GameObject* layoutChildC = scene().Instantiate(layoutPanel, "Layout Child C");

        layoutChildC->addComponent<RectTransformComponent>();

        ImageComponent* layoutImageC = layoutChildC->addComponent<ImageComponent>();

        layoutImageC->setColor(BLUE);

        LayoutElementComponent* layoutElementC = layoutChildC->addComponent<LayoutElementComponent>();

        layoutElementC->setMinSize({100.0f, 100.0f});
        layoutElementC->setPreferredSize({160.0f, 120.0f});
        layoutElementC->setFlexibleWeight({2.0f, 0.0f});

        // --------------------------------------------------
        // Vertical layout test
        //
        // Validates:
        // - vertical main-axis allocation
        // - preferred heights
        // - flexible Y surplus distribution
        // - horizontal cross-axis centering
        // --------------------------------------------------

        GameObject* verticalPanel = scene().Instantiate(canvasObject, "Vertical Layout");

        RectTransformComponent* verticalRect = verticalPanel->addComponent<RectTransformComponent>();

        verticalRect->setAnchors(
            {0.0f, 0.5f},
            {0.0f, 0.5f});

        verticalRect->setPivot({0.0f, 0.5f});
        verticalRect->setAnchoredPosition({80.0f, 0.0f});
        verticalRect->setSizeDelta({320.0f, 500.0f});

        ImageComponent* verticalBackground = verticalPanel->addComponent<ImageComponent>();

        verticalBackground->setColor(GRAY);

        VerticalLayoutComponent* verticalLayout = verticalPanel->addComponent<VerticalLayoutComponent>();

        verticalLayout->setPadding(
            {
                .left = 20.0f,
                .right = 20.0f,
                .top = 20.0f,
                .bottom = 20.0f
            });

        verticalLayout->setSpacing(20.0f);
        verticalLayout->setChildAlignment(UICrossAxisAlignment::Center);

        // Child A
        // Fixed at preferred height once enough space exists.

        GameObject* verticalChildA = scene().Instantiate(verticalPanel, "Vertical Child A");

        verticalChildA->addComponent<RectTransformComponent>();

        ImageComponent* verticalImageA = verticalChildA->addComponent<ImageComponent>();

        verticalImageA->setColor(RED);

        LayoutElementComponent* verticalElementA = verticalChildA->addComponent<LayoutElementComponent>();

        verticalElementA->setMinSize({120.0f, 60.0f});
        verticalElementA->setPreferredSize({180.0f, 80.0f});

        // Child B
        // Flexible Y weight 1.

        GameObject* verticalChildB = scene().Instantiate(verticalPanel, "Vertical Child B");

        verticalChildB->addComponent<RectTransformComponent>();

        ImageComponent* verticalImageB = verticalChildB->addComponent<ImageComponent>();

        verticalImageB->setColor(GREEN);

        LayoutElementComponent* verticalElementB = verticalChildB->addComponent<LayoutElementComponent>();

        verticalElementB->setMinSize({160.0f, 80.0f});
        verticalElementB->setPreferredSize({220.0f, 100.0f});
        verticalElementB->setFlexibleWeight({0.0f, 1.0f});

        // Child C
        // Flexible Y weight 2, so it receives twice B's
        // share of vertical surplus.

        GameObject* verticalChildC = scene().Instantiate(verticalPanel, "Vertical Child C");

        verticalChildC->addComponent<RectTransformComponent>();

        ImageComponent* verticalImageC = verticalChildC->addComponent<ImageComponent>();

        verticalImageC->setColor(BLUE);

        LayoutElementComponent* verticalElementC = verticalChildC->addComponent<LayoutElementComponent>();

        verticalElementC->setMinSize({200.0f, 80.0f});
        verticalElementC->setPreferredSize({260.0f, 100.0f});
        verticalElementC->setFlexibleWeight({0.0f, 2.0f});

        // --------------------------------------------------
        // Nested layout test
        //
        // Vertical layout controls two row containers.
        // Each row then performs its own horizontal layout
        // using the effective rect assigned by the parent.
        //
        // Validates:
        // Vertical -> effective UIRect -> Horizontal.
        // --------------------------------------------------

        GameObject* nestedLayoutPanel = scene().Instantiate(canvasObject, "Nested Layout");

        RectTransformComponent* nestedLayoutRect = nestedLayoutPanel->addComponent<RectTransformComponent>();

        nestedLayoutRect->setAnchors(
            {1.0f, 0.5f},
            {1.0f, 0.5f});

        nestedLayoutRect->setPivot({1.0f, 0.5f});
        nestedLayoutRect->setAnchoredPosition({-80.0f, 0.0f});
        nestedLayoutRect->setSizeDelta({500.0f, 400.0f});

        ImageComponent* nestedLayoutBackground = nestedLayoutPanel->addComponent<ImageComponent>();

        nestedLayoutBackground->setColor(GRAY);

        VerticalLayoutComponent* nestedVerticalLayout = nestedLayoutPanel->addComponent<VerticalLayoutComponent>();

        nestedVerticalLayout->setPadding(
            {
                .left = 20.0f,
                .right = 20.0f,
                .top = 20.0f,
                .bottom = 20.0f
            });

        nestedVerticalLayout->setSpacing(20.0f);
        nestedVerticalLayout->setChildAlignment(UICrossAxisAlignment::Center);

        // --------------------------------------------------
        // Nested row A
        // --------------------------------------------------

        GameObject* nestedRowA = scene().Instantiate(nestedLayoutPanel, "Nested Row A");

        nestedRowA->addComponent<RectTransformComponent>();

        ImageComponent* nestedRowAImage = nestedRowA->addComponent<ImageComponent>();

        nestedRowAImage->setColor(WHITE);

        LayoutElementComponent* nestedRowAElement = nestedRowA->addComponent<LayoutElementComponent>();

        nestedRowAElement->setMinSize({300.0f, 100.0f});
        nestedRowAElement->setPreferredSize({420.0f, 120.0f});
        nestedRowAElement->setFlexibleWeight({0.0f, 1.0f});

        HorizontalLayoutComponent* nestedRowALayout = nestedRowA->addComponent<HorizontalLayoutComponent>();

        nestedRowALayout->setPadding(
            {
                .left = 10.0f,
                .right = 10.0f,
                .top = 10.0f,
                .bottom = 10.0f
            });

        nestedRowALayout->setSpacing(10.0f);
        nestedRowALayout->setChildAlignment(UICrossAxisAlignment::Center);

        GameObject* nestedRowAChildA = scene().Instantiate(nestedRowA, "Nested Row A Child A");

        nestedRowAChildA->addComponent<RectTransformComponent>();

        ImageComponent* nestedRowAImageA = nestedRowAChildA->addComponent<ImageComponent>();

        nestedRowAImageA->setColor(RED);

        LayoutElementComponent* nestedRowAElementA = nestedRowAChildA->addComponent<LayoutElementComponent>();

        nestedRowAElementA->setMinSize({80.0f, 50.0f});
        nestedRowAElementA->setPreferredSize({100.0f, 70.0f});
        nestedRowAElementA->setFlexibleWeight({1.0f, 0.0f});

        GameObject* nestedRowAChildB = scene().Instantiate(nestedRowA, "Nested Row A Child B");

        nestedRowAChildB->addComponent<RectTransformComponent>();

        ImageComponent* nestedRowAImageB = nestedRowAChildB->addComponent<ImageComponent>();

        nestedRowAImageB->setColor(GREEN);

        LayoutElementComponent* nestedRowAElementB = nestedRowAChildB->addComponent<LayoutElementComponent>();

        nestedRowAElementB->setMinSize({80.0f, 50.0f});
        nestedRowAElementB->setPreferredSize({100.0f, 70.0f});
        nestedRowAElementB->setFlexibleWeight({2.0f, 0.0f});

        // --------------------------------------------------
        // Nested row B
        // --------------------------------------------------

        GameObject* nestedRowB = scene().Instantiate(nestedLayoutPanel, "Nested Row B");

        nestedRowB->addComponent<RectTransformComponent>();

        ImageComponent* nestedRowBImage = nestedRowB->addComponent<ImageComponent>();

        nestedRowBImage->setColor(WHITE);

        LayoutElementComponent* nestedRowBElement = nestedRowB->addComponent<LayoutElementComponent>();

        nestedRowBElement->setMinSize({300.0f, 100.0f});
        nestedRowBElement->setPreferredSize({420.0f, 120.0f});
        nestedRowBElement->setFlexibleWeight({0.0f, 2.0f});

        HorizontalLayoutComponent* nestedRowBLayout = nestedRowB->addComponent<HorizontalLayoutComponent>();

        nestedRowBLayout->setPadding(
            {
                .left = 10.0f,
                .right = 10.0f,
                .top = 10.0f,
                .bottom = 10.0f
            });

        nestedRowBLayout->setSpacing(10.0f);
        nestedRowBLayout->setChildAlignment(UICrossAxisAlignment::Center);

        GameObject* nestedRowBChildA = scene().Instantiate(nestedRowB, "Nested Row B Child A");

        nestedRowBChildA->addComponent<RectTransformComponent>();

        ImageComponent* nestedRowBImageA = nestedRowBChildA->addComponent<ImageComponent>();

        nestedRowBImageA->setColor(BLUE);

        LayoutElementComponent* nestedRowBElementA = nestedRowBChildA->addComponent<LayoutElementComponent>();

        nestedRowBElementA->setMinSize({80.0f, 50.0f});
        nestedRowBElementA->setPreferredSize({120.0f, 70.0f});
        nestedRowBElementA->setFlexibleWeight({1.0f, 0.0f});

        GameObject* nestedRowBChildB = scene().Instantiate(nestedRowB, "Nested Row B Child B");

        nestedRowBChildB->addComponent<RectTransformComponent>();

        ImageComponent* nestedRowBImageB = nestedRowBChildB->addComponent<ImageComponent>();

        nestedRowBImageB->setColor(MAGENTA);

        LayoutElementComponent* nestedRowBElementB = nestedRowBChildB->addComponent<LayoutElementComponent>();

        nestedRowBElementB->setMinSize({80.0f, 50.0f});
        nestedRowBElementB->setPreferredSize({120.0f, 70.0f});
        nestedRowBElementB->setFlexibleWeight({1.0f, 0.0f});

        // --------------------------------------------------
        // Canvas 1 - sorting test
        //
        // Higher sortingOrder, therefore this must render
        // over the main Canvas where they overlap.
        // --------------------------------------------------

        GameObject* overlayCanvasObject = scene().Instantiate("UI Test Overlay Canvas");

        CanvasComponent* overlayCanvas = overlayCanvasObject->addComponent<CanvasComponent>();

        overlayCanvas->setScaleMode(CanvasScaleMode::ScaleWithScreenSize);
        overlayCanvas->setReferenceResolution({1920.0f, 1080.0f});
        overlayCanvas->setMatchWidthOrHeight(0.5f);
        overlayCanvas->setSortingOrder(100);

        GameObject* overlayPanel = scene().Instantiate(overlayCanvasObject, "Overlay Panel");

        RectTransformComponent* overlayRect = overlayPanel->addComponent<RectTransformComponent>();

        overlayRect->setAnchors({0.5f, 0.5f}, {0.5f, 0.5f});

        overlayRect->setPivot({0.5f, 0.5f});
        overlayRect->setAnchoredPosition({180.0f, 100.0f});
        overlayRect->setSizeDelta({180.0f, 180.0f});

        ImageComponent* overlayImage = overlayPanel->addComponent<ImageComponent>();

        overlayImage->load("texel_checker.png");
        overlayImage->setColor(WHITE);
    }

    // --------------------------------------------------
    // Updates
    // --------------------------------------------------

    void RenderLab::updateComponents()
    {
        static float angle = 0.0f;

        angle += 25.0f * getDeltaTime();

        angle = std::fmod(angle, 360.0f);

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

        SpriteComponent* sprite = m_dynamicTextureObject->getComponent<SpriteComponent>();

        if (sprite == nullptr)
        {
            return;
        }

        static constexpr double UpdateInterval = 1.0 / 60.0;

        static auto previousTime = std::chrono::steady_clock::now();

        static double accumulatedTime = 0.0;

        const auto currentTime = std::chrono::steady_clock::now();

        const std::chrono::duration<double> delta = currentTime - previousTime;

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
                    utils::getRandomValue(0, MaxX ),
                    utils::getRandomValue(0, MaxY)
                };

                particle.direction =
                {
                    utils::getRandomValue(-1, 1),
                    utils::getRandomValue(-1, 1)
                };

                if (particle.direction.x == 0 && particle.direction.y == 0)
                {
                    particle.direction.x = 1;
                }

                particle.color = Color::getRandomColor();

                particles.push_back(particle);
            }
        }

        for (Particle& particle : particles)
        {
            if (particle.position.x <= 0 || particle.position.x >= MaxX)
            {
                particle.direction.x *= -1;
            }

            if (particle.position.y <= 0 || particle.position.y >= MaxY)
            {
                particle.direction.y *= -1;
            }

            particle.position.x += particle.direction.x;
            particle.position.y += particle.direction.y;
            particle.position.x = math::clamp(particle.position.x, 0, MaxX);
            particle.position.y = math::clamp(particle.position.y, 0, MaxY);

            sprite->setPixel(particle.position.x, particle.position.y, particle.color);
        }

        sprite->refresh();
    }

    void RenderLab::updateRenderTargetStress()
    {
        using Clock = std::chrono::steady_clock;

        static const std::array<ivec2, 3> TargetSizes =
        {
            ivec2{320, 180},
            ivec2{640, 360},
            ivec2{960, 540},
        };

        static constexpr double ResizeIntervalSeconds = 0.75;
        static auto lastResize = Clock::now();
        // Initial RenderLab target is 640x360.

        static std::size_t sizeIndex = 1;
        const auto now = Clock::now();
        const std::chrono::duration<double> elapsed = now - lastResize;

        if (elapsed.count() < ResizeIntervalSeconds)
        {
            return;
        }

        lastResize = now;
        sizeIndex = (sizeIndex + 1) % TargetSizes.size();

        const ivec2 targetSize = TargetSizes[sizeIndex];

        m_validationRenderTarget.resize(static_cast<u32>(targetSize.x), static_cast<u32>(targetSize.y));
        AIKO_ASSERT(m_validationRenderTarget.isValid(), "RenderLab render target invalid after resize");

        const ivec2 actualSize = m_validationRenderTarget.size();
        AIKO_ASSERT(actualSize.x == targetSize.x && actualSize.y == targetSize.y, "RenderLab render target size mismatch after resize");

        const TextureBinding* binding = m_renderTargetMaterial.textureBinding("u_texture");
        AIKO_ASSERT(binding != nullptr, "RenderLab render target material lost texture binding");
        AIKO_ASSERT(binding->runtimeTexture == &m_validationRenderTarget.colorTexture(), "RenderLab render target material has stale texture binding");
    }

    void RenderLab::updateLights()
    {
        constexpr float RotationSpeed = 0.5f;
        constexpr float Radius = 4.0f;

        for (LightInstance& light : m_lights)
        {
            light.angle += RotationSpeed * getDeltaTime();

            light.object->transform().position =
            {
                std::sin(light.angle) * Radius,
                1.5f,
                -12.0f + std::cos(light.angle) * Radius
            };
        }
    }

    void RenderLab::updateCompute()
    {
        if (m_computeReadback == nullptr || m_computeReadbackPrinted || !m_computeReadback->hasReadback()
        )
        {
            return;
        }

        const ComputeReadbackResult& result = m_computeReadback->getLastReadback();

        constexpr uint32_t ElementCount = 64;

        constexpr size_t ExpectedSize = ElementCount * sizeof(vec4);

        AIKO_ASSERT(result.data.size() == ExpectedSize, "Invalid compute readback size");

        const vec4* values = reinterpret_cast<const vec4*>(result.data.data());

        for (uint32_t i = 0; i < ElementCount; ++i)
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
        m_renderSystem->renderPoint({-4.0f, 3.0f, 0.0f}, &m_primitiveRedMaterial);
        m_renderSystem->renderLine({-4.0f, 2.5f, 0.0f}, {-2.0f, 2.5f, 0.0f}, &m_primitiveGreenMaterial);
        m_renderSystem->renderTriangle({-4.0f, 2.0f, 0.0f}, Size, &m_primitiveBlueMaterial);
        m_renderSystem->renderRectangle({-3.0f, 2.0f, 0.0f}, Size, &m_primitiveYellowMaterial);
        m_renderSystem->renderCircle({-2.0f, 2.0f, 0.0f}, Size, 32, &m_primitiveRedMaterial);
        m_renderSystem->renderNgon({-1.0f, 2.0f, 0.0f}, Size, 6, &m_primitiveGreenMaterial);
        m_renderSystem->renderCube({-4.0f, 1.0f, 0.0f}, Size, &m_primitiveBlueMaterial);
        m_renderSystem->renderPyramid({-3.0f, 1.0f, 0.0f}, Size, &m_primitiveYellowMaterial);
        m_renderSystem->renderSphere({-2.0f, 1.0f, 0.0f}, Size, 25, &m_primitiveRedMaterial);
        m_renderSystem->renderCylinder({-1.0f, 1.0f, 0.0f}, Size, 16, &m_primitiveGreenMaterial);
        m_renderSystem->renderTorus({-4.0f, 0.0f, 0.0f}, Size, &m_primitiveBlueMaterial);
        m_renderSystem->renderKnot({-3.0f, 0.0f, 0.0f}, Size, &m_primitiveYellowMaterial);
        m_renderSystem->renderGrid({0.0f, -2.5f, 0.0f}, 0.5f, {10, 10}, &m_primitiveGreenMaterial);
    }

    // --------------------------------------------------
    // Material + uniform path
    // --------------------------------------------------

    void RenderLab::renderMaterialTests()
    {
        m_renderSystem->renderCube({-5.0f, 0.0f, -5.0f}, 0.75f, &m_defaultMaterial);
        m_renderSystem->renderCube({-3.0f, 0.0f, -5.0f}, 0.75f, &m_uniformMaterial);
        m_renderSystem->renderCube({-1.0f, 0.0f, -5.0f}, 0.75f, &m_customShaderMaterial);
    }

    // --------------------------------------------------
    // Render state pipeline variants
    // --------------------------------------------------

    void RenderLab::renderRenderStateTests()
    {
        // Fill modes
        m_renderSystem->renderCube({-5.0f, 2.0f, -7.0f}, 0.75f, &m_solidMaterial);
        m_renderSystem->renderCube({-3.0f, 2.0f, -7.0f}, 0.75f, &m_wireframeMaterial);
        m_renderSystem->renderCube({-1.0f, 2.0f, -7.0f}, 0.75f, &m_pointMaterial);

        // Culling
        m_renderSystem->renderCube({1.0f, 2.0f, -7.0f}, 0.75f, &m_cullNoneMaterial);
        m_renderSystem->renderCube({3.0f, 2.0f, -7.0f}, 0.75f, &m_cullFrontMaterial);
        m_renderSystem->renderCube({5.0f, 2.0f, -7.0f}, 0.75f, &m_cullBackMaterial);

        // Depth test
        m_renderSystem->renderCube({-1.0f, 0.0f, -6.0f}, 1.0f, &m_depthMaterial);
        m_renderSystem->renderCube({-0.75f, 0.25f, -6.5f}, 0.75f, &m_noDepthMaterial);

        // Blend-enabled pipeline
        m_renderSystem->renderCube({2.0f, 0.0f, -6.0f}, 1.0f, &m_blendMaterial);
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

        renderer().drawMesh(transformA, m_runtimeTextureMesh, m_runtimeTextureMaterial);

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

        renderer().drawMesh(transformB, m_runtimeTextureMesh, m_multiTextureMaterial);
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

        renderer().drawMeshInstanced(m_instancingMesh, m_instancingMaterial, instances, 3);
    }

    // --------------------------------------------------
    // Light debug visualization
    // --------------------------------------------------

    void RenderLab::renderLights()
    {
        // --------------------------------------------------
        // Lit vs unlit comparison
        //
        // LEFT  = affected by scene lights
        // RIGHT = ignores scene lights
        // --------------------------------------------------

        m_renderSystem->renderSphere({-1.5f, 0.0f, -12.0f}, 1.0f, 32, &m_litMaterial);
        m_renderSystem->renderSphere({1.5f, 0.0f, -12.0f}, 1.0f, 32, &m_unlitMaterial);

        // Additional lit geometry so normals/shading are obvious.

        m_renderSystem->renderCube({-1.5f, -2.0f, -12.0f}, 0.75f, &m_litMaterial);
        m_renderSystem->renderCube({1.5f, -2.0f, -12.0f}, 0.75f, &m_unlitMaterial);

        // Light position markers.
        for (const LightInstance& light : m_lights)
        {
            m_renderSystem->renderSphere(light.object->transform().position, 0.08f, 12, &m_primitiveYellowMaterial);
        }
    }

    void RenderLab::renderGpuVertices()
    {
        const GpuVertexDrawDesc draw =
        {
            .material = &m_gpuVertexMaterial,
            .vertexBuffer = &m_gpuVertexBuffer,
            .vertexCount = 4,
            .indexBuffer = &m_gpuIndexBuffer,
            .indexCount = 6,
            .indirectBuffer = &m_gpuIndirectBuffer,
            .topology = TransientTopology::Triangles,
        };
        renderer().drawVerticesGpu(draw);
    }

    void RenderLab::renderRenderTargetTest()
    {
        // Snapshot everything submitted so far this frame and render it
        // from the secondary camera into the offscreen target.
        renderer().renderToTarget(m_secondaryCamera, m_validationRenderTarget);

        // This is submitted only after the snapshot, so the secondary
        // camera does not recursively render its own monitor.
        Transform monitor;

        monitor.position =
        {
            4.0f,
            2.5f,
            -3.5f
        };

        monitor.scale =
        {
            3.2f,
            1.8f,
            1.0f
        };

        renderer().drawMesh(monitor, m_renderTargetMesh, m_renderTargetMaterial);
    }

    void RenderLab::renderUI()
    {
        // Immediate UIContext test.
        //
        // UISystem has already submitted the retained UI by the
        // time Application::render() reaches here, so these should
        // appear above the retained UI.

        ui().rect(
            {20.0f, 20.0f},
            {200.0f, 60.0f},
            MAGENTA);

        // Clipping test for the immediate frontend.
        ui().pushClipRect(
            {20.0f, 100.0f},
            {200.0f, 100.0f});

        ui().rect(
            {-30.0f, 120.0f},
            {300.0f, 60.0f},
            CYAN);

        ui().popClipRect();
    }
}
