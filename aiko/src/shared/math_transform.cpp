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

        float radians(float angle)
        {
            return glm::radians(angle);
        }

        vec3 cross(vec3 a, vec3 b)
        {
            glm::vec3 aa = toGlm(a);
            glm::vec3 bb = toGlm(b);
            glm::vec3 n = glm::cross(aa, bb);
            return toAiko(n);
        }

        vec3 normalize(vec3 vec)
        {
            glm::vec3 v = toGlm(vec);
            glm::vec3 n = glm::normalize(v);
            return toAiko(n);
        }

        mat4 scale(mat4 mat, vec3 scale)
        {
            glm::mat4 m = toGlm(mat);
            glm::vec3 s = toGlm(scale);
            glm::mat4 result = glm::scale(m, s);
            return toAiko(result);
        }

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

        mat4 lookAt(vec3 position, vec3 target, vec3 up)
        {
            glm::mat4 mat = glm::mat4(1.0f);
            mat = glm::lookAt( toGlm(position), toGlm(target), toGlm(up));
            return toAiko(mat);
        }

        mat4 perspective( float angle, float width, float height, float min, float max)
        {
            glm::mat4 projection = glm::perspective( radians(angle), width / height, min, max);
            return toAiko(projection);
        }

    }
}

