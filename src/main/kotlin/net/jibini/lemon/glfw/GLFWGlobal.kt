package net.jibini.lemon.glfw

import net.jibini.lemon.Lemon

import org.lwjgl.glfw.GLFW

import org.slf4j.LoggerFactory

import kotlin.concurrent.thread

/**
 * Initializes, tracks, and destroys the global GLFW library state. This is
 * required for one-time initialization of the GLFW native libraries and
 * termination upon shutdown.
 *
 * @author Zach Goethel
 */
object GLFWGlobal
{
    private val log = LoggerFactory.getLogger(javaClass)

    /**
     * GLFW will poll for events with a pause between of this time in millis.
     */
    private const val POLLING_INTERVAL = 2

    /**
     * Related windows whose contexts should be updated upon polling.
     */
    private val windows = mutableListOf<GLFWWindow>()

    /**
     * Registers each window for destruction when it is requested to close; this
     * is checked each time GLFW polls for input.
     *
     * @param window Related window to check for closing request.
     */
    fun link(window: GLFWWindow)
    {
        windows += window
    }

    /**
     * Count how many contexts are active; when set back to zero, GLFW will be
     * terminated. When incremented from zero to one, GLFW will be created.
     */
    var count = 0
        set(value)
        {
            val f = field
            field = value

            when (value)
            {
                0 ->
                {
                    if (f <= 0) return
                    // Terminate
                    log.debug("Terminating global GLFW library context")
                    Lemon.MAIN_THREAD.perform(GLFW::glfwTerminate, wait = false)
                }

                1 ->
                {
                    if (f > 0) return
                    // Initialize
                    log.debug("Creating global GLFW library context")
                    Lemon.MAIN_THREAD.perform(GLFW::glfwInit, wait = false)

                    // Invoke a poll every few milliseconds
                    thread(name = "Poll Invoke", isDaemon = true, start = true)
                    {
                        log.debug("Polling every {}ms on the main thread", POLLING_INTERVAL)

                        while (count > 0)
                        {
                            Lemon.MAIN_THREAD.perform(this::poll, wait = true)
                            Thread.sleep(POLLING_INTERVAL.toLong())
                        }
                    }
                }
            }
        }

    /**
     * Polling and window checking operations which require the main thread.
     */
    private fun poll()
    {
        GLFW.glfwPollEvents()

        for (window in windows)
        {
            if (window.pointer != 0L && GLFW.glfwWindowShouldClose(window.pointer))
                window.invokeOnCloseFunctions()
        }
    }
}