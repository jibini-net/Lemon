package net.jibini.lemon

import net.jibini.lemon.opengl.OpenGL

import org.lwjgl.opengl.GL11

object TestApplication : Application
{
    override val context = OpenGL(4, 6)

    private val window = context.create<Window>()

    override fun start()
    {
        window.title = "Test Application"
        window.width = 800
        window.height = 600

        window.onClose {
            context.shouldClose = true
        }

        window.visible = true
    }

    override fun update()
    {
        context.perform({
            GL11.glClearColor(0.4f, 0.0f, 0.2f, 1.0f)
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