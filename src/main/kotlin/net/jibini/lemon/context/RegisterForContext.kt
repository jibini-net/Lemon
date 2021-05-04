package net.jibini.lemon.context

import kotlin.reflect.KClass

/**
 * Marks a class as a context-specific implementation of one of the Lemon
 * object interfaces. This annotation enables classpath scanning of the context
 * extensions to find implementations of those interfaces and create them when
 * requested.
 *
 * @property contextType Class of the relevant context for this implementation.
 * @property objectType Class of the Lemon interface which the class implements.
 * @author Zach Goethel
 */
@Retention(AnnotationRetention.RUNTIME)
@Target(AnnotationTarget.CLASS)
annotation class RegisterForContext(val contextType: KClass<out ContextExtension>, val objectType: KClass<out Any>)
