#include "ext_opengl.h"

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
    gl_context::gl_context(
        int major, int minor,
        bool core,
        bool forward_compat
    ) : context()
    {
        // Maintain a global GLFW context (at least one must exist)
        this->resources.attach(&this->global_glfw);

        log.info("Creating a new OpenGL context and pipelines ("
            + std::to_string(major)
            + "."
            + std::to_string(minor)
            + (core ? " core" : "")
            + ")");

        glfwDefaultWindowHints();
        // Set the context version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
        // Set the core flag (limits legacy features)
        if (core)
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Set the forward compatibility flag
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, forward_compat);
        // Enable experimental GLEW features for newer OpenGL versions
        if (major * 10 + minor >= 33)
            glewExperimental = true;

        // GLFW window must be created on main thread
        main_thread.execute_wait([&]()
        {
            // Create new window handle and make current
            this->window_handle = glfwCreateWindow(800, 600, "Lemon", NULL, NULL);
            glfwMakeContextCurrent(this->window_handle);

            // Initialize GLEW context bindings
            if (glewInit() != GLEW_OK)
            {
                log.error("GLEW failed to initialize; check system requirements and libraries");

                glfwTerminate();
                throw std::exception();
            }

            log.debug("Successfully initialized GLEW bindings for current context");
            // Relinquish the context from the main thread
            glfwMakeContextCurrent(NULL);
        });

        // Make this context current in the dedicated worker thread
        this->perform([=]()
        {
            glfwMakeContextCurrent(this->window_handle);
        });
    }

    gl_context::~gl_context()
    {
        log.info("Destroying OpenGL context and related resources");

        // GLFW windows must be destroyed on the main thread
        main_thread.execute_wait([=]()
        {
            glfwDestroyWindow(this->window_handle);
        });
    }
}
