package net.jibini.lemon

import net.jibini.lemon.context.ContextExtension
import net.jibini.lemon.context.impl.ContextObject

import java.util.concurrent.CopyOnWriteArrayList

/**
 * An application window. This is the interface with the user which is seen in
 * their OS.
 *
 * @author Zach Goethel
 */
abstract class Window(context: ContextExtension) : ContextObject(context)
{
    /**
     * Title of this window. This is displayed on the top bar of the window and
     * in the taskbar of the OS.
     */
    abstract var title: String

    /**
     * This flag sets whether the window is visible or hidden from the user.
     */
    abstract var visible: Boolean

    /**
     * This flag sets whether the window is in fullscreen mode or in regular
     * windowed mode.
     */
    abstract var fullscreen: Boolean

    /**
     * Width of the window or window framebuffer in pixels.
     */
    abstract var width: Int

    /**
     * Height of the window or window framebuffer in pixels.
     */
    abstract var height: Int

    /**
     * Whether a buffer swap should wait for a monitor vertical sync, thus
     * reducing tearing in the output image.
     */
    abstract var verticalSync: Boolean

    /**
     * Swaps the window's buffers to display the next rendered frame.
     */
    abstract fun swapBuffers()

    /**
     * Thread-safe collection of tasks to run when the window should be closed.
     */
    private val onCloseFunctions = CopyOnWriteArrayList<() -> Unit>()

    /**
     * The provided action will be invoked when the window is requested to be
     * closed or is closed.
     *
     * @param action Action function to run when the window closes.
     */
    fun onClose(action: () -> Unit)
    {
        onCloseFunctions += action
    }

    /**
     * Runs all of the registered [onClose] functions in the order they were
     * added to the collection.
     */
    fun invokeOnCloseFunctions()
    {
        for (function in onCloseFunctions)
            function()
    }
}