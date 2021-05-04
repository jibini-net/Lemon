package net.jibini.lemon.opengl

import net.jibini.lemon.Window
import net.jibini.lemon.context.impl.ContextObjectDiscoveryImpl
import net.jibini.lemon.glfw.GLFWContext
import net.jibini.lemon.glfw.GLFWGlobal
import net.jibini.lemon.glfw.GLFWWindow

import org.lwjgl.glfw.GLFW

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
class OpenGL(major: Int, minor: Int) : GLFWContext()
{
    override val pointer: Long

    init
    {
        GLFWGlobal.count++

        ContextObjectDiscoveryImpl.registerImplementation(this::class, Window::class, GLFWWindow::class.java)

        //TODO STORE FLAGS IN GLFWContext or Window FOR WINDOW RE-CREATION
        GLFW.glfwDefaultWindowHints()
        GLFW.glfwWindowHint(GLFW.GLFW_VERSION_MAJOR, major);
        GLFW.glfwWindowHint(GLFW.GLFW_VERSION_MINOR, major);
        GLFW.glfwWindowHint(GLFW.GLFW_VISIBLE, GLFW.GLFW_FALSE);

        if ((major > 3) || (major == 3 && minor >= 3))
        {
            GLFW.glfwWindowHint(GLFW.GLFW_OPENGL_PROFILE, GLFW.GLFW_OPENGL_CORE_PROFILE)
            GLFW.glfwWindowHint(GLFW.GLFW_OPENGL_FORWARD_COMPAT, GLFW.GLFW_TRUE)
        }

        //TODO MOVE INTO WINDOW
        pointer = GLFW.glfwCreateWindow(800, 600, "Window", 0L, 0L)
    }

    override fun destroy()
    {
        GLFWGlobal.count--
    }

    override val alive: Boolean
        get() = !shouldClose
}