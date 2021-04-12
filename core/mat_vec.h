#pragma once

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

#define MESH_BLOCK_SIZE 16392 * 3

namespace lemon
{
    /**
     * @brief A two-dimensional floating point vector.
     * @author Zach Goethel
     */
    struct vec2
    {
        float x, y;

        vec2 operator +(vec2& other);
        vec2 operator -(vec2& other);
        vec2 operator *(float& scale);

        /**
         * @brief Performs a cross product with the provided vector.
         * @return The resulting cross product vector.
         */
        vec2 cross(vec2& other);

        /**
         * @brief Performs a dot product with the provided vector.
         * @return The resulting dot product value.
         */
        float dot(vec2& other);
    };
    
    /**
     * @brief A three-dimensional floating point vector.
     * @author Zach Goethel
     */
    struct vec3 : vec2
    {
        float z;

        vec3 operator +(vec3& other);
        vec3 operator -(vec3& other);
        vec3 operator *(float& scale);

        /**
         * @brief Performs a cross product with the provided vector.
         * @return The resulting cross product vector.
         */
        vec3 cross(vec3& other);

        /**
         * @brief Performs a dot product with the provided vector.
         * @return The resulting dot product value.
         */
        float dot(vec3& other);
    };
    
    /**
     * @brief A four-dimensional floating point vector.
     * @author Zach Goethel
     */
    struct vec4 : vec3
    {
        float w;

        vec4 operator +(vec4& other);
        vec4 operator -(vec4& other);
        vec4 operator *(float& scale);

        /**
         * @brief Performs a cross product with the provided vector.
         * @return The resulting cross product vector.
         */
        vec4 cross(vec4& other);

        /**
         * @brief Performs a dot product with the provided vector.
         * @return The resulting dot product value.
         */
        float dot(vec4& other);
    };

    /**
     * @brief A four-by-four floating point matrix.
     * @author Zach Goethel
     */
    struct mat4
    {
        float values[4][4] =
        {
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        vec4 operator *(mat4& other);
        vec4 operator *(vec4& vec);
    };

    namespace mat
    {
        /**
         * @brief Creates an orthographic projection matrix.
         * @param left The left edge of the orthographic space.
         * @param right The right edge of the orthographic space.
         * @param bottom The bottom edge of the orthographic space.
         * @param top The top edge of the orthographic space.
         * @param near The near-clipping plane of the orthographic space.
         * @param far The far-clipping plane of the orthographic space.
         * @return The newly created orthographic matrix.
         */
        mat4 ortho(float left, float right, float bottom, float top, float near, float far);

        /**
         * @brief Creates a perspective projection matrix.
         * @param aspect The aspect ratio of the screen (width / height).
         * @param fov The field of view of the projection in radians.
         * @param near The near-clipping plane of the projected space.
         * @param far The far-clipping plane of the projected space.
         * @return The newly created projection matrix.
         */
        mat4 perspective(float aspect, float fov, float near, float far);
    };
}