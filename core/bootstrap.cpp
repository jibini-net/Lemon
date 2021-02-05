#include "bootstrap.h"

#include <iostream>
#include <thread>

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
     * @brief The bootstrapper-specific logger instance for bootstrap log messages.
     */
    logger _log("(Bootstrap)");

    worker_thread main_thread(false);
    worker_pool main_pool;

    resource_stack main_resource_stack;

    /**
     * This function will be the first task posted to the main thread for execution.
     * It may post future tasks, but may not hang infinitely.
     * 
     * Its primary purpose is to initialize the rendering engine and any required
     * contexts for applications maintained by this runtime instance. This may also
     * include creating additional threads and contexts, loading configuration, and
     * allocating resources as necessary to start applications.
     * 
     * @brief Runtime instance initialization and bootstrapping method.
     */
    void start()
    {
        
    }
}

/**
 * This application entrypoint parks the main thread for context- and windowing-
 * related tasks. The main thread will be parked by a worker thread which will
 * accept any operations which must be run on the main thread for thread-safety
 * reasons. This may include GLFW polling and other context-related operations.
 * 
 * It also spawns a collection of worker threads for arbitrary tasks. The number
 * of threads created is subject to change. These threads are global and may be
 * used by multiple contexts; they are not specific to any single context.
 * 
 * @brief Spawns the threads required for the runtime prior to initialization.
 */
int main()
{
	lemon::_log.info("\033[1;33m===============================================================");
	lemon::_log.info("                 \033[1;31mLemon\033[0m created by \033[1;31mZach Goethel");
	lemon::_log.info("\033[1;33m===============================================================");

    lemon::main_thread.execute(lemon::start);
    lemon::main_thread.park();

    return 0;
}