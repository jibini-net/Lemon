package net.jibini.lemon.context.impl

import net.jibini.lemon.Destroyable
import net.jibini.lemon.context.ContextExtension

/**
 * Any object which is related to a graphical context, has resources within that
 * graphical context, and can allocate and deallocate those resources.
 *
 * @author Zach Goethel
 */
abstract class ContextObject(val context: ContextExtension): Destroyable