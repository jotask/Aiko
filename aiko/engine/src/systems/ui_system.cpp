#include "ui_system.h"

#include <cmath>

#include "system_connector.h"
#include "components/canvas_component.h"
#include "components/rect_transform_component.h"
#include "systems/scene_system.h"
#include "modules/display_module.h"
#include "modules/module_connector.h"
#include "systems/render_system.h"
#include <algorithm>

#include "components/image_component.h"
#include "models/game_object.h"
#include "scene/scene.h"
#include "components/horizontal_layout_component.h"
#include "components/vertical_layout_component.h"
#include "components/layout_element_component.h"

namespace aiko
{

    UISystem::UISystem() = default;

    void UISystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(DisplayModule, moduleConnector, m_displayModule);
        BIND_SYSTEM_REQUIRED(RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
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

        if (ImageComponent* image = object.getComponent<ImageComponent>(); image != nullptr && image->isEnabled())
        {
            RectTransformComponent* rectTransform = object.getComponent<RectTransformComponent>();

            AIKO_ASSERT(rectTransform != nullptr, "ImageComponent requires a RectTransformComponent");

            if (rectTransform != nullptr)
            {
                UIImageAppearance appearance = theme.image.appearance;

                if (image->hasColorOverride())
                {
                    appearance.color = image->getColorOverride();
                }

                const vec2 physicalPosition = resolvedRect.position * canvasScale;

                const vec2 physicalSize = resolvedRect.size * canvasScale;

                if (physicalSize.x > 0.0f && physicalSize.y > 0.0f)
                {
                    if (image->hasTexture() == false)
                    {
                        m_renderSystem->drawUiRect(physicalPosition, physicalSize, appearance.color);
                    }
                    else
                    {
                        const AssetId& textureId = image->getTextureId();

                        if (textureId != InvalidAssetId)
                        {
                            m_renderSystem->drawUiImage(textureId, image->getTextureRegion(), physicalPosition, physicalSize, appearance.color);
                        }
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

    void UISystem::renderLinearLayout(GameObject& object, const UIRect& resolvedRect, const UIPadding& padding, float spacing, UICrossAxisAlignment childAlignment, LayoutAxis axis, float canvasScale, const UITheme& theme)
    {
        const vector<GameObject*> children = object.getChildren();

        vector<GameObject*> activeChildren;
        activeChildren.reserve(children.size());

        for (GameObject* child : children)
        {
            if (child != nullptr && child->isActiveInHierarchy())
            {
                activeChildren.push_back(child);
            }
        }

        if (activeChildren.empty())
        {
            return;
        }

        const bool horizontal = axis == LayoutAxis::Horizontal;

        const float mainStartPadding =
            horizontal ? padding.left : padding.top;

        const float mainEndPadding =
            horizontal ? padding.right : padding.bottom;

        const float crossStartPadding =
            horizontal ? padding.top : padding.left;

        const float crossEndPadding =
            horizontal ? padding.bottom : padding.right;

        const float resolvedMainSize =
            horizontal ? resolvedRect.size.x : resolvedRect.size.y;

        const float resolvedCrossSize =
            horizontal ? resolvedRect.size.y : resolvedRect.size.x;

        const float innerMainSize =
            std::max(
                0.0f,
                resolvedMainSize - mainStartPadding - mainEndPadding);

        const float innerCrossSize =
            std::max(
                0.0f,
                resolvedCrossSize - crossStartPadding - crossEndPadding);

        const float totalSpacing = spacing * static_cast<float>(activeChildren.size() - 1);

        const float availableMainSize =
            std::max(0.0f, innerMainSize - totalSpacing);

        float totalMinMainSize = 0.0f;
        float totalPreferredMainSize = 0.0f;
        float totalFlexibleWeight = 0.0f;

        for (GameObject* child : activeChildren)
        {
            const LayoutElementComponent* element = child->getComponent<LayoutElementComponent>();

            if (element == nullptr || element->isEnabled() == false)
            {
                continue;
            }

            const vec2& minSize = element->getMinSize();
            const vec2& preferredSize = element->getPreferredSize();
            const vec2& flexibleWeight = element->getFlexibleWeight();

            const float minMainSize =
                horizontal ? minSize.x : minSize.y;

            const float preferredMainSize =
                std::max(
                    minMainSize,
                    horizontal ? preferredSize.x : preferredSize.y);

            const float flexibleMainWeight =
                horizontal ? flexibleWeight.x : flexibleWeight.y;

            totalMinMainSize += minMainSize;
            totalPreferredMainSize += preferredMainSize;
            totalFlexibleWeight += flexibleMainWeight;
        }

        float interpolation = 1.0f;
        float surplus = 0.0f;

        if (availableMainSize < totalPreferredMainSize)
        {
            const float preferredRange =
                totalPreferredMainSize - totalMinMainSize;

            if (preferredRange > 0.0f)
            {
                interpolation = std::clamp(
                    (availableMainSize - totalMinMainSize) /
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
                availableMainSize - totalPreferredMainSize;
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
            const LayoutElementComponent* element = child->getComponent<LayoutElementComponent>();

            vec2 minSize = {0.0f, 0.0f};
            vec2 preferredSize = {0.0f, 0.0f};
            vec2 flexibleWeight = {0.0f, 0.0f};

            if (element != nullptr && element->isEnabled())
            {
                minSize = element->getMinSize();
                preferredSize = element->getPreferredSize();
                flexibleWeight = element->getFlexibleWeight();
            }

            const float minMainSize =
                horizontal ? minSize.x : minSize.y;

            const float preferredMainSize =
                std::max(
                    minMainSize,
                    horizontal ? preferredSize.x : preferredSize.y);

            const float flexibleMainWeight =
                horizontal ? flexibleWeight.x : flexibleWeight.y;

            float allocatedMainSize =
                minMainSize +
                (preferredMainSize - minMainSize) * interpolation;

            if (surplus > 0.0f &&
                totalFlexibleWeight > 0.0f &&
                flexibleMainWeight > 0.0f)
            {
                allocatedMainSize +=
                    surplus *
                    (flexibleMainWeight / totalFlexibleWeight);
            }

            const float minCrossSize =
                horizontal ? minSize.y : minSize.x;

            const float preferredCrossSize =
                std::max(
                    minCrossSize,
                    horizontal ? preferredSize.y : preferredSize.x);

            const float allocatedCrossSize =
                std::min(innerCrossSize, preferredCrossSize);

            float crossPosition = crossStart;

            switch (childAlignment)
            {
                case UICrossAxisAlignment::Start:
                    break;

                case UICrossAxisAlignment::Center:
                    crossPosition +=
                        (innerCrossSize - allocatedCrossSize) * 0.5f;
                    break;

                case UICrossAxisAlignment::End:
                    crossPosition +=
                        innerCrossSize - allocatedCrossSize;
                    break;
            }

            UIRect childRect;

            if (horizontal)
            {
                childRect =
                {
                    .position = {cursor, crossPosition},
                    .size = {allocatedMainSize, allocatedCrossSize}
                };
            }
            else
            {
                childRect =
                {
                    .position = {crossPosition, cursor},
                    .size = {allocatedCrossSize, allocatedMainSize}
                };
            }

            renderResolvedObject(
                *child,
                childRect,
                canvasScale,
                theme);

            cursor += allocatedMainSize + spacing;
        }
    }

}
