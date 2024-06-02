#pragma once



#include <aiko_types.h>
#include "types/color.h"

namespace aiko
{

    namespace ImGui
    {

        static void DragFloat3(const char* label, vec3& vec, float v_speed, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f)
        {
            static float tmp[3] = { 0.0f, 0.0f, 0.0f };
            tmp[0] = vec.x;
            tmp[1] = vec.y;
            tmp[2] = vec.z;
            // ImGui::DragFloat3(label, tmp, v_speed, v_min, v_max, format, power);
            vec.x = tmp[0];
            vec.y = tmp[1];
            vec.z = tmp[2];

        }

        static void ColorPicker(const char* label, Color& color, /*ImGuiColorEditFlags*/ int flags = 0)
        {
            static float tmp[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            tmp[0] = color.r;
            tmp[1] = color.g;
            tmp[2] = color.b;
            tmp[3] = color.a;
            // ImGui::ColorPicker4(label, tmp, flags);
            color.r = tmp[0];
            color.g = tmp[1];
            color.b = tmp[2];
            color.a = tmp[3];
        }

    }

}
