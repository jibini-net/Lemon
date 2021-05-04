package net.jibini.lemon

import net.jibini.lemon.context.ContextExtension
import net.jibini.lemon.context.impl.ContextObject

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
}