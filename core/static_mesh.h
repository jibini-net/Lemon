#pragma once

#include "mat_vec.h"

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

#define MESH_BLOCK_SIZE 65568 * 3
#define MESH_MAX_BODIES 2048

namespace lemon
{
    /**
     * A structural definition of the an element in the geometry buffer which is
     * being rendered.  Each vertex has a position, diffuse color, normal vector,
     * and a texture coordinate.
     * 
     * @brief Vertex structure as defined in video memory buffers.
     * @author Zach Goethel
     */
    struct vertex
    {
        /**
         * Three-dimensional weighted position of this vertex (x, y, z, and w).
         */
        vec4 position;

        /**
         * Color of this vertex.  This will be linearly interpolated at each
         * fragment which is rendered (r, g, b, and a).
         */
        vec4 diffuse;

        /**
         * Normal vector of this vertex which defines which direction this vertex is
         * facing in three-dimensional space.  This will be linearly interpolated at
         * each fragment which is rendered (x, y, and z).
         */
        vec3 normal_vector;
        float __padding_0[1];

        /**
         * The texture coordinate of this vertex which will be used to sample the
         * texture which is currently bound.  This will be linearly interpolated at
         * each fragment which is rendered (s and t).
         */
        vec2 texture_coord;

        /**
         * Index of the body to which this vertex belongs.  This will provide
         * material and transformation data for this vertex.
         */
        unsigned int body_index;
        float __padding_1[1];
    };

    /**
     * This shader buffer contains all vertices which are being rendered on this
     * render pass.  The vertex shader will be invoked once per vertex, and each
     * invocation can access the correct vertex via the vertex array.  The index
     * will be the GLSL vertex ID.
     * 
     * @brief Mesh structure as defined in video memory buffers.
     * @author Zach Goethel
     */
    struct render_data
    { 
        /**
         * How many vertices are held in this vertex buffer.  The vertex shader will
         * be invoked this many times.  This is also the size of the vertex array.
         */
        unsigned int num_vertices = MESH_BLOCK_SIZE;
        float __padding[3];

        /**
         * A contiguous vertex buffer which contains all vertices rendered in this
         * render pass.  Each vertex shader invocation should access the correct
         * vertex indexed at the current vertex ID.
         */
        vertex vertices[MESH_BLOCK_SIZE];
    };

    /**
     * A single discrete static mesh body of a particular material.  Each body can
     * have its own local transforms and material data.
     * 
     * @brief Body structure as defined in video memory buffers.
     * @author Zach Goethel
     */
    struct body
    {
        /**
         * This body's local transformation matrix.
         */
        mat4 transform;

        /**
         * The diffuse color of this body's material.
         */
        vec4 diffuse;

        /**
         * Coefficient of diffuse lighting on this body.
         */
        float diff;

        /**
         * Coefficient of specular lighting on this body.
         */
        float spec;

        /**
         * Applied specular lighting exponential power.
         */
        float spec_power;

        /**
         * Constant for ambient lighting on this body.
         */
        float ambient;
    };

    /**
     * This buffer will contain all of the bodies, local transforms, and material
     * data for those bodies.
     * 
     * @brief Mesh body structure as defined in video memory buffers.
     * @author Zach Goethel
     */
    struct body_data
    {
        /**
         * The number of bodies which may be represented in this render pass.
         */
        unsigned int num_bodies = 0;
        float __padding[3];

        /**
         * Array of all present bodies in memory.
         */
        body bodies[MESH_MAX_BODIES];
    };
};
