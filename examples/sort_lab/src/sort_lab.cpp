#include "sort_lab.h"

#include <cmath>
#include <algorithm>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/mesh.h"
#include "components/texture_component.h"
#include "components/model_component.h"
#include "models/camera.h"
#include "types/color.h"

#include <aiko_includes.h>

namespace sb
{
    void SortLab::init()
    {
        Application::init();

        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        cam->setCameraType(aiko::Camera::CameraType::Orthographic);
		camera->transform().position = { 0.0f, 0.0f, 10.0f };
		cam->getCamera().position = camera->transform().position;

        clear();
        shuffle();

    }

    void SortLab::update()
    {
        Application::update();
        if (isKeyJustPressed(aiko::Key::KEY_SPACE))
        {
            clear();
            shuffle();
        }
    }

    void SortLab::render()
    {
        Application::render();

        const float w = 0.40f;

        for (uint16_t i = 0 ; i < numbers.size(); ++i)
        {
            const NUMBER number = numbers[i];

            const float h = aiko::math::map<float>(number, 0, numbers.size(), 1.0f, 10.0f);

            const aiko::vec3 size = {w, h, 0.0f};
            const aiko::vec3 pos = {size.x / 2.0f - i * size.x, 0.0f, 0.0f};

            getRenderSystem()->drawRectangle(pos, size);
        }

    }

    void SortLab::shuffle()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::ranges::shuffle(numbers.begin(), numbers.end(), g);
    }

    void SortLab::clear()
    {
        for (uint16_t i = 0 ; i < numbers.size(); ++i)
        {
            numbers[i] = i;
        }
    }

    void SortLab::sort()
    {
    }

}

