#include "ui_system.h"

#include <cmath>
#include <algorithm>

#include <time/time.h>
#include "system_connector.h"
#include "components/canvas_component.h"
#include "components/rect_transform_component.h"
#include "systems/scene_system.h"
#include "modules/display_module.h"
#include "modules/module_connector.h"
#include "systems/render_system.h"
#include "systems/input_system.h"
#include "systems/asset_system.h"
#include "ui/ui_raycast.h"

#include "components/image_component.h"
#include "models/game_object.h"
#include "scene/scene.h"
#include "components/horizontal_layout_component.h"
#include "components/vertical_layout_component.h"
#include "components/layout_element_component.h"
#include "components/selectable_component.h"
#include "components/button_component.h"

namespace aiko
{

    namespace
    {
        constexpr bool DebugUIPointer = false;
        constexpr bool DebugUISelectableBounds = false;
    }

    UISystem::UISystem() = default;

    void UISystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule);
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
        BIND_SYSTEM_REQUIRED(InputSystem, systemConnector, m_inputSystem);
        BIND_SYSTEM_REQUIRED(AssetSystem, systemConnector, m_assetSystem);
    }

    void UISystem::update()
    {
        GameObject* previousTarget = getPointerTarget();

        GameObject* nextTarget = nullptr;

        const vec2 framebufferPosition = m_inputSystem->getMouseFramebufferPosition();

        if (m_inputSystem->isMouseCaptured() == false)
        {
            nextTarget = hitTest(framebufferPosition);
        }

        GameObject* commonAncestor = findCommonAncestor(previousTarget, nextTarget);

        if (previousTarget != nextTarget)
        {
            if (previousTarget != nullptr)
            {
                const UIPointerEvent exitEvent =
                {
                    .type = UIPointerEventType::Exit,
                    .target = previousTarget,
                    .framebufferPosition = framebufferPosition
                };

                routePointerEvent(*previousTarget, exitEvent, commonAncestor);
            }

            if (nextTarget != nullptr)
            {
                const UIPointerEvent enterEvent =
                {
                    .type = UIPointerEventType::Enter,
                    .target = nextTarget,
                    .framebufferPosition = framebufferPosition
                };

                routePointerEvent(*nextTarget, enterEvent, commonAncestor);
            }
        }

        if (nextTarget != nullptr)
        {
            m_pointerTargetId = nextTarget->uuid();
        }
        else
        {
            m_pointerTargetId.reset();
        }

        constexpr MouseButton PrimaryButton = MouseButton::MOUSE_BUTTON_LEFT;

        if (m_inputSystem->isMouseCaptured() == false && m_inputSystem->isMouseButtonJustPressed(PrimaryButton))
        {
            GameObject* captureTarget = findPointerEventHandlerOwner(nextTarget);

            if (captureTarget != nullptr)
            {
                m_pointerCaptureId = captureTarget->uuid();

                const UIPointerEvent downEvent =
                {
                    .type = UIPointerEventType::Down,
                    .target = captureTarget,
                    .framebufferPosition = framebufferPosition,
                    .button = PrimaryButton
                };

                routePointerEvent(*captureTarget, downEvent, nullptr);
            }
        }

        if (m_inputSystem->isMouseButtonJustReleased(PrimaryButton))
        {
            GameObject* capturedTarget = getPointerCapture();

            if (capturedTarget != nullptr)
            {
                const UIPointerEvent upEvent =
                {
                    .type = UIPointerEventType::Up,
                    .target = capturedTarget,
                    .framebufferPosition = framebufferPosition,
                    .button = PrimaryButton
                };

                routePointerEvent(*capturedTarget, upEvent, nullptr);

                if (isDescendantOrSelf(nextTarget, capturedTarget))
                {
                    const UIPointerEvent clickEvent =
                    {
                        .type = UIPointerEventType::Click,
                        .target = capturedTarget,
                        .framebufferPosition = framebufferPosition,
                        .button = PrimaryButton
                    };

                    routePointerEvent(*capturedTarget, clickEvent, nullptr);
                }
            }

            m_pointerCaptureId.reset();
        }

    }

    void UISystem::render()
    {
        const ivec2 displaySize = m_displayModule->getFramebufferSize();

        if (displaySize.x <= 0 || displaySize.y <= 0)
        {
            return;
        }

        const vec2 surfaceSize =
        {
            static_cast<float>(displaySize.x),
            static_cast<float>(displaySize.y)
        };

        Scene& scene = m_sceneSystem->getScene();

        vector<CanvasComponent*> canvases = scene.components<CanvasComponent>();

        std::stable_sort(
            canvases.begin(),
            canvases.end(),
            [](const CanvasComponent* lhs, const CanvasComponent* rhs)
            {
                return lhs->getSortingOrder() < rhs->getSortingOrder();
            });

        for (CanvasComponent* canvas : canvases)
        {
            if (canvas == nullptr || canvas->isActiveAndEnabled() == false)
            {
                continue;
            }
            renderCanvas(*canvas, surfaceSize);
        }

        if constexpr (DebugUIPointer)
        {
            if (m_inputSystem->isMouseCaptured() == false)
            {
                const vec2 pointer =
                    m_inputSystem->getMouseFramebufferPosition();

                constexpr float size = 8.0f;

                m_renderSystem->drawUiRect(
                    {
                        pointer.x - size * 0.5f,
                        pointer.y - size * 0.5f
                    },
                    {size, size},
                    MAGENTA);
        }
        }
    }

    float UISystem::resolveCanvasScale(const CanvasComponent& canvas, const vec2& surfaceSize) const
    {
        AIKO_ASSERT(surfaceSize.x > 0.0f, "UI surface width must be greater than zero");
        AIKO_ASSERT(surfaceSize.y > 0.0f, "UI surface height must be greater than zero");

        float scale = 1.0f;

        if (canvas.getScaleMode() == CanvasScaleMode::ScaleWithScreenSize)
        {
            const vec2& reference = canvas.getReferenceResolution();

            const float widthScale = surfaceSize.x / reference.x;
            const float heightScale = surfaceSize.y / reference.y;

            const float logWidth = std::log2(widthScale);
            const float logHeight = std::log2(heightScale);

            const float logScale = logWidth + (logHeight - logWidth) * canvas.getMatchWidthOrHeight();

            scale = std::exp2(logScale);
        }

        return scale * canvas.getScaleFactor();
    }

    vec2 UISystem::framebufferToCanvasPosition(const CanvasComponent& canvas, const vec2& framebufferSize, const vec2& framebufferPosition) const
    {
        const float scale = resolveCanvasScale(canvas, framebufferSize);
        AIKO_ASSERT(scale > 0.0f, "Canvas resolved scale must be greater than zero");
        if (scale <= 0.0f)
        {
            return {};
        }
        return framebufferPosition / scale;
    }

    UIRect UISystem::resolveRect(const RectTransformComponent& rectTransform, const UIRect& parentRect) const
    {
        const vec2 parentMin = parentRect.position;
        const vec2 parentSize = parentRect.size;
        const vec2 anchorMin = parentMin + parentSize * rectTransform.getAnchorMin();
        const vec2 anchorMax = parentMin + parentSize * rectTransform.getAnchorMax();
        const vec2 anchorSize = anchorMax - anchorMin;
        const vec2 size = anchorSize + rectTransform.getSizeDelta();
        const vec2 anchorReference = anchorMin + anchorSize * rectTransform.getPivot();
        const vec2 pivotPosition = anchorReference + rectTransform.getAnchoredPosition();
        const vec2 position = pivotPosition - size * rectTransform.getPivot();

        return
        {
            .position = position,
            .size = size
        };
    }

    void UISystem::renderCanvas(CanvasComponent& canvas, const vec2& surfaceSize)
    {
        GameObject* canvasObject = canvas.getGameObject();

        AIKO_ASSERT(canvasObject != nullptr, "CanvasComponent is not attached to a GameObject");

        if (canvasObject == nullptr)
        {
            return;
        }

        const float scale = resolveCanvasScale(canvas, surfaceSize);

        AIKO_ASSERT(scale > 0.0f, "Canvas resolved scale must be greater than zero");

        if (scale <= 0.0f)
        {
            return;
        }

        const UIRect canvasRect =
        {
            .position = {0.0f, 0.0f},
            .size = surfaceSize / scale
        };

        const UITheme& theme = canvas.getTheme();

        for (GameObject* child : canvasObject->getChildren())
        {
            renderObject(*child, canvasRect, scale, theme);
        }
    }

    void UISystem::renderObject(GameObject& object, const UIRect& parentRect, float canvasScale, const UITheme& theme)
    {
        if (object.isActiveInHierarchy() == false)
        {
            return;
        }

        if (CanvasComponent* canvas = object.getComponent<CanvasComponent>())
        {
            return;
        }

        RectTransformComponent* rectTransform =
            object.getComponent<RectTransformComponent>();

        UIRect resolvedRect = parentRect;

        if (rectTransform != nullptr)
        {
            resolvedRect = resolveRect(*rectTransform, parentRect);

            if (resolvedRect.size.x < 0.0f ||
                resolvedRect.size.y < 0.0f)
            {
                return;
            }
        }

        renderResolvedObject(object, resolvedRect, canvasScale, theme);
    }

    void UISystem::renderResolvedObject(GameObject& object, const UIRect& resolvedRect, float canvasScale, const UITheme& theme)
    {

        if (object.isActiveInHierarchy() == false)
        {
            return;
        }

        if (object.hasComponent<CanvasComponent>())
        {
            return;
        }

        if (ImageComponent* image = object.getComponent<ImageComponent>(); image != nullptr && image->isEnabled())
        {
            RectTransformComponent* rectTransform = object.getComponent<RectTransformComponent>();

            AIKO_ASSERT(rectTransform != nullptr, "ImageComponent requires a RectTransformComponent");

            if (rectTransform != nullptr)
            {
                UIImageAppearance appearance = theme.image.appearance;

                if (SelectableComponent* selectable = object.getComponent<SelectableComponent>(); selectable != nullptr)
                {
                    const UISelectableStyle* themeStyle = &theme.selectable;

                    if (object.getComponent<ButtonComponent>() != nullptr)
                    {
                        themeStyle = &theme.button.selectable;
                    }

                    const UISelectableStyle& selectableStyle = selectable->resolveStyle(*themeStyle);

                    appearance = selectable->resolveAppearance(selectableStyle, Time::it().getDeltaTime());
                }

                AssetId textureId = image->getTextureId();

                TextureRegion textureRegion = image->getTextureRegion();

                if (appearance.texture.has_value())
                {
                    const AssetId& styleTexture = *appearance.texture;

                    if (styleTexture != InvalidAssetId && m_assetSystem->isLoaded<TextureAsset>(styleTexture))
                    {
                        textureId = styleTexture;
                    }
                }

                if (appearance.textureRegion.has_value())
                {
                    textureRegion = *appearance.textureRegion;
                }

                if (image->hasColorOverride())
                {
                    appearance.color = image->getColorOverride();
                }

                const vec2 physicalPosition = resolvedRect.position * canvasScale;
                const vec2 physicalSize = resolvedRect.size * canvasScale;
                const float cornerRadius = appearance.cornerRadius * canvasScale;
                const float borderThickness = appearance.border.thickness * canvasScale;

                if (physicalSize.x > 0.0f && physicalSize.y > 0.0f)
                {
                    if (textureId == InvalidAssetId)
                    {
                        m_renderSystem->drawUiRect(physicalPosition, physicalSize, appearance.color, cornerRadius, borderThickness, appearance.border.color);
                    }
                    else
                    {
                        m_renderSystem->drawUiImage(textureId, textureRegion, physicalPosition, physicalSize, appearance.color, cornerRadius, borderThickness, appearance.border.color);
                    }
                }

                if constexpr (DebugUISelectableBounds)
                {

                    if (object.getComponent<SelectableComponent>() != nullptr)
                    {
                        const vec2 debugPosition =
                            resolvedRect.position * canvasScale;

                        const vec2 debugSize =
                            resolvedRect.size * canvasScale;

                        constexpr float thickness = 2.0f;

                        // Top
                        m_renderSystem->drawUiRect(
                            debugPosition,
                            {debugSize.x, thickness},
                            CYAN);

                        // Bottom
                        m_renderSystem->drawUiRect(
                            {
                                debugPosition.x,
                                debugPosition.y + debugSize.y - thickness
                            },
                            {debugSize.x, thickness},
                            CYAN);

                        // Left
                        m_renderSystem->drawUiRect(
                            debugPosition,
                            {thickness, debugSize.y},
                            CYAN);

                        // Right
                        m_renderSystem->drawUiRect(
                            {
                                debugPosition.x + debugSize.x - thickness,
                                debugPosition.y
                            },
                            {thickness, debugSize.y},
                            CYAN);
                    }

                }

            }
        }

        if (HorizontalLayoutComponent* layout = object.getComponent<HorizontalLayoutComponent>(); layout != nullptr && layout->isEnabled())
        {
            renderLinearLayout(
                object,
                resolvedRect,
                layout->getPadding(),
                layout->getSpacing(),
                layout->getChildAlignment(),
                LayoutAxis::Horizontal,
                canvasScale,
                theme);

            return;
        }

        if (VerticalLayoutComponent* layout = object.getComponent<VerticalLayoutComponent>(); layout != nullptr && layout->isEnabled())
        {
            renderLinearLayout(
                object,
                resolvedRect,
                layout->getPadding(),
                layout->getSpacing(),
                layout->getChildAlignment(),
                LayoutAxis::Vertical,
                canvasScale,
                theme);

            return;
        }

        for (GameObject* child : object.getChildren())
        {
            renderObject(*child, resolvedRect, canvasScale, theme);
        }
    }

    vector<UISystem::ResolvedChild> UISystem::resolveLinearLayoutChildren(GameObject& object, const UIRect& resolvedRect, const UIPadding& padding, float spacing, UICrossAxisAlignment childAlignment, LayoutAxis axis) const
    {
        vector<ResolvedChild> result;

        const vector<GameObject*> children =
            object.getChildren();

        vector<GameObject*> activeChildren;

        activeChildren.reserve(children.size());

        for (GameObject* child : children)
        {
            if (child != nullptr &&
                child->isActiveInHierarchy())
            {
                activeChildren.push_back(child);
            }
        }

        if (activeChildren.empty())
        {
            return result;
        }

        result.reserve(activeChildren.size());

        const bool horizontal =
            axis == LayoutAxis::Horizontal;

        const float mainStartPadding =
            horizontal ? padding.left : padding.top;

        const float mainEndPadding =
            horizontal ? padding.right : padding.bottom;

        const float crossStartPadding =
            horizontal ? padding.top : padding.left;

        const float crossEndPadding =
            horizontal ? padding.bottom : padding.right;

        const float resolvedMainSize =
            horizontal
                ? resolvedRect.size.x
                : resolvedRect.size.y;

        const float resolvedCrossSize =
            horizontal
                ? resolvedRect.size.y
                : resolvedRect.size.x;

        const float innerMainSize =
            std::max(
                0.0f,
                resolvedMainSize -
                    mainStartPadding -
                    mainEndPadding);

        const float innerCrossSize =
            std::max(
                0.0f,
                resolvedCrossSize -
                    crossStartPadding -
                    crossEndPadding);

        const float totalSpacing =
            spacing *
            static_cast<float>(
                activeChildren.size() - 1);

        const float availableMainSize =
            std::max(
                0.0f,
                innerMainSize - totalSpacing);

        float totalMinMainSize = 0.0f;
        float totalPreferredMainSize = 0.0f;
        float totalFlexibleWeight = 0.0f;

        for (GameObject* child : activeChildren)
        {
            const LayoutElementComponent* element =
                child->getComponent<LayoutElementComponent>();

            if (element == nullptr ||
                element->isEnabled() == false)
            {
                continue;
            }

            const vec2& minSize =
                element->getMinSize();

            const vec2& preferredSize =
                element->getPreferredSize();

            const vec2& flexibleWeight =
                element->getFlexibleWeight();

            const float minMainSize =
                horizontal
                    ? minSize.x
                    : minSize.y;

            const float preferredMainSize =
                std::max(
                    minMainSize,
                    horizontal
                        ? preferredSize.x
                        : preferredSize.y);

            const float flexibleMainWeight =
                horizontal
                    ? flexibleWeight.x
                    : flexibleWeight.y;

            totalMinMainSize += minMainSize;
            totalPreferredMainSize +=
                preferredMainSize;

            totalFlexibleWeight +=
                flexibleMainWeight;
        }

        float interpolation = 1.0f;
        float surplus = 0.0f;

        if (availableMainSize <
            totalPreferredMainSize)
        {
            const float preferredRange =
                totalPreferredMainSize -
                totalMinMainSize;

            if (preferredRange > 0.0f)
            {
                interpolation =
                    std::clamp(
                        (availableMainSize -
                         totalMinMainSize) /
                            preferredRange,
                        0.0f,
                        1.0f);
            }
            else
            {
                interpolation = 0.0f;
            }
        }
        else
        {
            surplus =
                availableMainSize -
                totalPreferredMainSize;
        }

        float cursor =
            (horizontal
                ? resolvedRect.position.x
                : resolvedRect.position.y) +
            mainStartPadding;

        const float crossStart =
            (horizontal
                ? resolvedRect.position.y
                : resolvedRect.position.x) +
            crossStartPadding;

        for (GameObject* child : activeChildren)
        {
            const LayoutElementComponent* element =
                child->getComponent<LayoutElementComponent>();

            vec2 minSize = {0.0f, 0.0f};
            vec2 preferredSize = {0.0f, 0.0f};
            vec2 flexibleWeight = {0.0f, 0.0f};

            if (element != nullptr &&
                element->isEnabled())
            {
                minSize =
                    element->getMinSize();

                preferredSize =
                    element->getPreferredSize();

                flexibleWeight =
                    element->getFlexibleWeight();
            }

            const float minMainSize =
                horizontal
                    ? minSize.x
                    : minSize.y;

            const float preferredMainSize =
                std::max(
                    minMainSize,
                    horizontal
                        ? preferredSize.x
                        : preferredSize.y);

            const float flexibleMainWeight =
                horizontal
                    ? flexibleWeight.x
                    : flexibleWeight.y;

            float allocatedMainSize =
                minMainSize +
                (preferredMainSize -
                 minMainSize) *
                    interpolation;

            if (surplus > 0.0f &&
                totalFlexibleWeight > 0.0f &&
                flexibleMainWeight > 0.0f)
            {
                allocatedMainSize +=
                    surplus *
                    (flexibleMainWeight /
                     totalFlexibleWeight);
            }

            const float minCrossSize =
                horizontal
                    ? minSize.y
                    : minSize.x;

            const float preferredCrossSize =
                std::max(
                    minCrossSize,
                    horizontal
                        ? preferredSize.y
                        : preferredSize.x);

            const float allocatedCrossSize =
                std::min(
                    innerCrossSize,
                    preferredCrossSize);

            float crossPosition =
                crossStart;

            switch (childAlignment)
            {
                case UICrossAxisAlignment::Start:
                    break;

                case UICrossAxisAlignment::Center:
                    crossPosition +=
                        (innerCrossSize -
                         allocatedCrossSize) *
                        0.5f;
                    break;

                case UICrossAxisAlignment::End:
                    crossPosition +=
                        innerCrossSize -
                        allocatedCrossSize;
                    break;
            }

            UIRect childRect;

            if (horizontal)
            {
                childRect =
                {
                    .position =
                    {
                        cursor,
                        crossPosition
                    },
                    .size =
                    {
                        allocatedMainSize,
                        allocatedCrossSize
                    }
                };
            }
            else
            {
                childRect =
                {
                    .position =
                    {
                        crossPosition,
                        cursor
                    },
                    .size =
                    {
                        allocatedCrossSize,
                        allocatedMainSize
                    }
                };
            }

            result.push_back(
            {
                .object = child,
                .rect = childRect
            });

            cursor +=
                allocatedMainSize +
                spacing;
        }

        return result;
    }

    void UISystem::renderLinearLayout(GameObject& object, const UIRect& resolvedRect, const UIPadding& padding, float spacing, UICrossAxisAlignment childAlignment, LayoutAxis axis, float canvasScale, const UITheme& theme)
    {
        const vector<ResolvedChild> children = resolveLinearLayoutChildren(object, resolvedRect, padding, spacing, childAlignment, axis);

        for (const ResolvedChild& child : children)
        {
            if (child.object == nullptr)
            {
                continue;
            }

            renderResolvedObject(*child.object, child.rect, canvasScale, theme);
        }
    }

    GameObject* UISystem::findCommonAncestor(GameObject* first, GameObject* second) const
    {
        if (first == nullptr || second == nullptr)
        {
            return nullptr;
        }

        for (GameObject* firstCurrent = first; firstCurrent != nullptr; firstCurrent = firstCurrent->getParent())
        {
            for (GameObject* secondCurrent = second; secondCurrent != nullptr; secondCurrent = secondCurrent->getParent())
            {
                if (firstCurrent == secondCurrent)
                {
                    return firstCurrent;
                }

                if (secondCurrent->hasComponent<CanvasComponent>())
                {
                    break;
                }
            }

            if (firstCurrent->hasComponent<CanvasComponent>())
            {
                break;
            }
        }

        return nullptr;
    }

    void UISystem::routePointerEvent(GameObject& target, const UIPointerEvent& event, const GameObject* stopBefore)
    {
        GameObject* current = &target;

        while (current != nullptr && current != stopBefore)
        {
            for (Component* component : current->getComponents())
            {
                if (component == nullptr || component->isActiveAndEnabled() == false)
                {
                    continue;
                }

                UIPointerEventHandler* handler = dynamic_cast<UIPointerEventHandler*>(component);

                if (handler == nullptr)
                {
                    continue;
                }

                const UIEventPropagation propagation = handler->onPointerEvent(event);

                const bool canStop = event.type != UIPointerEventType::Enter && event.type != UIPointerEventType::Exit;

                if (canStop && propagation == UIEventPropagation::Stop)
                {
                    return;
                }
            }

            if (current->hasComponent<CanvasComponent>())
            {
                return;
            }

            current = current->getParent();
        }
    }

    GameObject* UISystem::getPointerTarget()
    {
        if (m_pointerTargetId.has_value() == false)
        {
            return nullptr;
        }

        for (GameObject* object : m_sceneSystem->getScene().getObjects())
        {
            if (object != nullptr && object->uuid() == *m_pointerTargetId)
            {
                return object;
            }
        }

        return nullptr;
    }

    const GameObject* UISystem::getPointerTarget() const
    {
        if (m_pointerTargetId.has_value() == false)
        {
            return nullptr;
        }

        const Scene& scene = m_sceneSystem->getScene();

        for (const GameObject* object : scene.getObjects())
        {
            if (object != nullptr && object->uuid() == *m_pointerTargetId)
            {
                return object;
            }
        }

        return nullptr;
    }

    bool UISystem::isRaycastTarget(const GameObject& object) const
    {
        for (const Component* component : object.getComponents())
        {
            if (component == nullptr || component->isEnabled() == false)
            {
                continue;
            }

            const UIRaycastTarget* raycastTarget = dynamic_cast<const UIRaycastTarget*>(component);
            if (raycastTarget != nullptr && raycastTarget->isRaycastTarget())
            {
                return true;
            }
        }

        return false;
    }

    bool UISystem::containsPoint(const UIRect& rect, const vec2& point) const
    {
        if (rect.size.x <= 0.0f ||
            rect.size.y <= 0.0f)
        {
            return false;
        }

        return
            point.x >= rect.position.x &&
            point.y >= rect.position.y &&
            point.x < rect.position.x + rect.size.x &&
            point.y < rect.position.y + rect.size.y;
    }

    GameObject* UISystem::hitTest(const vec2& framebufferPosition)
    {
        const ivec2 size =
            m_displayModule->getFramebufferSize();

        if (size.x <= 0 || size.y <= 0)
        {
            return nullptr;
        }

        const vec2 framebufferSize =
        {
            static_cast<float>(size.x),
            static_cast<float>(size.y)
        };

        vector<CanvasComponent*> canvases = m_sceneSystem->getScene().components<CanvasComponent>();

        std::stable_sort(
            canvases.begin(),
            canvases.end(),
            [](const CanvasComponent* lhs, const CanvasComponent* rhs)
            {
                return
                    lhs->getSortingOrder() <
                    rhs->getSortingOrder();
            });

        for (auto it = canvases.rbegin(); it != canvases.rend(); ++it)
        {
            CanvasComponent* canvas = *it;

            if (canvas == nullptr || canvas->isActiveAndEnabled() == false)
            {
                continue;
            }

            GameObject* hit = hitTestCanvas(*canvas, framebufferSize, framebufferPosition);

            if (hit != nullptr)
            {
                return hit;
            }
        }

        return nullptr;
    }

    GameObject* UISystem::hitTestCanvas(CanvasComponent& canvas, const vec2& framebufferSize, const vec2& framebufferPosition)
    {
        GameObject* canvasObject = canvas.getGameObject();

        if (canvasObject == nullptr)
        {
            return nullptr;
        }

        const float scale = resolveCanvasScale(canvas, framebufferSize);

        if (scale <= 0.0f)
        {
            return nullptr;
        }

        const UIRect canvasRect =
        {
            .position = {0.0f, 0.0f},
            .size = framebufferSize / scale
        };

        const vec2 pointerPosition = framebufferToCanvasPosition(canvas, framebufferSize, framebufferPosition);

        vector<GameObject*> children = canvasObject->getChildren();

        for (auto it = children.rbegin(); it != children.rend(); ++it)
        {
            GameObject* child = *it;

            if (child == nullptr)
            {
                continue;
            }

            GameObject* hit = hitTestObject(*child, canvasRect, pointerPosition);

            if (hit != nullptr)
            {
                return hit;
            }
        }

        return nullptr;
    }

    GameObject* UISystem::hitTestObject(GameObject& object, const UIRect& parentRect, const vec2& pointerPosition)
    {
        if (object.isActiveInHierarchy() == false)
        {
            return nullptr;
        }

        if (object.hasComponent<CanvasComponent>())
        {
            return nullptr;
        }

        RectTransformComponent* rectTransform = object.getComponent<RectTransformComponent>();

        UIRect resolvedRect = parentRect;

        if (rectTransform != nullptr)
        {
            resolvedRect = resolveRect(*rectTransform, parentRect);

            if (resolvedRect.size.x < 0.0f || resolvedRect.size.y < 0.0f)
            {
                return nullptr;
            }
        }

        return hitTestResolvedObject(object, resolvedRect, pointerPosition);
    }

    GameObject* UISystem::hitTestResolvedObject(GameObject& object, const UIRect& resolvedRect, const vec2& pointerPosition)
    {
        if (object.isActiveInHierarchy() == false)
        {
            return nullptr;
        }

        if (object.hasComponent<CanvasComponent>())
        {
            return nullptr;
        }

        if (HorizontalLayoutComponent* layout = object.getComponent<HorizontalLayoutComponent>(); layout != nullptr && layout->isEnabled())
        {
            const vector<ResolvedChild> children = resolveLinearLayoutChildren(object, resolvedRect, layout->getPadding(), layout->getSpacing(), layout->getChildAlignment(), LayoutAxis::Horizontal);

            for (auto it = children.rbegin(); it != children.rend(); ++it)
            {
                if (it->object == nullptr)
                {
                    continue;
                }

                GameObject* hit = hitTestResolvedObject(*it->object, it->rect, pointerPosition);

                if (hit != nullptr)
                {
                    return hit;
                }
            }
        }
        else if (VerticalLayoutComponent* layout = object.getComponent<VerticalLayoutComponent>(); layout != nullptr && layout->isEnabled())
        {
            const vector<ResolvedChild> children = resolveLinearLayoutChildren(object, resolvedRect, layout->getPadding(), layout->getSpacing(), layout->getChildAlignment(), LayoutAxis::Vertical);

            for (auto it = children.rbegin(); it != children.rend(); ++it)
            {
                if (it->object == nullptr)
                {
                    continue;
                }

                GameObject* hit = hitTestResolvedObject(*it->object, it->rect, pointerPosition);

                if (hit != nullptr)
                {
                    return hit;
                }
            }
        }
        else
        {
            vector<GameObject*> children = object.getChildren();

            for (auto it = children.rbegin(); it != children.rend(); ++it)
            {
                GameObject* child = *it;

                if (child == nullptr)
                {
                    continue;
                }

                GameObject* hit = hitTestObject( *child, resolvedRect, pointerPosition);

                if (hit != nullptr)
                {
                    return hit;
                }
            }
        }

        if (isRaycastTarget(object) == false)
        {
            return nullptr;
        }

        if (containsPoint(resolvedRect, pointerPosition) == false)
        {
            return nullptr;
        }

        return &object;
    }

    GameObject* UISystem::getPointerCapture()
    {
        if (m_pointerCaptureId.has_value() == false)
        {
            return nullptr;
        }

        for (GameObject* object : m_sceneSystem->getScene().getObjects())
        {
            if (object != nullptr && object->uuid() == *m_pointerCaptureId)
            {
                return object;
            }
        }

        return nullptr;
    }

    GameObject* UISystem::findPointerEventHandlerOwner(GameObject* target) const
    {
        for (GameObject* current = target; current != nullptr; current = current->getParent())
        {
            for (Component* component : current->getComponents())
            {
                if (component == nullptr || component->isActiveAndEnabled() == false)
                {
                    continue;
                }

                if (dynamic_cast<UIPointerEventHandler*>(component) != nullptr)
                {
                    return current;
                }
            }

            if (current->hasComponent<CanvasComponent>())
            {
                break;
            }
        }

        return nullptr;
    }

    bool UISystem::isDescendantOrSelf(const GameObject* object, const GameObject* ancestor) const
    {
        if (object == nullptr || ancestor == nullptr)
        {
            return false;
        }

        for (const GameObject* current = object; current != nullptr; current = current->getParent())
        {
            if (current == ancestor)
            {
                return true;
            }

            if (current->hasComponent<CanvasComponent>())
            {
                break;
            }
        }

        return false;
    }

}
