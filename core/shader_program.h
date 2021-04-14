#pragma once

#include <string>

#include "logger.h"
#include "resource.h"

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