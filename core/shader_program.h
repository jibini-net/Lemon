#pragma once

#include <string>

#include "logger.h"
#include "resource.h"

namespace lemon
{
    class shader_program : public resource
    {
    protected:
        logger log { "Shader Program" };

    public:
        shader_program(std::shared_ptr<context> in_context) : resource(in_context)
        { }

        ~shader_program()
        { }
    };
}