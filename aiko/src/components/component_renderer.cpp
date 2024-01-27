#include "component_renderer.h"

namespace aiko
{
    
    ComponentRenderer::Dictionary ComponentRenderer::dict =
    {
        { typeid(Transform), [](Component* t) { render_transform(t); } } ,
        { typeid(CameraComponent), [](Component* t) { render_camera(t); } },
        { typeid(MeshComponent), [](Component* t) { render_mesh(t); } },
        { typeid(LightComponent), [](Component* t) { render_light(t); } },
    };

}
