#include "sort_lab.h"

#include <algorithm>

#include <time/time.h>
#include <aiko_includes.h>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "models/camera.h"

#include "algorithm/bubble_sort.h"
#include "algorithm/insertion_sort.h"
#include "algorithm/selection_sort.h"

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

        setup();

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
            m_sorters[m_currentSorterIdx]->init();
        }

        if (isKeyJustPressed(aiko::Key::KEY_RIGHT))
        {
            nextSorter(+1);
        }
        if (isKeyJustPressed(aiko::Key::KEY_LEFT))
        {
            nextSorter(-1);
        }

        if (isSorted() == true)
        {
            aiko::logger::Log::warning("Done sorting");
        }
        else
        {
            m_timer += ::aiko::Time::it().getDeltaTime();
            if (m_timer >= UPDATE_STEP_TIME)
            {
                m_sorters[m_currentSorterIdx]->step(m_numbers);
                m_timer -= UPDATE_STEP_TIME; // keeps it stable even with frame drops
            }
        }
    }

    void SortLab::render()
    {
        Application::render();

        const float w = 0.40f;

        for (uint16_t i = 0 ; i < m_numbers.size(); ++i)
        {
            const NUMBER number = m_numbers[i];

            const float h = aiko::math::map<float>(number, 0, m_numbers.size(), 1.0f, 10.0f);

            const aiko::vec3 size = {w, h, 0.0f};
            aiko::vec3 pos = {size.x / 2.0f - i * size.x, 0.0f, 0.0f};

            getRenderSystem()->drawRectangle(pos, size);
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
        m_sorters.emplace_back(std::make_shared<BubbleSort>(this));
        m_sorters.emplace_back(std::make_shared<SelectionSort>(this));
        m_sorters.emplace_back(std::make_shared<InsertionSort>(this));
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
}

