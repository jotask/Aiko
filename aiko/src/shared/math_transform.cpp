#include "math_transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aiko
{
    namespace math
    {

        static glm::mat4 toGlm(mat4 m)
        {
            glm::mat4 result;
            int index = 0;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result[i][j] = m(i, j);
                }
            }
            return result;
        };

        static mat4 toAiko(glm::mat4 m)
        {
            mat4 result;
            int index = 0;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result(i, j) = m[i][j];
                }
            }
            return result;
        };

        static glm::vec3 toGlm(vec3 m)
        {
            return { m.x, m.y, m.z };
        };

        static vec3 toAiko(glm::vec3 m)
        {
            return { m.x, m.y, m.z };
        };

        mat4 translate(mat4 matrix , vec3 position )
        {
            glm::mat4 m = toGlm(matrix);
            m = glm::translate( m, toGlm(position));
            return toAiko(m);
        }

        mat4 rotate(mat4 matrix, float angle, vec3 axis)
        {
            glm::mat4 m = toGlm(matrix);
            m = glm::rotate(m, angle, toGlm(axis));
            return toAiko(m);
        }

        vec3 normalize( vec3 vec )
        {
            glm::vec3 v = toGlm(vec);
            glm::vec3 n = glm::normalize(v);
            return toAiko(n);
        }

    }
}

