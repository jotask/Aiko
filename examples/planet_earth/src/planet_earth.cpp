#include "planet_earth.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "types/builtin_shaders.h"

#include <magic_enum/magic_enum.hpp>

#include <imgui.h>

namespace pe
{

    void PlanetEarth::init()
    {

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 1.0f, 2.5f };
        camera->speed() *= 8.0f;

        scene().clearColor() = aiko::BLACK;

        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 1.0f;

        m_defaultMaterial.m_shaderId = assets().loadShader(aiko::renderer::BuiltinShader::Model);
        m_defaultMaterial.m_baseColor = aiko::RED;
        m_defaultMaterial.m_lit = true;
        m_defaultMaterial.m_useVertexColor = true;

    }

    void PlanetEarth::update()
    {

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

            ImGui::SeparatorText("File Generate Mesh");
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
            ImGui::Spacing();

        }
        ImGui::End();

        // Render World

        renderer().submitLight(aiko::LightData{
            .type = aiko::LightType::Directional,
            .direction = {1.0f, -1.0f, 0.3f},
            .color = aiko::WHITE,
            .intensity = 1.5f
        });

        for (const TerrainChunk& chunk : m_worldChunks)
        {
            if (chunk.mesh.isValid() == false)
            {
                continue;
            }
            renderer().drawMesh(chunk.transform, chunk.mesh, m_defaultMaterial);
        }

    }

}

