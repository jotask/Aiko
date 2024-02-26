#include "render_component_3d.h"

#include "core/libs.h"

#include "config.h"

#include "modules/module_connector.h"
#include "modules/renderer/render_module.h"
#include "modules/camera_module.h"
#include "core/utils.h"

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

        auto* camera = m_renderModule->getCameraModule()->GetMainCamera();

        // Move camera around the scene
        double cameraTime = time * 0.3;
        camera->position.x = (float)cos(cameraTime) * 40.0f;
        camera->position.z = (float)sin(cameraTime) * 40.0f;

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

        auto* camera = m_renderModule->getCameraModule()->GetMainCamera();

        /*
        typedef struct Camera3D
        {
            Vector3 position;       // Camera position
            Vector3 target;         // Camera target it looks-at
            Vector3 up;             // Camera up vector (rotation over its axis)
            float fovy;             // Camera field-of-view aperture in Y (degrees) in perspective, used as near plane width in orthographic
            int projection;         // Camera projection: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
        } Camera3D;
        */

        Camera3D cam3D;
        {
            cam3D.position = Utils::toV3(camera->position);
            cam3D.target = Utils::toV3(camera->target);
            cam3D.up = Utils::toV3(camera->getUp());
            cam3D.fovy = camera->getFOV();
            cam3D.projection = camera->getCameraType();
        }

        BeginMode3D( cam3D );

        DrawGrid(10, 5.0f);

        float time = GetTime();

        // Calculate time scale for cube position and size
        float scale = (2.0f + (float)sin(time)) * 0.7f;

        constexpr int numBlocks = 15;

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
