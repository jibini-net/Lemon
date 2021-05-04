package net.jibini.lemon

import net.jibini.lemon.opengl.OpenGL

fun main()
{
    Lemon.start(TestApplication)
}

object TestApplication : Application
{
    override val context = OpenGL(4, 6)

    override fun start()
    {

    }

    override fun update()
    {
        //TODO
        context.shouldClose = true
    }

    override fun destroy()
    {

    }
}