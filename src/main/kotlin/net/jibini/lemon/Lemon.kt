package net.jibini.lemon

import net.jibini.lemon.concurrency.Worker

import org.slf4j.LoggerFactory

import kotlin.concurrent.thread
import kotlin.system.exitProcess

/**
 * Collection of static states and bootstrap operations. This object will start
 * up the application, creating basic resources such as a graphical context and
 * worker threads for completing application tasks.
 *
 * This engine can start up multiple application instances with several backend
 * implementations, but [start] must only be called once; it will indefinitely
 * consume the thread by which it is called in order to allow posting of tasks
 * for execution on the that thread (e.g., posting window operations to the main
 * thread for GLFW operations).
 *
 * @author Zach Goethel
 */
object Lemon
{
    private val log = LoggerFactory.getLogger(javaClass)

    /**
     * This application's plain-English name.
     */
    private const val APP_NAME = "Lemon"

    /**
     * This application's version string.
     */
    private const val APP_VERSION = "1.0-kt"

    init
    {
        Thread.currentThread().name = "Bootstrap"
        log.info("Welcome to {} ({}) written by Zach Goethel", APP_NAME, APP_VERSION)
    }

    /**
     * This worker thread runs on the application's primary thread; this should
     * be the main thread, but can also be whichever thread called [start].
     *
     * Tasks posted to this worker will be executed on the main thread. This is
     * required for certain libraries such as GLFW, which requires many of its
     * window operations to be executed on the primary thread. Window and OpenGL
     * initialization, polling, and context destruction will likely be performed
     * by this worker thread.
     */
    @JvmStatic
    val MAIN_THREAD = Worker.create(start = false)

    /**
     * A worker pool which allows for execution of arbitrary auxiliary tasks.
     * This pool will be initialized to have the same number of workers as the
     * runtime has logical cores.
     */
    @JvmStatic
    val PRIMARY_POOL = Worker.createPool()

    fun start(vararg applications: Application)
    {
        MAIN_THREAD.perform({
            log.debug("Main thread has been consumed and will hang indefinitely")
        }, false)

        Thread.currentThread().name = "Main Worker"
        val appThreads = mutableListOf<Thread>()

        for (app in applications)
            appThreads += thread(name = "Application", start = true)
            {
                app.start()
                while (app.context.alive)
                    app.update()

                app.destroy()
                app.context.destroy()
            }

        thread(name = "Shutdown", start = true)
        {
            for (thread in appThreads)
                thread.join()

            log.info("All applications have exited")
            MAIN_THREAD.perform({ exitProcess(0) }, wait = true)
        }

        MAIN_THREAD.useThisThread()
    }
}