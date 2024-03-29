#pragma once

#include <memory>
#include <functional>
#include <string>
#include <iostream>
#include <fstream>

#include "context.h"

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
    /**
     * @brief Any context or graphical asset which can be created and destroyed.
     */
    class resource
    {
        protected:
            /**
             * @brief Context which created and uses this resource.
             */
            std::shared_ptr<context> in_context;

        public:
            /**
             * @brief Constructs a new resource object of some type.
             * @param in_context The resource is linked to this context.
             */
            resource(std::shared_ptr<context> in_context)
            {
                this->in_context = in_context;
            }

            /**
             * @brief Destroys and deallocates the resource object correctly.
             */
            ~resource()
            { }
    };

    std::string read_file(std::string path, bool aggregate = true, std::function<void(std::string)> per_line = [](auto s) { });
}
