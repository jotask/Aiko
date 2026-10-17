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
        camera->speed() *= 4.0f;

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

        if (m_worldMesh.isValid() == true)
        {

            renderer().submitLight(aiko::LightData{
                .type = aiko::LightType::Directional,
                .direction = {1.0f, -1.0f, 0.3f},
                .color = aiko::WHITE,
                .intensity = 1.5f
            });

            aiko::Transform transform;
            renderer().drawMesh(transform, m_worldMesh, m_defaultMaterial);
        }

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
                const EarthHeightmap data = m_downloader.generateHeighMapData(m_mapResolution);
                const aiko::MeshAsset asset = m_generator.generateMeshAsset(data);
                m_worldMesh.upload(asset);
            }
            ImGui::Spacing();

        }
        ImGui::End();
    }

}

