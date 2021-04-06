#pragma once

#include <functional>

#include "resource_stack.h"
#include "worker_thread.h"

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

namespace lemon
{
    /**
     * This class is implemented by Lemon extensions to represent each
     * extension's respective library contexts.  For example, the OpenGL
     * extension will implement this class to maintain an OpenGL context.  The
     * class constructor creates a unique thread which the context may use for
     * its synchronized operations (to keep all context-related actions within
     * the scope of a single thread).
     * 
     * By wrapping the context, an abstraction layer is provided to construct
     * and deconstruct context objects.  In this way, a graphical context can
     * be treated as any other object which can be scoped with smart-pointers or
     * constructed on demand.
     * 
     * Each context implementation should create a single context upon
     * construction and keep its context current on the created and provided
     * thread.  Contexts may keep global variables or states, such as GLFW's
     * library initialization which is only performed once for multiple
     * contexts.  Any number of each type of context may exist at any given
     * point.  The context may be created and destroyed as an automated function
     * of the Lemon engine.  If a function is not supported by a type of
     * context, the specific implementation is responsible for validation.
     * 
     * For debug purposes, it is recommended to inject debug callbacks or error-
     * checking at this stage in the context implementation.
     * 
     * @brief Represents an active graphical or application context.
     * @author Zach Goethel
     */
    class context
    {
        private:
            /**
             * This thread is created upon creation of a context, and allows for
             * the context to remain active/current in this thread for the
             * entire duration of its runtime.
             * 
             * Tasks specific to this context instance should be queued in this
             * worker thread, as it will allow multithreaded access to a single
             * context instance.
             * 
             * @brief Dedicated thread in which the context can stay active.
             */
            worker_thread thread;
        
        public:
            /**
             * Enqueues a context-related task to be performed on the context's
             * dedicated thread.  This method's implementation must be thread-
             * safe (such that multiple threads may enqueue tasks for execution
             * on one shared context).
             * 
             * This method will enqueue the provided task, but may also perform
             * validation and error-checking for the provided task.
             * 
             * Provided tasks should be considered atomic operations or "atomic"
             * groups of operations; only one operation should be executed at
             * any given time, and any operations which should be executed in a
             * specific order (or must be executed together) should be grouped
             * together into a single submitted task.
             * 
             * @brief Runs a task related to this context within this context.
             * @param task Context-related task to enqueue for execution from
             *      the context's dedicated thread.
             */
            void perform(std::function<void()> task);

            /**
             * @brief Updates the context, swaps the framebuffer, and polls input.
             */
            virtual void update()
            { }

            /**
             * @brief Checks whether this context is currently alive.
             * 
             * @return true If this context is still active and alive.
             * @return false If this context should be shut down.
             */
            virtual bool is_alive()
            { return true; }

            /**
             * @brief Marks that the context is inactive and should be shut down.
             */
            virtual void kill()
            { }
    };
}