#pragma once

#include <math/math.h>
#include <optional>

#include "systems/base_system.h"
#include "ui/ui_types.h"
#include "ui/ui_style.h"
#include "ui/ui_layout.h"
#include "core/uuid.h"
#include "ui/ui_pointer_event.h"

namespace aiko
{

    class CanvasComponent;
    class RectTransformComponent;
    class SceneSystem;
    class DisplayModule;
    class RenderSystem;
    class GameObject;
    class InputSystem;
    class AssetSystem;

    class UISystem : public BaseSystem
    {
    private:
        enum class LayoutAxis
        {
            Horizontal,
            Vertical
        };
        struct ResolvedChild
        {
            GameObject* object = nullptr;
            UIRect rect;
        };
    public:

        UISystem();
        virtual ~UISystem() = default;

        float resolveCanvasScale(const CanvasComponent& canvas, const vec2& surfaceSize) const;

        vec2 framebufferToCanvasPosition(const CanvasComponent& canvas, const vec2& framebufferSize, const vec2& framebufferPosition) const;

        UIRect resolveRect(const RectTransformComponent& rectTransform, const UIRect& parentRect) const;

        GameObject* getPointerTarget();
        const GameObject* getPointerTarget() const;

    protected:
        SystemUpdatePhase updatePhase() const override { return SystemUpdatePhase::UIInteraction; }
        SystemRenderPhase renderPhase() const override { return SystemRenderPhase::UI; }
        void connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector) override;
        void update() override;
        void render() override;

    private:

        DisplayModule* m_displayModule = nullptr;
        RenderSystem* m_renderSystem = nullptr;
        SceneSystem* m_sceneSystem = nullptr;
        InputSystem* m_inputSystem = nullptr;
        AssetSystem* m_assetSystem = nullptr;

        std::optional<uuid::Uuid> m_pointerTargetId;
        std::optional<uuid::Uuid> m_pointerCaptureId;

        void renderCanvas(CanvasComponent& canvas, const vec2& surfaceSize);
        void renderObject(GameObject& object, const UIRect& parentRect, float canvasScale, const UITheme& theme);

        void renderResolvedObject(GameObject& object, const UIRect& resolvedRect, float canvasScale, const UITheme& theme);
        void renderLinearLayout(GameObject& object, const UIRect& resolvedRect, const UIPadding& padding, float spacing, UICrossAxisAlignment childAlignment, LayoutAxis axis, float canvasScale, const UITheme& theme);

        GameObject* hitTest(const vec2& framebufferPosition);
        GameObject* hitTestCanvas(CanvasComponent& canvas, const vec2& framebufferSize, const vec2& framebufferPosition);
        GameObject* hitTestObject(GameObject& object, const UIRect& parentRect, const vec2& pointerPosition);
        GameObject* hitTestResolvedObject(GameObject& object, const UIRect& resolvedRect, const vec2& pointerPosition);
        bool containsPoint(const UIRect& rect, const vec2& point) const;
        bool isRaycastTarget(const GameObject& object) const;
        vector<ResolvedChild> resolveLinearLayoutChildren(GameObject& object, const UIRect& resolvedRect, const UIPadding& padding, float spacing, UICrossAxisAlignment childAlignment, LayoutAxis axis) const;

        GameObject* findCommonAncestor(GameObject* first, GameObject* second) const;
        void routePointerEvent(GameObject& target, const UIPointerEvent& event, const GameObject* stopBefore);

        GameObject* getPointerCapture();

        GameObject* findPointerEventHandlerOwner(GameObject* target) const;

        bool isDescendantOrSelf(const GameObject* object, const GameObject* ancestor) const;

    };

}
