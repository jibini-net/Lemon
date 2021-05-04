package net.jibini.lemon.glfw

import net.jibini.lemon.Lemon
import net.jibini.lemon.Window
import net.jibini.lemon.context.ContextExtension

import org.lwjgl.glfw.GLFW

/**
 * Implementation of the [Window] type for any context which uses GLFW. Only one
 * window may exist at a time for a GLFW-based context, but this is subject to
 * change with later implementations.
 *
 * @author Zach Goethel
 */
class GLFWWindow(context: ContextExtension) : Window(context)
{
    /**
     * This context should only ever be a GLFW context.
     */
    private val c = context as GLFWContext

    init
    {
        // Increment the number of windows
        c.windowCount++
    }

    override var title: String = "Window"
        set(value)
        {
            Lemon.MAIN_THREAD.perform({
                GLFW.glfwSetWindowTitle(c.pointer, value)
            }, wait = false)

            field = value
        }

    override var visible: Boolean
        get() = TODO("Not yet implemented")
        set(value)
        {
            TODO("Not yet implemented")
        }

    override var fullscreen: Boolean
        get() = TODO("Not yet implemented")
        set(value)
        {
            TODO("Not yet implemented")
        }

    override var width: Int
        get() = TODO("Not yet implemented")
        set(value)
        {
            TODO("Not yet implemented")
        }

    override var height: Int
        get() = TODO("Not yet implemented")
        set(value)
        {
            TODO("Not yet implemented")
        }

    override fun destroy()
    {
        Lemon.MAIN_THREAD.perform({
            GLFW.glfwDestroyWindow(c.pointer)
        }, wait = true)

        // Decrement the number of windows
        c.windowCount--
    }
}