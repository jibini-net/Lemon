package net.jibini.lemon

/**
 * Any object whose actions can be stopped and resources can be deallocated.
 *
 * @author Zach Goethel
 */
@FunctionalInterface
interface Destroyable
{
    /**
     * Stops the object's actions and releases its resources.
     */
    fun destroy()
}