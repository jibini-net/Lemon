package net.jibini.lemon

import net.jibini.lemon.opengl.OpenGL

object TestApplication : Application
{
    override val context = OpenGL(4, 6)

    private val window = context.create<Window>()

    override fun start()
    {
        window.visible = true
        window.title = "Test Application"
        window.width = 800
        window.height = 600

        window.onClose {
            context.shouldClose = true
        }
    }

    override fun update()
    {
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