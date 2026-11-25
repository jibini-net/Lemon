#pragma once

#include "context.h"
#include "shader_program.h"
#include "shader_buffer.h"

namespace lemon
{
    class extension
    {
        public:
            virtual ~extension()
            { }

            virtual std::shared_ptr<context> create_context()
            { return std::shared_ptr<context>(); }

            virtual std::shared_ptr<shader_program> create_program(
                std::shared_ptr<context> in_context,
                std::string vert_src,
                std::string frag_src)
            { return std::shared_ptr<shader_program>(); }

            virtual std::shared_ptr<shader_buffer> create_buffer(
                std::shared_ptr<context> in_context,
                unsigned int index)
            { return std::shared_ptr<shader_buffer>(); }
    };
}