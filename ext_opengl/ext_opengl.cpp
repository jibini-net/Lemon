#include "ext_opengl.h"

namespace lemon
{
    std::shared_ptr<context> ext_opengl::create_context()
    {
        return std::shared_ptr<context>(new gl_context(version / 10, version % 10, core, forward_compat));
    }

    std::shared_ptr<shader_program> ext_opengl::create_program(
        std::shared_ptr<context> in_context,
        std::string vert_src, 
        std::string frag_src)
    {
        return std::shared_ptr<shader_program>(new gl_program(in_context, vert_src, frag_src));
    }

    std::shared_ptr<shader_buffer> ext_opengl::create_buffer(
        std::shared_ptr<context> in_context,
        unsigned int index)
    {
        return std::shared_ptr<shader_buffer>(new gl_ssbo(in_context, index));
    }
}