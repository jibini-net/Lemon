package net.jibini.lemon.context

import kotlin.reflect.KClass

@Retention(AnnotationRetention.RUNTIME)
@Target(AnnotationTarget.CLASS)
annotation class ContextObject(val contextType: KClass<out ContextExtension>, val objectType: KClass<out Any>)
