#pragma once

#include <memory>
#include <functional>
#include <string>
#include <iostream>
#include <fstream>

#include "context.h"

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
