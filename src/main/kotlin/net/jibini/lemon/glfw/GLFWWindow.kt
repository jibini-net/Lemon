package net.jibini.lemon.glfw

import kotlinx.coroutines.runBlocking

import net.jibini.lemon.Lemon
import net.jibini.lemon.Window
import net.jibini.lemon.context.ContextExtension
import net.jibini.lemon.context.RegisterForContext

import org.lwjgl.glfw.GLFW
import org.lwjgl.opengl.GL

/**
 * Implementation of the [Window] type for any context which uses GLFW. Only one
 * window may exist at a time for a GLFW-based context, but this is subject to
 * change with later implementations.
 *
 * By default, the window points to the GLFW-based context's primary window,
 * which may be invisible. Upon changing a window hint which requires the window
 * to be re-created, such as changing the visibility or fullscreen mode, a
 * secondary window will be created with a resource share with the original
 * context window.
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

    private var _pointer = 0L
    /**
     * The active window pointer of this GLFW window.
     */
    val pointer: Long
        get() = if (_pointer == 0L) c.pointer else _pointer

    init
    {
        // Increment the number of windows
        c.windowCount++
        GLFWGlobal.link(this)
    }

    private fun recreate()
    {
        Lemon.MAIN_THREAD.perform({
            c.windowHints[GLFW.GLFW_VISIBLE] = if (visible) GLFW.GLFW_TRUE else GLFW.GLFW_FALSE
            c.windowHints[GLFW.GLFW_RESIZABLE] = GLFW.GLFW_FALSE
            // Restore GLFW window hint states with cache
            GLFW.glfwDefaultWindowHints()
            for ((k, v) in c.windowHints)
                GLFW.glfwWindowHint(k, v)
            // Retrieve monitor and create window
            val monitor = if (fullscreen) GLFW.glfwGetPrimaryMonitor() else 0L
            val old = _pointer
            _pointer = GLFW.glfwCreateWindow(width, height, title, monitor, c.pointer)
            // Destroy old window and update pointer
            if (old != 0L)
                runBlocking {
                    GLFW.glfwDestroyWindow(old)
                    c.contextActive.lock()
                }

            c.worker.perform({
                Thread.currentThread().name = "Context"

                if (_pointer != 0L)
                {
                    GLFW.glfwMakeContextCurrent(pointer)
                    GL.createCapabilities()

                    c.contextActive.unlock()
                }
            }, wait = true)
        }, wait = true)
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

    override var verticalSync = false

    override fun destroy()
    {
        Lemon.MAIN_THREAD.perform({
            if (_pointer != 0L)
                GLFW.glfwDestroyWindow(_pointer)
            _pointer = 0L

            // Decrement the number of windows
            c.windowCount--
        }, wait = false)
    }

    override fun swapBuffers()
    {
        c.perform({
            GLFW.glfwSwapInterval(if (verticalSync) GLFW.GLFW_TRUE else GLFW.GLFW_FALSE)
            GLFW.glfwSwapBuffers(pointer)
        }, wait = true)
    }
}