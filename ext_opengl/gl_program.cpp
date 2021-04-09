#include "gl_program.h"

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

namespace lemon
{
    gl_program::gl_program(context& in_context,
        std::string src_vert,
        std::string src_frag) : shader_program(in_context)
    {
        in_context.perform([&]()
        {
            this->pointer = glCreateProgram();
            this->_attach(GL_VERTEX_SHADER, src_vert, "Vertex");
            this->_attach(GL_FRAGMENT_SHADER, src_frag, "Fragment");
            this->_link();
        });
    }

    gl_program::~gl_program()
    {
        in_context->perform([&]()
        {
            glDeleteProgram(this->pointer);
        });
    }

    void gl_program::_attach(GLenum type, std::string source, std::string name)
    {
        GLuint shader = glCreateShader(type);

        const char *glsl = source.c_str();
        const int glsl_length = (int)source.length();

        glShaderSource(shader, 1, &glsl, &glsl_length);
        glCompileShader(shader);

        log.debug("Checking shader for compilation or syntax errors");

        GLint max_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        if (max_length > 0)
        {
            std::vector<GLchar> error_log(max_length);
            glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

            log.error("SHADER COMPILE ERROR:\n" + (std::string)error_log.data() + "\n");
        } else
            log.debug("Shader compiled with no error messages");

        log.debug("Shader is compiled; attaching to shader program and deleting");

        glAttachShader(pointer, shader);
        glDeleteShader(shader);
    }

    void gl_program::_link()
    {
        glLinkProgram(this->pointer);
    }

    void gl_program::_use()
    {
        glUseProgram(this->pointer);
    }
}