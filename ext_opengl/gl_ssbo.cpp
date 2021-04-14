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
    gl_ssbo::gl_ssbo(std::shared_ptr<context> in_context, int index) : shader_buffer(in_context)
    {
        this->index = index;

        this->in_context->perform([&]()
        {
            glGenBuffers(1, &this->pointer);
        }, true);
    }

    gl_ssbo::~gl_ssbo()
    {
        this->in_context->perform([&]()
        {
            glBindBufferBase(buffer_type, index, GL_NONE);
            glDeleteBuffers(1, &this->pointer);
        }, true);
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

        void*& mapped = this->mapped;
        this->in_context->perform([&]()
        {
            if (mapped == nullptr)
            {
                glBindBuffer(buffer_type, pointer);
                mapped = glMapBuffer(buffer_type, access);
            }
        }, true);
        
        return mapped;
    }

    void gl_ssbo::unmap()
    {
        auto buffer_type = this->buffer_type;
        auto pointer = this->pointer;
        void*& mapped = this->mapped;

        this->in_context->perform([=, &mapped]()
        {
            if (mapped != nullptr)
            {
                glBindBuffer(buffer_type, pointer);
                glUnmapBuffer(buffer_type);
                mapped = nullptr;
            }
        });
    }

    void gl_ssbo::put(void* data, int size)
    {
        auto buffer_type = this->buffer_type;
        auto buffer_usage = this->buffer_usage;
        auto pointer = this->pointer;

        this->in_context->perform([=]()
        {
            glBindBuffer(buffer_type, pointer);
            glBufferData(buffer_type, size, data, buffer_usage);
        });
    }

    void gl_ssbo::bind_base()
    {
        auto buffer_type = this->buffer_type;
        auto index = this->index;
        auto pointer = this->pointer;

        this->in_context->perform([=]()
        {
            glBindBufferBase(buffer_type, index, pointer);
        });
    }
}