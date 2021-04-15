#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "ext_glfw/ext_glfw.h"

#include "core/context.h"

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
     * @brief Managed OpenGL context built on GLFW windows and GLEW bindings.
     */
    class gl_context : public context
    {
    private:
        /**
         * @brief Keep at least one global GLFW context alive.
         */
        glfw_context global_glfw;

        /**
         * @brief The GLFW window associated with this context.
         */
        GLFWwindow* window_handle = nullptr;

        /**
         * @brief OpenGL-context-specific logger instance.
         */
        logger log { "OpenGL" };

        /**
         * @brief A static flag of whether this context should close.
         */
        bool should_close = false;
    
    public:
        /**
         * @brief Construct a new OpenGL context with the given features.
         * 
         * @param major Major OpenGL version (e.g., 4 for OpenGL 4.3).
         * @param minor Minor OpenGL version (e.g., 3 for OpenGL 4.3).
         * @param core Whether this is a core profile (limits legacy features).
         * @param forward_compat Whether this context should be forward compat.
         */
        gl_context(int major, int minor, bool core, bool forward_compat);

        /**
         * @brief Destroy the OpenGL context and associated window/resources.
         */
        ~gl_context();

        /**
         * @brief Updates the context, swaps the framebuffer, and polls input.
         */
        void update();

        /**
         * @brief Checks whether this context is currently alive.
         * 
         * @return true If this context is still active and alive.
         * @return false If this context should be shut down.
         */
        bool is_alive();

        /**
         * @brief Marks that the context is inactive and should be shut down.
         */
        void kill();
    };
}
