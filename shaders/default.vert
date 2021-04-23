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


// Linearly interpolated output fields
out vec3 position;
out vec4 diffuse;
out vec3 normal_vector;
out vec2 texture_coord;
// Flat integer output fields
out flat uint body_index;

// Uniform bound projection matrix
uniform mat4 m_project;
// Uniform bound global transforms
uniform mat4 m_model;
// Uniform bound texture sampler (must be set)
uniform sampler2D texture;

uniform float time;

void main()
{
    // Fetch the current vertex object
    vertex v = vertices[gl_VertexID];
    // Fetch the current vertex's body
    body_index = v.body_index;
    body b = bodies[v.body_index];

    // Set the interpolated fields
    position = (/* b.transform * m_model * */ v.position).xyz;
    diffuse = v.diffuse * b.diffuse;
    normal_vector = v.normal_vector;
    texture_coord = v.texture_coord;

    vec4 pos = v.position;
    float len = length(pos.xz);
    float angle = atan(pos.z, pos.x);
    pos.x = -len * sin(angle + time);
    pos.z = len * cos(angle + time);

    pos.y -= 80.0;
    pos.z -= 90.0;

    // pos.z -= 110.0;

    // pos.y -= 100.0;
    // pos.z -= 220.0;

    len = length(normal_vector.xz);
    angle = atan(normal_vector.z, normal_vector.x);
    normal_vector.x = -len * sin(angle + time);
    normal_vector.z = len * cos(angle + time);

    // Set the static vertex position field
    gl_Position = b.transform /* * m_model * m_project */ * pos;
}
