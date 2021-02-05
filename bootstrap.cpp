#include <iostream>
#include <thread>

#include "core/worker_thread.h"

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

/**
 * This function will be the first task posted to the main thread for execution.
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
    //TODO REMOVE
    std::cout << std::this_thread::get_id() << std::endl;
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
    lemon::worker_thread main_thread(false);

    //TODO SPAWN ARBITRARY WORKER THREAD POOL
    int num_threads = std::thread::hardware_concurrency();
    //TODO REMOVE
    for (int i = 0; i < num_threads; i++)
    {
        auto worker = new lemon::worker_thread(true);

        worker->execute(start);
        worker->execute([&] { std::cout << "Hello, world!" << std::endl; });
    }

    main_thread.execute(start);
    main_thread.park();

    return 0;
}