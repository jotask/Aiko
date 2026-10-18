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
        m_camera->getCamera().position = {0.0f, 0.0f, WorldRadius * 3.0f};
        m_camera->speed() = 28.0f;
        m_camera->radius() = 300.0f;

        m_rotateCamera = m_camera->getCameraController() == aiko::camera::CameraController::Orbit;

        scene().clearColor() = aiko::BLACK;

        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.05f;

        m_sunMesh.upload(aiko::mesh::factory::generateMeshSphere(32, 32));
        m_sunMaterial.m_shaderId = assets().loadShader(aiko::renderer::BuiltinShader::Model);
        m_sunMaterial.m_baseColor = aiko::WHITE;
        m_sunMaterial.m_lit = false;
        m_sunMaterial.m_useVertexColor = false;

        m_sunLight = aiko::LightData{
            .type = aiko::LightType::Directional,
            .direction = {1.0f, -0.2f, 0.0f},
            .color = aiko::WHITE,
            .intensity = 1.5f
        };
        m_sunLight.direction = aiko::math::normalize(m_sunLight.direction);

        m_earthMaterial.m_shaderId = assets().loadShader(aiko::renderer::BuiltinShader::Model);
        m_earthMaterial.m_baseColor = aiko::WHITE;
        m_earthMaterial.m_lit = true;
        m_earthMaterial.m_useVertexColor = true;

        m_oceanMesh.upload(aiko::mesh::factory::generateMeshSphere(192, 192));

        m_oceanMaterial.m_shaderId = assets().loadShader(aiko::renderer::BuiltinShader::Model);
        m_oceanMaterial.m_baseColor = aiko::Color(0.02f, 0.10f, 0.25f, 0.70f);
        m_oceanMaterial.setSpecular(0.25f, 160.0f);
        m_oceanMaterial.m_lit = true;
        m_oceanMaterial.m_useVertexColor = false;
        m_oceanMaterial.m_renderState.blend = true;
        m_oceanMaterial.m_renderState.depthTest = true;
        m_oceanMaterial.m_renderState.depthWrite = false;
        m_oceanMaterial.m_renderState.cullMode = aiko::CullMode::Back;

        // Atmosphere
        m_atmosphereMaterial.m_shaderId = assets().loadShader("model.vs", "atmosphere.fs");

        m_atmosphereMaterial.m_baseColor = aiko::Color(0.22f, 0.52f, 1.0f, 1.0f);

        m_atmosphereMaterial.m_lit = false;
        m_atmosphereMaterial.m_useVertexColor = false;

        m_atmosphereMaterial.m_renderState.blend = true;
        m_atmosphereMaterial.m_renderState.depthTest = true;
        m_atmosphereMaterial.m_renderState.depthWrite = false;
        m_atmosphereMaterial.m_renderState.cullMode = aiko::CullMode::Back;

        m_atmosphereMaterial.setVec4(
            "u_atmosphereParams",
            m_atmosphereIntensity,
            m_atmosphereFalloff,
            m_atmosphereSunsetStrength,
            0.0f
        );

        m_atmosphereMaterial.setVec4("u_sunsetColor", 1.0f, 0.42f, 0.12f, 1.0f);


        m_scatteringAtmosphereMaterial.m_shaderId = assets().loadShader("model.vs", "atmosphere_scattering.fs");

        m_scatteringAtmosphereMaterial.m_baseColor = aiko::WHITE;

        m_scatteringAtmosphereMaterial.m_lit = false;
        m_scatteringAtmosphereMaterial.m_useVertexColor = false;

        m_scatteringAtmosphereMaterial.m_renderState.blend = true;
        m_scatteringAtmosphereMaterial.m_renderState.depthTest = true;
        m_scatteringAtmosphereMaterial.m_renderState.depthWrite = false;
        m_scatteringAtmosphereMaterial.m_renderState.cullMode = aiko::CullMode::Back;

        m_scatteringAtmosphereMaterial.setVec4("u_planetCenterRadius", 0.0f, 0.0f, 0.0f, WorldRadius);
        m_scatteringAtmosphereMaterial.setVec4("u_rayleighScattering", 0.015f, 0.038f, 0.090f, 0.0f);
        m_scatteringAtmosphereMaterial.setVec4("u_mieSettings", 0.015f, 0.76f, 1.0f, 0.85f);

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

            ImGui::SeparatorText("Ocean");

            ImGui::Checkbox("Render Ocean", &m_showOcean);

            if (m_showOcean)
            {
                ImGui::ColorEdit4("Ocean Color", &m_oceanMaterial.m_baseColor.r);
            }

            ImGui::Spacing();

            ImGui::SeparatorText("Sun");

            ImGui::Checkbox("Show Sun", &m_showSun);

            ImGui::SliderFloat("Sun Intensity", &m_sunLight.intensity, 0.0f, 2.0f);
            ImGui::ColorEdit3("Sun Color", &m_sunLight.color.r);

            ImGui::Checkbox("Animate Sun", &m_animateSun);

            if (m_animateSun)
            {
                ImGui::SliderFloat("Sun Speed", &m_sunSpeed, 0.0f, 0.2f);
            }
            else
            {
                const aiko::vec3 previousDirection = m_sunLight.direction;

                if (ImGui::SliderFloat3("Sun Direction", &m_sunLight.direction.x, -1.0f, 1.0f))
                {
                    if (aiko::math::lengthSquared(m_sunLight.direction) <= 1e-8f)
                    {
                        m_sunLight.direction = previousDirection;
                    }
                    else
                    {
                        m_sunLight.direction = aiko::math::normalize(m_sunLight.direction);
                    }
                }
            }

            ImGui::Spacing();

            ImGui::SeparatorText("Atmosphere");

            ImGui::Checkbox("Show Atmosphere", &m_showAtmosphere);

            if (m_showAtmosphere)
            {
                ImGui::Checkbox("Use Scattering Atmosphere", &m_useScatteringAtmosphere);
                ImGui::SliderFloat("Atmosphere Thickness", &m_atmosphereThickness, 0.005f, 0.10f);

                if (m_useScatteringAtmosphere)
                {
                    ImGui::SliderFloat("Scattering Exposure", &m_scatteringExposure, 0.5f, 20.0f);
                }
                else
                {
                    ImGui::SliderFloat("Atmosphere Intensity", &m_atmosphereIntensity, 0.0f, 1.0f);
                    ImGui::SliderFloat("Atmosphere Falloff", &m_atmosphereFalloff, 1.0f, 8.0f);
                    ImGui::SliderFloat("Sunset Strength", &m_atmosphereSunsetStrength, 0.0f, 1.0f);
                    ImGui::ColorEdit3("Atmosphere Color", &m_atmosphereMaterial.m_baseColor.r);
                }
            }
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

        if (m_showSun == true)
        {
            constexpr float SunDistance = 500.0f;
            aiko::Transform sunTransform;
            sunTransform.position = aiko::math::normalize(m_sunLight.direction) * -SunDistance;
            sunTransform.scale = {20.0f, 20.0f, 20.0f};
            renderer().drawMesh(sunTransform, m_sunMesh, m_sunMaterial);
        }

        if (m_showAtmosphere)
        {
            const float groundRadius = WorldRadius;
            const float atmosphereRadius = groundRadius * (1.0f + m_atmosphereThickness);

            aiko::Transform atmosphereTransform;
            atmosphereTransform.scale =
            {
                atmosphereRadius * 2.0f,
                atmosphereRadius * 2.0f,
                atmosphereRadius * 2.0f
            };

            if (m_useScatteringAtmosphere)
            {
                m_scatteringAtmosphereMaterial.setVec4(
                    "u_atmosphereSettings",
                    atmosphereRadius,
                    WorldRadius * 0.0077f,
                    WorldRadius * 0.0028f,
                    m_scatteringExposure
                );

                renderer().drawMesh(atmosphereTransform, m_oceanMesh, m_scatteringAtmosphereMaterial);
            }
            else
            {
                m_atmosphereMaterial.setVec4("u_atmosphereParams", m_atmosphereIntensity, m_atmosphereFalloff, m_atmosphereSunsetStrength, 0.0f);
                renderer().drawMesh(atmosphereTransform, m_oceanMesh, m_atmosphereMaterial);
            }
        }
    }
}

