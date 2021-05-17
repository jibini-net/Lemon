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
     * Three-dimensional weighted position of this vertex (x, y, z, and w).
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

    /**
     * Index of the body to which this vertex belongs.  This will provide
     * material and transformation data for this vertex.
     */
    uint body_index;
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
	uint num_vertices;

    /**
     * A contiguous vertex buffer which contains all vertices rendered in this
     * render pass.  Each vertex shader invocation should access the correct
     * vertex indexed at the current vertex ID.
     */
	vertex vertices[];
};

/**
 * A single discrete static mesh body of a particular material.  Each body can
 * have its own local transforms and material data.
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
 */
layout (std430, binding = 1) buffer body_data
{
    /**
     * The number of bodies which may be represented in this render pass.
     */
    uint num_bodies;

    /**
     * Array of all present bodies in memory.
     */
    body bodies[];
};

// Linearly interpolated input fields
in vec3 position;
in vec4 diffuse;
in vec3 normal_vector;
in vec2 texture_coord;
// Flat integer input fields
in flat uint body_index;

// Fragment color output field
out vec4 color;

// Uniform bound texture sampler (must be set)
uniform sampler2D texture;

void main()
{
    body b = bodies[body_index];
    // Set the output fragment to a modulated sampled color
    float c = abs(normal_vector.z);

    float a = b.ambient;
    float d = b.diff * pow(c, 2.0);
    float s = b.spec * pow(c, b.spec_power);

    s -= max(0.0, pow(normal_vector.y, 2)) * 1.2;

    color = vec4(vec3(a + max(0.0, d + s)), 1.0) * diffuse;// * texture2D(texture, texture_coord);

    color.r += max(0.0, pow(normal_vector.y, 5)) * 0.3;
    color.g += max(0.0, pow(normal_vector.y, 5)) * 0.2;
}
