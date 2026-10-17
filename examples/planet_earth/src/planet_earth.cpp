#include "planet_earth.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"
#include "types/builtin_shaders.h"
#include <time/time.h>

#include <magic_enum/magic_enum.hpp>

#include <imgui.h>

namespace pe
{

    void PlanetEarth::init()
    {

        m_camera = scene().createCamera(aiko::camera::CameraController::Fly);
        m_camera->getCamera().position = { 0.0f, 1.0f, 2.5f };
        m_camera->speed() = 28.0f;
        m_camera->radius() = 300.0f;

        m_rotateCamera = m_camera->getCameraController() == aiko::camera::CameraController::Orbit;

        scene().clearColor() = aiko::BLACK;

        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.05f;

        m_sunLight = aiko::LightData{
            .type = aiko::LightType::Directional,
            .direction = {1.0f, -0.2f, 0.0f},
            .color = aiko::WHITE,
            .intensity = 1.5f
        };

        m_earthMaterial.m_shaderId = assets().loadShader(aiko::renderer::BuiltinShader::Model);
        m_earthMaterial.m_baseColor = aiko::WHITE;
        m_earthMaterial.m_lit = true;
        m_earthMaterial.m_useVertexColor = true;

        m_oceanMesh.upload(aiko::mesh::factory::generateMeshSphere(192, 192));

        m_oceanMaterial.m_shaderId = assets().loadShader(aiko::renderer::BuiltinShader::Model);
        m_oceanMaterial.m_baseColor = aiko::Color(0.04f, 0.20f, 0.65f, 0.4f);
        m_oceanMaterial.m_lit = true;
        m_oceanMaterial.m_useVertexColor = false;
        m_oceanMaterial.m_renderState.blend = true;
        m_oceanMaterial.m_renderState.depthTest = true;
        m_oceanMaterial.m_renderState.depthWrite = false;
        m_oceanMaterial.m_renderState.cullMode = aiko::CullMode::Back;

    }

    void PlanetEarth::update()
    {
        if (m_animateSun)
        {

            const float dt = aiko::Time::it().getDeltaTime();

            m_sunLight.direction = aiko::math::normalize(
                aiko::math::rotate(
                    m_sunLight.direction,
                    m_sunSpeed * dt,
                    {0.0f, 1.0f, 0.0f}
                )
            );
        }
    }

    void PlanetEarth::render()
    {

        if (ImGui::Begin("Planet Earth"))
        {
            ImGui::SeparatorText("File Download");
            {
                if (ImGui::BeginCombo("FileRequest##Mode", magic_enum::enum_name(m_mapResolution).data()))
                {
                    for (const auto current : magic_enum::enum_values<HeightMapResolution>())
                    {
                        const bool selected = m_mapResolution == current;

                        if (ImGui::Selectable(magic_enum::enum_name(current).data(), selected))
                        {
                            m_mapResolution = current;
                        }

                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (m_downloader.isFileCached(m_mapResolution))
                {
                    if (ImGui::Button("Clear Cache"))
                    {
                        m_downloader.clearCache(m_mapResolution);
                    }
                    if (ImGui::Button("Generate Earth Height Map data"))
                    {
                        m_worldChunks.clear();
                        const EarthHeightmap data = m_downloader.generateHeighMapData(m_mapResolution);
                        const std::vector<GeneratedTerrainChunk> assets = m_generator.generateWorldMeshAssets(data);
                        for (const GeneratedTerrainChunk& generatedChunk : assets)
                        {
                            m_worldChunks.emplace_back(generatedChunk);
                        }

                    }
                }
                else
                {
                    if (ImGui::Button("Download"))
                    {
                        m_downloader.downloadHeightMap(m_mapResolution);
                    }
                }
            }
            ImGui::Spacing();

            ImGui::SeparatorText("Camera Setting");
            ImGui::SliderFloat("Camera Speed", &m_camera->speed(), 0.0f, 64.0f);
            ImGui::SliderFloat("Camera Radius", &m_camera->radius(), 1.0f, 512.0f);
            if (ImGui::Checkbox("Rotate", &m_rotateCamera) == true)
            {
                m_camera->setCameraController(m_rotateCamera ? aiko::camera::Orbit : aiko::camera::Fly);
                if (m_rotateCamera)
                {
                    m_camera->getCamera().target = {0.0f};
                }
            }
            ImGui::Spacing();

            ImGui::SeparatorText("Rendering Setting");
            ImGui::Checkbox("Render Ocean", &m_showOcean);
            ImGui::ColorEdit4("Ocean Color", &m_oceanMaterial.m_baseColor.r);
            ImGui::Text("Sun");
            ImGui::SliderFloat("Sun Intensity", &m_sunLight.intensity, 0.0f, 2.0f);
            ImGui::ColorEdit4("Sun Color", &m_sunLight.color.r);
            ImGui::Checkbox("Animate Sun", &m_animateSun);
            if (m_animateSun)
            {
                ImGui::SliderFloat("Sun Speed", &m_sunSpeed, 0.0f, 0.2f);
            }
            else
            {
                ImGui::SliderFloat3("Sun Direction", &m_sunLight.direction.x, -1.0f, 1.0f);
            }
            ImGui::Spacing();

        }
        ImGui::End();

        // Render World

        renderer().submitLight(m_sunLight);

        for (const TerrainChunk& chunk : m_worldChunks)
        {
            if (chunk.mesh.isValid() == false)
            {
                continue;
            }
            renderer().drawMesh(chunk.transform, chunk.mesh, m_earthMaterial);
        }

        if (m_showOcean == true)
        {
            aiko::Transform oceanTransform;
            constexpr float oceanDiameter = WorldRadius * 2.0f;
            oceanTransform.scale = { oceanDiameter, oceanDiameter, oceanDiameter};
            renderer().drawMesh(oceanTransform, m_oceanMesh, m_oceanMaterial);
        }

    }

}

