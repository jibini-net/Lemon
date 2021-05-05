package net.jibini.lemon.opengl

import net.jibini.lemon.Lemon
import net.jibini.lemon.Window
import net.jibini.lemon.context.impl.ContextObjectDiscoveryImpl
import net.jibini.lemon.glfw.GLFWContext
import net.jibini.lemon.glfw.GLFWWindow

import org.lwjgl.glfw.GLFW
import org.lwjgl.opengl.GL

/**
 * An OpenGL context with a specified context version. This context extension
 * provides OpenGL implementations for all basic Lemon features. It relies on
 * GLFW for windowing and context management.
 *
 * Context versions greater than or equal to 3.3 will be marked as core and
 * forward compatible. This may disable legacy features of OpenGL.
 *
 * @author Zach Goethel
 */
class OpenGL(private val major: Int, private val minor: Int) : GLFWContext()
{
    override val pointer: Long
        get() = invisibleWindow

    /**
     * Context's main window which is invisible but persists any changes to the
     * visible UI window.
     */
    private var invisibleWindow = 0L

    init
    {
        ContextObjectDiscoveryImpl.registerImplementation(this::class, Window::class, GLFWWindow::class.java)

        Lemon.MAIN_THREAD.perform(this::contextInit, wait = false)
    }

    /**
     * Initialization functionality which must be executed on the main thread.
     */
    private fun contextInit()
    {
        windowHints[GLFW.GLFW_CONTEXT_VERSION_MAJOR] = major
        windowHints[GLFW.GLFW_CONTEXT_VERSION_MINOR] = minor
        windowHints[GLFW.GLFW_VISIBLE] = GLFW.GLFW_FALSE

        if ((major > 3) || (major == 3 && minor >= 3))
        {
            windowHints[GLFW.GLFW_OPENGL_PROFILE] = GLFW.GLFW_OPENGL_CORE_PROFILE
            windowHints[GLFW.GLFW_OPENGL_FORWARD_COMPAT] = GLFW.GLFW_TRUE
        }

        GLFW.glfwDefaultWindowHints()
        for ((k, v) in windowHints)
            GLFW.glfwWindowHint(k, v)

        // Create context with hidden window
        invisibleWindow = GLFW.glfwCreateWindow(8, 8, "Context", 0L, 0L)
        // Create actual backend context
        GLFW.glfwMakeContextCurrent(pointer)
        GL.createCapabilities()
        // Relinquish on main thread (required)
        GLFW.glfwMakeContextCurrent(0L)
    }

    override fun destroy()
    {
        Lemon.MAIN_THREAD.perform({
            GLFW.glfwDestroyWindow(invisibleWindow)
        }, wait = false)

        super.destroy()
    }

    override val alive: Boolean
        get() { print(""); return !shouldClose }
}