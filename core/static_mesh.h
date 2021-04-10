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

#define MESH_BLOCK_SIZE 2048 * 3

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
         * Position of this vertex (x, y, z, and w).
         */
        float position[4];

        /**
         * Color of this vertex.  This will be linearly interpolated at each
         * fragment which is rendered.
         */
        float diffuse[4];

        /**
         * Normal vector of this vertex which defines which direction this vertex is
         * facing in three-dimensional space.  This will be linearly interpolated at
         * each fragment which is rendered.
         */
        float normal_vector[3];
        float __padding0[1];

        /**
         * The texture coordinate of this vertex which will be used to sample the
         * texture which is currently bound.  This will be linearly interpolated at
         * each fragment which is rendered.
         */
        float texture_coord[2];
        float __padding1[2];
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
        int num_vertices = MESH_BLOCK_SIZE;
        float __padding0[3];

        /**
         * A contiguous vertex buffer which contains all vertices rendered in this
         * render pass.  Each vertex shader invocation should access the correct
         * vertex indexed at the current vertex ID.
         */
        vertex vertices[MESH_BLOCK_SIZE];
    };
};
