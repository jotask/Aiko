#pragma once

#include <math/math.h>

#include "systems/base_system.h"
#include "ui/ui_types.h"
#include "ui/ui_style.h"
#include "ui/ui_layout.h"

namespace aiko
{

    class CanvasComponent;
    class RectTransformComponent;
    class SceneSystem;
    class DisplayModule;
    class RenderSystem;
    class GameObject;

    class UISystem : public BaseSystem
    {
    private:
        enum class LayoutAxis
        {
            Horizontal,
            Vertical
        };
    public:

        UISystem();
        virtual ~UISystem() = default;

        float resolveCanvasScale(const CanvasComponent& canvas, const vec2& surfaceSize) const;

        UIRect resolveRect(const RectTransformComponent& rectTransform, const UIRect& parentRect) const;

    protected:

        void connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector) override;
        void render() override;

    private:

        DisplayModule* m_displayModule = nullptr;
        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;

        void renderCanvas(CanvasComponent& canvas, const vec2& surfaceSize);
        void renderObject(GameObject& object, const UIRect& parentRect, float canvasScale, const UITheme& theme);

        void renderResolvedObject(GameObject& object, const UIRect& resolvedRect, float canvasScale, const UITheme& theme);
        void renderLinearLayout(GameObject& object, const UIRect& resolvedRect, const UIPadding& padding, float spacing, UICrossAxisAlignment childAlignment, LayoutAxis axis, float canvasScale, const UITheme& theme);
    };

}
