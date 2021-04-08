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
     * @brief A single discrete application with a graphical context.
     */
    class application
    {
        protected:
            /**
             * @brief This application's unique graphical context of any type.
             */
            context* app_context;

            /**
             * @brief A dedicated thread for application frame rendering.
             */
            worker_thread app_thread;
        
            /**
             * @brief Started on the application's dedicated thread to boot.
             */
            void start();

            /**
             * @brief Application-management-specific logger instance.
             */
            logger log { "Application" };

            /**
             * @brief This application's resource stack for application assets.
             */
            resource_stack resources;

            /**
             * @brief A resource hold on the stack's base (e.g., context).
             */
            resource_hold* base_hold = new resource_hold { this->resources };

            /**
             * @brief A function which is invoked for every frame.
             */
            std::function<void()> loop;

        public:
            template <class T>
            application(T* app_context, std::function<void()> loop)
            {
                // Maintain a reference to the context
                this->app_context = app_context;
                // Register the context for deletion
                this->resources.attach(app_context);

                this->loop = loop;

                // Start the application on the dedicated thread
                log.debug("Starting application on dedicated thread");
                this->app_thread.execute([&]()
                {
                    log.debug("Active on dedicated thread; looping until context dies");
                    this->start();
                });
            }
    };
}
