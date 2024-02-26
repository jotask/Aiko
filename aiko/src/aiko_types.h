#pragma once

#include <memory>
#include <type_traits>

#include "core/libs.h"
#include <glm/glm.hpp>

namespace aiko
{

    template<class T>
    using AikoPtr = std::shared_ptr<T>;

    template<class T>
    using AikoUPtr = std::unique_ptr<T>;

    // Temporal for now
    // Glm
    using vec4 = glm::vec4;
    using vec3 = glm::vec3;
    using vec2 = glm::vec2;

    // raylib
    // TODO
    // using ColorAiko = Color;

    // TODO extract to  utils

    static float map(float input, float input_start, float input_end, float output_start, float output_end)
    {
        return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
    }

}