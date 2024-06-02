#pragma once

#include <glm/glm.hpp>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <functional>
#include <map>

#include "imgui_helper.h"
#include "models/component.h"
#include "components/transform_component.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/camera.h"
#include "types/camera_types.h"

namespace aiko
{
    
    class ComponentRenderer
    {
    
        // TODO
        // super temporally class. All this should be refactored
        // Unproud code!
    
    public:
        static void render(Component* cmp)
        {
            if (cmp == nullptr)
            {
                throw std::exception();
            }
    
            #define HANDLE_COMPONENT(component) if ( handleComponent<component>(cmp) == true ) return;
    
            HANDLE_COMPONENT(Transform);
            HANDLE_COMPONENT(CameraComponent);
            HANDLE_COMPONENT(MeshComponent);
            HANDLE_COMPONENT(LightComponent);
    
            throw std::exception("Not Supported");
    
        }
    
    private:
    
        using ComponentFunc = std::function<void(Component*)>;
        using Dictionary = std::map<std::type_index, ComponentFunc >;
    
        static Dictionary dict;
    
        template<class T>
        static bool handleComponent(Component * cmp)
        {
            T* tmp = dynamic_cast<T*>(cmp);
            if (tmp == nullptr)
            {
                return false;
            }
            auto r = dict.find(typeid(T));
            if (r == dict.end())
            {
                throw new std::exception("Component can be cast but not found!");
            }
            /*
            ImGui::PushID( &cmp->uuid );
            */
            ComponentFunc func = r->second;
            func(cmp);
            /*
            ImGui::PopID();
            */
            return true;
        }
    
        #define ENSURE_CAST if ( t == nullptr) throw std::exception();
    
        static void render_transform(Component* cmp)
        {
            Transform* t = dynamic_cast<Transform*>(cmp);
            ENSURE_CAST;
            ImGui::DragFloat3("Position", t->position, 0.25f);
            ImGui::DragFloat3("Rotation", t->rotation, 0.25f);
            ImGui::DragFloat3("Scale",    t->scale,    0.25f);
        }
    
        static void render_camera(Component* cmp)
        {
            CameraComponent* t = dynamic_cast<CameraComponent*>(cmp);
            ENSURE_CAST;
            // Type
            {
    
                using Dictionarty = std::map<camera::CameraType, string>;
    
                static const Dictionarty types =
                {
                    { camera::CameraType::Orthographic, "Orthographic"},
                    { camera::CameraType::Perspective, "Perspective"},
                };
    
                const auto combo_preview_value = types.find(t->getCameraType());
    
                /*
                if (ImGui::BeginCombo("Type", combo_preview_value->second.c_str()))
                {
                    for (auto it = types.begin(); it != types.end() ; it++ )
                    {
                        const bool is_selected = (t->getCameraType() == it->first);
                        if (ImGui::Selectable(it->second.c_str(), is_selected))
                        {
                            t->setCameraType(it->first);
                        }
    
                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                */
            }
            // Controller
            {
                using Dictionarty = std::map<camera::CameraController, string>;
    
                static const Dictionarty types =
                {
                    { camera::CameraController::Drag, "Drag"},
                    { camera::CameraController::Fly, "Fly"},
                    { camera::CameraController::Orbit, "Orbit"},
                };
    
                const auto combo_preview_value = types.find(t->getCameraController());
    
                /*
                if (ImGui::BeginCombo("Controller", combo_preview_value->second.c_str()))
                {
                    for (auto it = types.begin(); it != types.end() ; it++ )
                    {
                        const bool is_selected = (t->getCameraController() == it->first);
                        if (ImGui::Selectable(it->second.c_str(), is_selected))
                        {
                            t->setCameraController(it->first);
                        }
    
                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                */
            }
        }
    
        static void render_mesh(Component* cmp)
        {
            MeshComponent* t = dynamic_cast<MeshComponent*>(cmp);
            ENSURE_CAST;
            /*
            ImGui::Text("Meshes here");
            */
        }
    
        static void render_light(Component* cmp)
        {
            LightComponent* t = dynamic_cast<LightComponent*>(cmp);
            ENSURE_CAST;
            /*
            ImGui::DragFloat("Intensity", &t->intensity, 0.025f, 0.f);
            */
            ImGui::ColorPicker("##Color", t->color);
    
        }
    
    };

}
