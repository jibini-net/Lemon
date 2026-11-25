#include <math.h>

#include "mat_vec.h"

namespace lemon
{
    namespace mat
    {
        mat4 ortho(float left, float right, float bottom, float top, float near, float far)
        {
            return mat4
            {
                {
                    // Note: representation is transposed from reality
                    { 1.0f / ((right - left) / 2.0f), 0.0f, 0.0f, 0.0f },
                    { 0.0f, 1.0f / ((top - bottom) / 2.0f), 0.0f, 0.0f },
                    { 0.0f, 0.0f, 1.0f / ((near - far) / 2.0f), 0.0f },
                    { 0.0f, 0.0f, (near + far) / (near - far), 1.0f }
                }
            };
        }

        mat4 perspective(float aspect, float fov, float near, float far)
        {
            return mat4
            {
                {
                    // Note: representation is transposed from reality
                    { 1.0f / (aspect * (float)tan(fov / 2.0f)), 0.0f, 0.0f, 0.0f },
                    { 0.0f, 1.0f / (float)tan(fov / 2.0f), 0.0f, 0.0f },
                    { 0.0f, 0.0f, (near + far) / (near - far), -1.0f },
                    { 0.0f, 0.0f, (2.0f * far * near) / (near - far), 0.0f }
                }
            };
        }
    };
}