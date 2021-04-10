#include "gl_ssbo.h"

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
    gl_ssbo::gl_ssbo(context& in_context, int index) : shader_buffer(in_context)
    {
        this->index = index;

        this->in_context->perform([&]()
        {
            glGenBuffers(1, &this->pointer);
        });
    }

    gl_ssbo::~gl_ssbo()
    {
        this->in_context->perform([&]()
        {
            glBindBufferBase(buffer_type, index, GL_NONE);
            glDeleteBuffers(1, &this->pointer);
        });
    }

    void* gl_ssbo::map(bool read, bool write)
    {
        GLenum access;

        if (read && write)
            access = GL_READ_WRITE;
        else if (read)
            access = GL_READ_ONLY;
        else if (write)
            access = GL_WRITE_ONLY;
        else
        {
            this->log.error("Attempted to map a buffer with neither read nor write enabled");
            return nullptr;
        }

        void* address;

        this->in_context->perform([&]()
        {
            glBindBuffer(buffer_type, pointer);
            address = glMapBuffer(buffer_type, access);
        });
        
        return address;
    }

    void gl_ssbo::unmap()
    {
        this->in_context->perform([&]()
        {
            glBindBuffer(buffer_type, pointer);
            glUnmapBuffer(buffer_type);
        });
    }

    void gl_ssbo::put(void* data, int size)
    {
        this->in_context->perform([&]()
        {
            glBindBuffer(buffer_type, pointer);
            glBufferData(buffer_type, size, data, buffer_usage);

            glBindBufferBase(buffer_type, index, pointer);
        });
    }

    void gl_ssbo::bind_base()
    {
        this->in_context->perform([&]()
        {
            glBindBufferBase(buffer_type, index, pointer);
        });
    }
}