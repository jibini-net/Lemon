#include "bootstrap.h"

#include <iostream>
#include <thread>
#include <math.h>

#include "application.h"
#include "logger.h"
#include "worker_thread.h"

#include "ext_opengl/ext_opengl.h"
#include "ext_opengl/gl_ssbo.h"
#include "ext_glfw/ext_glfw.h"

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
    worker_pool primary_pool;

    struct test_t
    {
        int val;
    };

    /**
     * This function will be the first task posted to the main thread for execution.
     * It may post future tasks, but must not hang infinitely.
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
        static auto gl = new gl_context(4, 6, true, true);
        static auto buffer = new gl_ssbo(gl, 0);

        buffer->put(new test_t, sizeof(test_t));

        auto app = new application(gl, [&]()
        {
            buffer->map_scoped<test_t>(true, false, [&](auto mapped)
            {
                
            });

            int i = 0, j;
            for (i; i < 200000; i++)
                j = (int)sqrt(5.0f);
        });
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
 * @brief Bootstraps the runtime in preparation for application operations.
 */
int main(void)
{
    lemon::_log.info("\033[1;33m===============================================================");
    lemon::_log.info("                 \033[1;31mLemon\033[0m created by \033[1;31mZach Goethel");
    lemon::_log.info("\033[1;33m===============================================================");

    lemon::main_thread.execute(lemon::start);
    lemon::main_thread.park();

    return 0;
}