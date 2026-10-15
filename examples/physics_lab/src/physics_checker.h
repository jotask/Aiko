#pragma once

#include "aiko_physics_types.h"
#include "core/transform.h"
#include "math/math_vector.h"

#include <string>

namespace aiko::lab
{
    bool near(const vec3& a, const vec3& b, float tolerance = 0.003f);
    bool samePose(const Transform& a, const Transform& b);
    bool contains(const std::vector<physics::BodyId>& ids, physics::BodyId id);
    bool check(bool condition, const std::string& name, int& checksPassed, int& checksFailed, std::vector<std::string>& failures);

    void runChecks(int& checksPassed, int& checksFailed, std::vector<std::string>& failures);
}
