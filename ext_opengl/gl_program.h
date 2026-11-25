#pragma once

#include "ext_opengl.h"
#include "gl_context.h"

#include "core/shader_program.h"
#include "core/context.h"
#include "ext_glfw/ext_glfw.h"

namespace lemon
{
    class gl_program : public shader_program
    {
        protected:
            GLuint pointer;

            void _attach(GLenum type, std::string source, std::string name);

            void _link();

            void _use();

        public:
            gl_program(std::shared_ptr<context> in_context, std::string src_vert, std::string src_frag);

            ~gl_program();
    };
}