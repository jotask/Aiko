#include "entity_component_system.h"

#include <stdexcept>
#include <memory>

// TODO Please delete, temporally as initial code
#include <raylib.h>

#include "modules/module_connector.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/component_renderer.h"
#include "render_system.h"
#include "models/light.h"

//  TODO TMP
#include "modules/renderer/render_component_texture.h"

namespace aiko
{
    
    RenderSystem::RenderSystem()
        : m_seeds()
        , m_rendererComponentTexture(nullptr)
    {
    }
    
    void RenderSystem::init()
    {
        m_rendererComponentTexture = (RenderComponentTexture*)m_renderModule->GetRenderComponent();
        // TODO Again, please delete me
        RegenerateSeeds();
        GenerateCelular();
    }
    
    void RenderSystem::update()
    {
        if (IsKeyPressed(KEY_F2))
        {
            RegenerateSeeds();
            GenerateCelular();
        }
    }
    
    aiko::AikoPtr<Mesh> RenderSystem::createMesh()
    {
        auto mesh = std::make_unique<Mesh>();
        return mesh;
    }
    
    aiko::AikoPtr<Light> RenderSystem::createLight()
    {
        auto light = std::make_unique<Light>();
        return light;
    }
    
    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        m_renderModule = moduleConnector->find<RenderModule>();
    }
    
    void RenderSystem::GenerateCelular()
    {   
        auto pixels = m_rendererComponentTexture->getPixels();
        auto screen = m_rendererComponentTexture->getDisplayViewport();
        int seedsPerRow = screen.x / tileSize;
        int seedsPerCol = screen.y / tileSize;
        int seedCount = seedsPerRow * seedsPerCol;
    
        for (int y = 0; y < screen.y; y++)
        {
            int tileY = y / tileSize;
    
            for (int x = 0; x < screen.x; x++)
            {
                int tileX = x / tileSize;
    
                float minDistance = 65536.0f; //(float)strtod("Inf", NULL);
    
                // Check all adjacent tiles
                for (int i = -1; i < 2; i++)
                {
                    if ((tileX + i < 0) || (tileX + i >= seedsPerRow)) continue;
    
                    for (int j = -1; j < 2; j++)
                    {
                        if ((tileY + j < 0) || (tileY + j >= seedsPerCol)) continue;
    
                        vec2 neighborSeed = m_seeds[(tileY + j) * seedsPerRow + tileX + i];
    
                        float dist = (float)hypot(x - (int)neighborSeed.x, y - (int)neighborSeed.y);
                        minDistance = (float)fmin(minDistance, dist);
                    }
                }
    
                // I made this up, but it seems to give good results at all tile sizes
                int intensity = (int)(minDistance * 256.0f / tileSize);
                if (intensity > 255) intensity = 255;
    
                unsigned char newIntensity = (unsigned char)intensity;

                pixels[y * screen.x + x] = { newIntensity, newIntensity, newIntensity, 255 };
    
            }
        }
        m_rendererComponentTexture->setPixels(pixels);
    }
    
    void RenderSystem::RegenerateSeeds()
    {
        auto screen = m_rendererComponentTexture->getDisplayViewport();
        int seedsPerRow = screen.x / tileSize;
        int seedsPerCol = screen.y / tileSize;
        int seedCount = seedsPerRow * seedsPerCol;
    
        float deltaTime = GetFrameTime();
    
        m_seeds = std::vector<vec2>(seedCount);
    
        for (int i = 0; i < seedCount; i++)
        {
            const int random = GetRandomValue(0, tileSize - 1) + deltaTime;
            int y = (i / seedsPerRow) * tileSize + random;
            int x = (i % seedsPerRow) * tileSize + random;
            m_seeds[i] = { (float)x, (float)y };
        }
    }
    
    void RenderSystem::add(Light* light)
    {
        switch (light->m_type)
        {
        case Light::Type::Ambient:
            {
    
            }
            break;
        case Light::Type::Directional:
            {
    
            }
            break;
        case Light::Type::Point:
            {
    
            }
            break;
        default:
            throw new std::exception();
            break;
        }
    }
    
    void RenderSystem::render(MeshComponent* mesh)
    {
    
    }

}
