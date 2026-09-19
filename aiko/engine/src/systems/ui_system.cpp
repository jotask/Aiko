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
        const ivec2 displaySize = m_displayModule->getDisplaySize();

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
        if (object.hasComponent<CanvasComponent>())
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
        if (ImageComponent* image = object.getComponent<ImageComponent>())
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

        if (HorizontalLayoutComponent* layout = object.getComponent<HorizontalLayoutComponent>())
        {
            renderHorizontalLayout(object, resolvedRect, *layout, canvasScale, theme);

            return;
        }

        for (GameObject* child : object.getChildren())
        {
            renderObject(*child, resolvedRect, canvasScale, theme);
        }
    }

    void UISystem::renderHorizontalLayout(GameObject& object, const UIRect& resolvedRect, const HorizontalLayoutComponent& layout, float canvasScale, const UITheme& theme)
    {
        const vector<GameObject*> children = object.getChildren();

        if (children.empty())
        {
            return;
        }

        const UIPadding& padding = layout.getPadding();

        const float innerWidth = std::max(0.0f, resolvedRect.size.x - padding.left - padding.right);

        const float innerHeight = std::max(0.0f, resolvedRect.size.y - padding.top - padding.bottom);

        const float totalSpacing = layout.getSpacing() * static_cast<float>(children.size() - 1);

        const float availableWidth = std::max(0.0f, innerWidth - totalSpacing);

        float totalMinWidth = 0.0f;
        float totalPreferredWidth = 0.0f;
        float totalFlexibleWeight = 0.0f;

        for (GameObject* child : children)
        {
            const LayoutElementComponent* element = child->getComponent<LayoutElementComponent>();

            if (element == nullptr)
            {
                continue;
            }

            const float minWidth = element->getMinSize().x;
            const float preferredWidth = std::max(minWidth, element->getPreferredSize().x);

            totalMinWidth += minWidth;
            totalPreferredWidth += preferredWidth;
            totalFlexibleWeight += element->getFlexibleWeight().x;
        }

        float interpolation = 1.0f;
        float surplus = 0.0f;

        if (availableWidth < totalPreferredWidth)
        {
            const float preferredRange = totalPreferredWidth - totalMinWidth;

            if (preferredRange > 0.0f)
            {
                interpolation = std::max(0.0f, std::min( 1.0f, (availableWidth - totalMinWidth) / preferredRange));
            }
            else
            {
                interpolation = 0.0f;
            }
        }
        else
        {
            surplus = availableWidth - totalPreferredWidth;
        }

        float cursorX = resolvedRect.position.x + padding.left;

        const float contentTop = resolvedRect.position.y + padding.top;

        for (GameObject* child : children)
        {
            const LayoutElementComponent* element = child->getComponent<LayoutElementComponent>();

            vec2 minSize = {0.0f, 0.0f};
            vec2 preferredSize = {0.0f, 0.0f};
            vec2 flexibleWeight = {0.0f, 0.0f};

            if (element != nullptr)
            {
                minSize = element->getMinSize();
                preferredSize = element->getPreferredSize();
                flexibleWeight = element->getFlexibleWeight();
            }

            preferredSize.x = std::max(minSize.x, preferredSize.x);

            preferredSize.y = std::max(minSize.y, preferredSize.y);

            float width = minSize.x + (preferredSize.x - minSize.x) * interpolation;

            if (surplus > 0.0f && totalFlexibleWeight > 0.0f && flexibleWeight.x > 0.0f)
            {
                width += surplus * (flexibleWeight.x / totalFlexibleWeight);
            }

            float height = std::min(innerHeight, std::max(minSize.y, preferredSize.y));

            float positionY = contentTop;

            switch (layout.getChildAlignment())
            {
                case UICrossAxisAlignment::Start:
                    break;

                case UICrossAxisAlignment::Center:
                    positionY +=
                        (innerHeight - height) * 0.5f;
                    break;

                case UICrossAxisAlignment::End:
                    positionY += innerHeight - height;
                    break;
            }

            const UIRect childRect =
            {
                .position = {cursorX, positionY},
                .size = {width, height}
            };

            renderResolvedObject(*child, childRect, canvasScale, theme);

            cursorX += width + layout.getSpacing();
        }
    }

}
