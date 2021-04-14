#pragma once

#include <memory>

#include "context.h"
#include "worker_thread.h"
#include "logger.h"

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
     * A single scoped instance of an application which has its own unique
     * graphical context, set of resources, and runtime processes.  As a rule of
     * thumb, each application will have a single window, dedicated choreography
     * and graphical thread, and application-specific and context-linked set of
     * assets and resources.
     * 
     * Each application should be should have states and runtimes which are
     * mutually exclusive from all other applications.  The termination of any
     * given application should allow other started applications to continue.
     * Unless by design, an unrecoverable error in one application should only
     * lead to the loss of the one application.  Behavior for attempting to use
     * another application's asset is undefined, but is not expected to be
     * successful or graceful (it will likely result in unexpected behavior or
     * an unpredictable crash).
     * 
     * Provided contexts will be destroyed upon termination of the application;
     * to avoid double-deletion, the provided contexts should be statically
     * defined or dynamically allocated.
     * 
     * @brief A single discrete application with a graphical context.
     * @author Zach Goethel
     */
    class application
    {
        protected:
            /**
             * This graphical context provides asset instances and performs
             * graphical operations for the application.
             *  
             * Provided contexts will be destroyed upon termination of the
             * application; to avoid double-deletion, the provided contexts
             * should be statically defined or dynamically allocated.
             * 
             * @brief This application's unique graphical context of any type.
             */
            std::shared_ptr<context> app_context;

            /**
             * Choreography tasks (frame updates, allocation, invocation of
             * queueing of graphical operations) are performed on this thread.
             * This thread does not contain a graphical context, thus it does
             * not perform any direct graphical calls.
             * 
             * @brief A dedicated thread for application frame rendering.
             */
            worker_thread app_thread;
        
            /**
             * This function will hang until the application dies.  It is the
             * primary application loop which invokes rendering, updates, and
             * graphical context updates (buffer swap, buffer clearing, etc.).
             * 
             * @brief Started on the application's dedicated thread to boot.
             * @param loop A function which is invoked each frame to update the
             *      application, provided frame-times.
             */
            void start(std::function<void(double)> loop);

            /**
             * @brief Application-management-specific logger instance.
             */
            logger log { "Application" };

        public:
            application(std::shared_ptr<context> app_context, std::function<void(double)> loop);
    };
}
