// Use a modern OpenGL 4 core profile
#version 430 core

// Define type precision levels
precision highp float;
precision mediump int;

/**
 * A structural definition of the an element in the geometry buffer which is
 * being rendered.  Each vertex has a position, diffuse color, normal vector,
 * and a texture coordinate.
 */
struct vertex
{
    /**
     * Position of this vertex (x, y, z, and w).
     */
    vec4 position;

    /**
     * Color of this vertex.  This will be linearly interpolated at each
     * fragment which is rendered.
     */
    vec4 diffuse;

    /**
     * Normal vector of this vertex which defines which direction this vertex is
     * facing in three-dimensional space.  This will be linearly interpolated at
     * each fragment which is rendered.
     */
    vec3 normal_vector;

    /**
     * The texture coordinate of this vertex which will be used to sample the
     * texture which is currently bound.  This will be linearly interpolated at
     * each fragment which is rendered.
     */
    vec2 texture_coord;
};

/**
 * This shader buffer contains all vertices which are being rendered on this
 * render pass.  The vertex shader will be invoked once per vertex, and each
 * invocation can access the correct vertex via the vertex array.  The index
 * will be the GLSL vertex ID.
 */
layout (std430, binding = 0) buffer render_data
{ 
    /**
     * How many vertices are held in this vertex buffer.  The vertex shader will
     * be invoked this many times.  This is also the size of the vertex array.
     */
	int num_vertices;

    /**
     * A contiguous vertex buffer which contains all vertices rendered in this
     * render pass.  Each vertex shader invocation should access the correct
     * vertex indexed at the current vertex ID.
     */
	vertex vertices[];
};

// Linearly interpolated input fields
in vec4 diffuse;
in vec3 normal_vector;
in vec2 texture_coord;

// Fragment color output field
out vec4 color;

// Uniform bound texture sampler (must be set)
uniform sampler2D texture;

void main()
{
    // Set the output fragment to a modulated sampled color
    color = diffuse;// * texture2D(texture, texture_coord);
}
