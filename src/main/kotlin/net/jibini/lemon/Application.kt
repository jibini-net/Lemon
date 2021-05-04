package net.jibini.lemon

import net.jibini.lemon.context.ContextExtension

/**
 * Any implementation of an application which uses the Lemon framework must
 * implement this interface. The instance can than be passed to [Lemon.start] to
 * initialize and start the application.
 *
 * Route creation of graphical objects through the [context] object. It will
 * provide access to the correct context-specific object factory.
 *
 * @author Zach Goethel
 */
interface Application : Destroyable
{
    /**
     * An instance of the context this application will use. This will decide
     * which context extension is used throughout runtime.
     */
    val context: ContextExtension

    /**
     * Initialization method for the application. Create and set up objects.
     */
    fun start()

    /**
     * Update method for the application. Update calculations and app data.
     */
    fun update()
}