package net.jibini.lemon.glfw

import net.jibini.lemon.context.ContextExtension

import java.lang.UnsupportedOperationException

/**
 * This base class implements features common to all GLFW-based contexts, which
 * includes OpenGL, OpenGL ES, Vulkan, and OpenGL for compute. GLFW-based
 * contexts may only have one window at a time, but this is subject to change
 * with later implementation.
 *
 * @author Zach Goethel
 */
abstract class GLFWContext : ContextExtension()
{
    /**
     * Stored window flags for recreating and modifying windows mid-runtime.
     */
    val windowHints = mutableMapOf<Int, Int>()

    /**
     * Tracks how many GLFW windows are active on this GLFW context.
     */
    var windowCount = 0
        set(value)
        {
            field = value

            // Currently, GLFW-based contexts only support one window
            if (value > 1)
                throw UnsupportedOperationException("GLFW contexts may only have a single window")
            if (value == 0)
                shouldClose = true
        }

    /**
     * Window handle for the primary GLFW window of this context.
     */
    abstract val pointer: Long

    /**
     * Whether the window has been requested to close or the application has
     * requested it be closed.
     */
    var shouldClose = false

    init
    {
        GLFWGlobal.count++
    }

    override fun destroy()
    {
        GLFWGlobal.count--
    }
}