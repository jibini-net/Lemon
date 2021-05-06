package net.jibini.lemon.context

import net.jibini.lemon.Destroyable
import net.jibini.lemon.context.impl.ContextObject
import net.jibini.lemon.context.impl.ContextObjectDiscoveryImpl

/**
 * Any instance of a graphical context as defined by a context extension which
 * is on the classpath; this can include GLFW-based contexts, such as OpenGL, or
 * Vulkan. Graphical context extensions provide backend implementations for the
 * interfaces set by Lemon.
 *
 * Mark implementing classes with [RegisterForContext], passing class of the
 * relevant context type and the [ContextObject] it implements.
 *
 * @author Zach Goethel
 */
abstract class ContextExtension : Destroyable
{
    /**
     * Searches for the proper implementation of the requested object type for
     * this context. An instance is created, initialized, and returned.
     *
     * @param T Type of the object to create. This should be the class of the
     *      Lemon interface which represents this type of object, not an
     *      implementing class. It should also be the same class as what is
     *      specified in the implementation's [RegisterForContext] annotation.
     * @return An instance of the requested type with the proper backend
     *      implementation for this context extension.
     */
    inline fun <reified T : Any> create(): T = ContextObjectDiscoveryImpl.createInstance(this)

    /**
     * Whether this application is still running and usable. When false, the
     * behavior of the context may be undefined. If unsure if a context is still
     * in "working" condition, check this flag before attempting to use it.
     */
    abstract val alive: Boolean

    /**
     * Executes the provided task within the graphical context. It will be
     * posted for execution in a thread with the correct states for executing
     * this context's operations.
     *
     * @param task Task to execute within this context.
     * @param wait Whether this call should hang until the task has been
     *      executed within the target context.
     */
    abstract fun perform(task: () -> Unit, wait: Boolean = false)
}