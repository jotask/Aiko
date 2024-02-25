#include "render_component_3d.h"

#include <raylib.h>

#include "config.h"

#include "modules/module_connector.h"
#include "modules/renderer/render_module.h"
#include "modules/camera_module.h"

namespace aiko
{
    
    RenderComponent3D::RenderComponent3D(RenderModule* renderModule)
        : RendererComponent(renderModule)
    {
    }
    
    void RenderComponent3D::preInit()
    {
    }
    
    void RenderComponent3D::init()
    {
    }
    
    void RenderComponent3D::postInit()
    {
    }
    
    void RenderComponent3D::preUpdate()
    {
    }
    
    void RenderComponent3D::update()
    {

        double time = GetTime();

        auto& camera = m_renderModule->getCameraModule()->GetMainCamera();

        // Move camera around the scene
        double cameraTime = time * 0.3;
        camera.position.x = (float)cos(cameraTime) * 40.0f;
        camera.position.z = (float)sin(cameraTime) * 40.0f;

    }
    
    void RenderComponent3D::postUpdate()
    {
    }
    
    void RenderComponent3D::preRender()
    {
    
    }
    
    void RenderComponent3D::render()
    {

        ClearBackground(RAYWHITE);

        BeginMode3D(m_renderModule->getCameraModule()->GetMainCamera());

        DrawGrid(10, 5.0f);

        float time = GetTime();

        // Calculate time scale for cube position and size
        float scale = (2.0f + (float)sin(time)) * 0.7f;

        for (int x = 0; x < numBlocks; x++)
        {
            for (int y = 0; y < numBlocks; y++)
            {
                for (int z = 0; z < numBlocks; z++)
                {
                    // Scale of the blocks depends on x/y/z positions
                    float blockScale = (x + y + z) / 30.0f;

                    // Scatter makes the waving effect by adding blockScale over time
                    float scatter = sinf(blockScale * 20.0f + (float)(time * 4.0f));

                    // Calculate the cube position
                    Vector3 cubePos = {
                        (float)(x - numBlocks / 2) * (scale * 3.0f) + scatter,
                        (float)(y - numBlocks / 2) * (scale * 2.0f) + scatter,
                        (float)(z - numBlocks / 2) * (scale * 3.0f) + scatter
                    };

                    // Pick a color with a hue depending on cube position for the rainbow color effect
                    Color cubeColor = ColorFromHSV((float)(((x + y + z) * 18) % 360), 0.75f, 0.9f);

                    // Calculate cube size
                    float cubeSize = (2.4f - scale) * blockScale;

                    // And finally, draw the cube!
                    DrawCube(cubePos, cubeSize, cubeSize, cubeSize, cubeColor);
                }
            }
        }

        EndMode3D();

    }
    
    void RenderComponent3D::postRender()
    {
    }

}
