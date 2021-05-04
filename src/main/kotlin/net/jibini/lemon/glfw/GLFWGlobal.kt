package net.jibini.lemon.glfw

import org.lwjgl.glfw.GLFW

import org.slf4j.LoggerFactory

import java.lang.RuntimeException

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
     * Count how many contexts are active; when set back to zero, GLFW will be
     * terminated. When incremented from zero to one, GLFW will be created.
     */
    var count = 0
        set(value)
        {
            if (value == 0)
            {
                // Terminate
                log.debug("Terminating global GLFW library context")
                GLFW.glfwTerminate()
            } else if (field == 0)
            {
                // Initialize
                log.debug("Creating global GLFW library context")
                if (!GLFW.glfwInit())
                    throw RuntimeException("Failed to initialize global GLFW context")
            }

            field = value
        }
}