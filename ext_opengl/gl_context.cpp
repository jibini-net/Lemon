#include "gl_context.h"

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
    void GLAPIENTRY _gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const GLchar* message, const void* userParam )
    {
        std::lock_guard<std::mutex> lock(_log_mutex());
        std::cerr << message << std::endl;
    }

    gl_context::gl_context(
        int major, int minor,
        bool core,
        bool forward_compat
    ) : context()
    {
        log.info("Creating a new OpenGL context and pipelines ("
            + std::to_string(major)
            + "."
            + std::to_string(minor)
            + (core ? " core" : "")
            + ")");

        // GLFW window must be created on main thread
        main_thread.execute_wait([&]()
        {
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
            glewExperimental = major * 10 + minor >= 33;
            glfwWindowHint(GLFW_SAMPLES, 16);

            // Create new window handle and make current
            this->window_handle = glfwCreateWindow(1400, 900, "Lemon", NULL, NULL);
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
        auto window_handle = this->window_handle;
        this->perform([=]()
        {
            glfwMakeContextCurrent(window_handle);

            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(_gl_message_callback, 0);
        });
    }

    gl_context::~gl_context()
    {
        log.info("Destroying OpenGL context and related resources");

        // GLFW windows must be destroyed on the main thread
        auto window_handle = this->window_handle;
        main_thread.execute_wait([=]()
        {
            glfwDestroyWindow(window_handle);
        });
    }

    void gl_context::update()
    {
        this->perform([&]()
        {
            glFlush();

            this->should_close |= (bool)glfwWindowShouldClose(this->window_handle);
            glfwSwapInterval(false);
            glfwSwapBuffers(this->window_handle);

            glClear(GL_COLOR_BUFFER_BIT);
        }, true);
    }

    bool gl_context::is_alive()
    {
        return !this->should_close;
    }

    void gl_context::kill()
    {
        this->should_close = true;
    }
}
