#pragma once

#include "core/extension.h"

#include "gl_context.h"
#include "gl_ssbo.h"
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
    class ext_opengl : public extension
    {
        protected:
            unsigned int version;
            bool core, forward_compat;

        public:
            ext_opengl(unsigned int major, unsigned int minor, bool core, bool forward_compat)
            {
                this->version = major * 10 + minor;
                this->core = core;
                this->forward_compat = forward_compat;
            }

            std::shared_ptr<context> create_context();

            std::shared_ptr<shader_program> create_program(
                std::shared_ptr<context> in_context,
                std::string vert_src, 
                std::string frag_src);

            std::shared_ptr<shader_buffer> create_buffer(
                std::shared_ptr<context> in_context,
                unsigned int index);
    };
}