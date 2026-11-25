#pragma once

#include "worker_thread.h"

namespace lemon
{
    /**
     * @brief Worker thread for queuing and executing tasks on the main thread.
     */
    extern worker_thread main_thread;

    /**
     * @brief Global worker pool for multithreading across all system threads.
     */
    extern worker_pool primary_pool;
}