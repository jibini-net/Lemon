#pragma once

#include "GLFW/glfw3.h"

#include <exception>

#include "core/context.h"
#include "core/logger.h"
#include "core/bootstrap.h"

namespace lemon
{
    /**
     * @brief GLFW windowing context extension implementation.
     * @author Zach Goethel
     */
    class glfw_context : public context
    {
        private:
            logger log { "GLFW Context" };
        
        public:
            /**
             * The number of contexts maintained by GLFW or requiring GLFW to be
             * active.  Upon creation of a context, this value will increment.
             * Upon destruction of a context, this value will decrement.
             * 
             * If this value is zero, construction of GLFW will initialize the
             * library.  It will not be terminated until this counter is zero.
             * 
             * @brief Tracks the number of contexts managed under GLFW.
             */
            static std::atomic<int> context_count;

            /**
             * @brief Constructs a new GLFW context object.
             */
            glfw_context();

            /**
             * @brief Destroys the GLFW context object.
             */
            ~glfw_context();
    };
}