package net.jibini.lemon.glfw

import net.jibini.lemon.Lemon
import net.jibini.lemon.Window
import net.jibini.lemon.context.ContextExtension
import net.jibini.lemon.context.RegisterForContext

import org.lwjgl.glfw.GLFW

/**
 * Implementation of the [Window] type for any context which uses GLFW. Only one
 * window may exist at a time for a GLFW-based context, but this is subject to
 * change with later implementations.
 *
 * @author Zach Goethel
 */
@RegisterForContext(GLFWContext::class, Window::class)
class GLFWWindow(context: ContextExtension) : Window(context)
{
    /**
     * This context should only ever be a GLFW context.
     */
    private val c = context as GLFWContext

    /**
     * The active window pointer of this GLFW window.
     */
    var pointer = 0L

    init
    {
        // Increment the number of windows
        c.windowCount++
        GLFWGlobal.link(this)

        recreate()
    }

    private fun recreate()
    {
        Lemon.MAIN_THREAD.perform({
            c.windowHints[GLFW.GLFW_VISIBLE] = if (visible) GLFW.GLFW_TRUE else GLFW.GLFW_FALSE
            // Restore GLFW window hint states with cache
            GLFW.glfwDefaultWindowHints()
            for ((k, v) in c.windowHints)
                GLFW.glfwWindowHint(k, v)
            // Retrieve monitor and create window
            val monitor = if (fullscreen) GLFW.glfwGetPrimaryMonitor() else 0L
            val window = GLFW.glfwCreateWindow(width, height, title, monitor, c.pointer)

            // Destroy old window and update pointer
            if (pointer != 0L)
                GLFW.glfwDestroyWindow(pointer)
            pointer = window

            //TODO MAKE CURRENT IN CONTEXT THREAD
        }, wait = false)
    }

    override var title = "Window"
        set(value)
        {
            field = value

            Lemon.MAIN_THREAD.perform({
                GLFW.glfwSetWindowTitle(pointer, value)
            }, wait = false)
        }

    override var visible = false
        set(value)
        {
            field = value
            recreate()
        }

    override var fullscreen = false
        set(value)
        {
            field = value
            recreate()
        }

    override var width = 800
        set(value)
        {
            field = value

            Lemon.MAIN_THREAD.perform({
                GLFW.glfwSetWindowSize(pointer, value, height)
            }, wait = false)
        }

    override var height = 400
        set(value)
        {
            field = value

            Lemon.MAIN_THREAD.perform({
                GLFW.glfwSetWindowSize(pointer, width, value)
            }, wait = false)
        }

    override fun destroy()
    {
        Lemon.MAIN_THREAD.perform({
            GLFW.glfwDestroyWindow(pointer)
            pointer = 0L

            // Decrement the number of windows
            c.windowCount--
        }, wait = false)
    }

    override fun swapBuffers()
    {
        Lemon.MAIN_THREAD.perform({
            if (pointer != 0L)
                GLFW.glfwSwapBuffers(pointer)
        }, wait = true)
    }
}