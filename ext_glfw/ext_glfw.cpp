#include "ext_glfw.h"

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
    // Initialize static context counter
    std::atomic<int> glfw_context::context_count = 0;

    glfw_context::glfw_context() : context()
    {
        // GLFW operations must be executed on the main thread
        // Operation will hang until executed
        main_thread.execute_wait([&]()
        {
            // Initialize if this is the first/only context
            if (glfw_context::context_count.fetch_add(1) == 0)
            {
                // Attempt to initialize GLFW library
                if (!glfwInit())
                    throw std::runtime_error("Could not initialize GLFW windowing library");
                
                this->log.info("Initialized global GLFW context");
            }
        });
    }

    glfw_context::~glfw_context()
    {
        // GLFW operations must be executed on the main thread
        // Operation will hang until executed
        main_thread.execute_wait([&]()
        {
            // Terminate if this is the last/only context
            if (glfw_context::context_count.fetch_sub(1) == 1)
            {
                this->log.info("Terminating global GLFW context");

                // Terminate the GLFW library
                glfwTerminate();
            }
        });
    }
}