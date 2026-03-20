#include "sort_lab.h"

#include <algorithm>
#include <format>

#include <time/time.h>
#include <aiko_includes.h>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "../../../aiko/renderer/src/models/camera.h"

#include "algorithm/bubble_sort.h"
#include "algorithm/insertion_sort.h"
#include "algorithm/selection_sort.h"

namespace sb
{
    void SortLab::init()
    {
        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        cam->setCameraType(aiko::Camera::CameraType::Orthographic);
		camera->transform().position = { 0.0f, 0.0f, 25.0f };
		cam->getCamera().position = camera->transform().position;

        setup();

        clear();
        shuffle();

    }

    void SortLab::update()
    {
        if (app->isKeyJustPressed(aiko::Key::KEY_SPACE))
        {
            clear();
            shuffle();
            m_sorters[m_currentSorterIdx]->init();
        }

        if (app->isKeyJustPressed(aiko::Key::KEY_RIGHT))
        {
            nextSorter(+1);
        }
        if (app->isKeyJustPressed(aiko::Key::KEY_LEFT))
        {
            nextSorter(-1);
        }

        if (isSorted() == true)
        {
            aiko::logger::Log::warning("Done sorting");
            return;
        }

        m_timer += ::aiko::Time::it().getDeltaTime();
        if (m_timer >= UPDATE_STEP_TIME)
        {
            m_sorters[m_currentSorterIdx]->step(m_numbers);
            m_timer -= UPDATE_STEP_TIME; // keeps it stable even with frame drops
        }
    }

    void SortLab::render()
    {

        constexpr float w = 0.25f;

        const float initial_position_x = ((w * m_numbers.size()) / 2.0f) * -1.0f;

        for (uint16_t i = 0 ; i < m_numbers.size(); ++i)
        {
            const NUMBER number = m_numbers[i];
            const float h = aiko::math::map<float>(number, 0, m_numbers.size(), 1.0f, 10.0f);
            const aiko::vec3 size = {w, h, 0.0f};
            const aiko::vec3 pos = {initial_position_x + ( i * w), 0.0f, 0.0f};
            app->getRenderSystem()->renderRectangle(pos, size);
        }

    }

    void SortLab::nextSorter(int dir)
    {
        m_sorters[m_currentSorterIdx]->dispose();
        m_currentSorterIdx += dir;
        m_currentSorterIdx %= m_sorters.size();
        m_sorters[m_currentSorterIdx]->init();
        clear();
        shuffle();
    }

    bool SortLab::isSorted() const
    {
        for (uint16_t i = 0; i < m_numbers.size() - 1; ++i )
        {
            if (m_numbers[i] > m_numbers[i + 1])
            {
                return false;
            }
        }
        return true;
    }

    void SortLab::setup()
    {
        m_timer = 0.0f;
        m_currentSorterIdx = 0;
        m_sorters.clear();
        m_sorters.emplace_back(std::make_shared<InsertionSort>(this));
        m_sorters.emplace_back(std::make_shared<BubbleSort>(this));
        m_sorters.emplace_back(std::make_shared<SelectionSort>(this));
        m_sorters[m_currentSorterIdx]->init();
    }

    void SortLab::shuffle()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::ranges::shuffle(m_numbers.begin(), m_numbers.end(), g);
    }

    void SortLab::clear()
    {
        for (uint16_t i = 0 ; i < m_numbers.size(); ++i)
        {
            m_numbers[i] = i;
        }
    }

    void SortLab::printArray()
    {
        aiko::string str;
        str.append("[");
        for (size_t i = 0 ; i < m_numbers.size(); ++i)
        {
            str.append(std::format("{}{}", m_numbers[i], (i + 1 < m_numbers.size() ? ", " : "")));
        }
        str.append("]");
        ::aiko::logger::Log::info(str);
    }
}

