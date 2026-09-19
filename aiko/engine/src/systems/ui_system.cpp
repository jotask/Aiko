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

        for (GameObject* child : canvasObject->getChildren())
        {
            renderObject(*child, canvasRect, scale);
        }
    }

    void UISystem::renderObject(GameObject& object, const UIRect& parentRect, float canvasScale)
    {
        if (object.hasComponent<CanvasComponent>())
        {
            return;
        }

        RectTransformComponent* rectTransform = object.getComponent<RectTransformComponent>();

        UIRect resolvedRect = parentRect;

        if (rectTransform != nullptr)
        {
            resolvedRect = resolveRect(*rectTransform, parentRect);

            if (resolvedRect.size.x < 0.0f || resolvedRect.size.y < 0.0f)
            {
                return;
            }
        }

        if (ImageComponent* image = object.getComponent<ImageComponent>())
        {
            AIKO_ASSERT(rectTransform != nullptr, "ImageComponent requires a RectTransformComponent");

            if (rectTransform != nullptr)
            {
                const vec2 physicalPosition = resolvedRect.position * canvasScale;
                const vec2 physicalSize = resolvedRect.size * canvasScale;

                if (physicalSize.x > 0.0f && physicalSize.y > 0.0f)
                {
                    if (image->getTexture() == InvalidAssetId)
                    {
                        m_renderSystem->drawUiRect(
                            physicalPosition,
                            physicalSize,
                            image->getColor());
                    }
                    else
                    {
                        m_renderSystem->drawUiImage(
                            image->getTexture(),
                            image->getTextureRegion(),
                            physicalPosition,
                            physicalSize,
                            image->getColor());
                    }
                }
            }
        }

        for (GameObject* child : object.getChildren())
        {
            renderObject(*child, resolvedRect, canvasScale);
        }
    }

}
