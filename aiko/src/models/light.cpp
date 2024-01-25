#include "light.h"

Light::Light()
    : m_light(nullptr)
{
    reset();
}

void Light::update()
{

    switch (m_type)
    {
    case Light::Ambient:
    {

    }
    break;
    case Light::Directional:
    {

    }
    break;
    case Light::Point:
    {

    }
    break;
    default:
        break;
    }

}

void Light::reset()
{
    switch (m_type)
    {
    case Light::Ambient:
        {

        }
        break;
    case Light::Directional:
        {

        }
        break;
    case Light::Point:
        {

        }
        break;
    default:
        break;
    }
}