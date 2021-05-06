package net.jibini.lemon.test

import net.jibini.lemon.Application
import net.jibini.lemon.Lemon
import net.jibini.lemon.Window
import net.jibini.lemon.opengl.OpenGL

import org.lwjgl.glfw.GLFW
import org.lwjgl.opengl.GL11

import org.slf4j.LoggerFactory

import kotlin.math.sin

object TestApplication : Application
{
    private val log = LoggerFactory.getLogger(javaClass)

    // Use OpenGL context version 4.6 for testing
    override val context = OpenGL(4, 6)

    // This context's window; implemented via GLFW extension
    private val window = context.create<Window>()

    override fun start()
    {
        window.title = "Test Application"
        window.width = 800
        window.height = 600
        window.verticalSync = true

        window.onClose {
            context.shouldClose = true
        }

        window.visible = true
    }

    override fun update()
    {
        context.perform({
            val time = GLFW.glfwGetTime().toFloat()
            val r = sin(time)
            val g = sin(time + 2 * 3.14f / 3)
            val b = sin(time + 4 * 3.14f / 3)

            GL11.glClearColor(r, g, b, 1.0f)
            GL11.glClear(GL11.GL_COLOR_BUFFER_BIT)
        })

        window.swapBuffers()
    }

    override fun destroy()
    {
        window.destroy()
    }
}

fun main()
{
    Lemon.start(TestApplication)
}