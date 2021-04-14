#pragma once

#include "ext_opengl.h"

#include "core/shader_buffer.h"
#include "core/context.h"
#include "ext_glfw/ext_glfw.h"

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
    class gl_ssbo : public shader_buffer
    {
        protected:
            GLuint pointer;

            int index;

            GLenum buffer_type = GL_SHADER_STORAGE_BUFFER;

            GLenum buffer_usage = GL_DYNAMIC_COPY;

            void* mapped = nullptr;

        public:
            gl_ssbo(std::shared_ptr<context> in_context, int index);

            ~gl_ssbo();

            void* map(bool read, bool write);

            void unmap();

            void put(void* data, int size);

            void bind_base();
    };
}