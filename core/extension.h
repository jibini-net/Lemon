#pragma once

#include "context.h"
#include "shader_program.h"
#include "shader_buffer.h"

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
    class extension
    {
        public:
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