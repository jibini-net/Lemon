package net.jibini.lemon.context.impl

import io.github.classgraph.ClassGraph

import net.jibini.lemon.context.ContextExtension
import net.jibini.lemon.context.ContextObject

import org.slf4j.LoggerFactory

import java.util.concurrent.ConcurrentHashMap

import kotlin.reflect.KClass
import kotlin.reflect.jvm.jvmName

/**
 * Performs classpath scanning and discovers implementations of graphical
 * objects specific to a particular context type. For instance, this will find
 * the proper implementation for an OpenGL framebuffer when the OpenGL context
 * extension is active.
 *
 * The scan is performed once when this class is loaded. Implementation classes
 * are cached for later use during runtime.
 *
 * @author Zach Goethel
 */
object ContextObjectDiscoveryImpl
{
    private val log = LoggerFactory.getLogger(javaClass)

    /**
     * A convenience class which is a collection of implementations for a
     * specific context type.
     */
    private class ContextObjects : MutableMap<KClass<out Any>, Class<out Any>> by ConcurrentHashMap()

    /**
     * Map of context types to all implementation classes for those contexts.
     */
    private val discoveredObjects = ConcurrentHashMap<KClass<out ContextExtension>, ContextObjects>()

    init
    {
        // Perform classpath search for annotated classes
        val annotationName = ContextObject::class.jvmName
        val annotated = ClassGraph()
            .enableClassInfo()
            .enableAnnotationInfo()
            .scan(Runtime.getRuntime().availableProcessors())
            .getClassesWithAnnotation(annotationName)

        for (o in annotated)
        {
            // Load class and annotation
            val c = o.loadClass()
            val a = c.getAnnotationsByType(ContextObject::class.java)[0]

            log.debug("Found context implementation '{}' for object type '{}' in '{}'",
                c.simpleName,
                a.objectType.simpleName,
                a.contextType.simpleName)

            // Place implementation in context's collection
            registerImplementation(a.contextType, a.objectType, c)
        }
    }

    /**
     * Manually registers the provided implementation for the specified object
     * type when the specified context type is active.
     *
     * @param context Context extension for which this implementation applies.
     * @param object Object type which this registered class implements.
     * @param impl Implementation type for the provided object and context type.
     */
    fun registerImplementation(context: KClass<out ContextExtension>, `object`: KClass<out Any>, impl: Class<out Any>)
    {
        // Place implementation in context's collection
        val objects = discoveredObjects.getOrPut(context) { ContextObjects() }
        objects[`object`] = impl
    }
}