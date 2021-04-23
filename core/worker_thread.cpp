#include "worker_thread.h"

#include <chrono>

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
    // Synchronization objects for awaiting task execution
    thread_local std::mutex await_mutex;
    // Conditional variable for awaiting task execution
    std::condition_variable await_condition;

    worker_thread::worker_thread(bool create_thread)
    {
        if (create_thread)
            // Create and park a new thread on which to execute tasks
            std::thread(&worker_thread::park, this).detach();
    }

    void worker_thread::park()
    {
        // Cannot run a worker thread on two separate threads; this may result
        // in unexpected dequeue behavior
        if (is_parked)
            throw std::runtime_error("Cannot park a single worker thread more than once concurrently");
        // Set the parked flag to true to avoid the above issue
        is_parked = true;
        thread_id = std::this_thread::get_id();

        // Loop until the park flag is disabled (likely infinite)
        while (is_parked)
        {
            semaphore.acquire();

            try
            {
                this->execution_queue.poll()();
            } catch(const std::exception& ex)
            {
                auto error = ex.what();
                log.error(error);
            }
        }
    }

    void worker_thread::execute(std::function<void()> task)
    {
        // Queue the provided task
        this->execution_queue.add(task);
        semaphore.release();
    }

    void worker_thread::execute_wait(std::function<void()> task)
    {
        // Protection for a thread queueing onto itself
        if (std::this_thread::get_id() == this->thread_id)
        {
            // Run task directly to avoid hardlocks
            try
            {
                task();
            } catch(const std::exception& ex)
            {
                auto error = ex.what();
                log.error(error);
            }
            
            return;
        }

        std::binary_semaphore l { 0 };

        // Wrap the task with synchronization operations
        execute([&]()
        {
            task();

            l.release();
        });

        l.acquire();
    }

    worker_pool::worker_pool(int num_workers)
    {
        log.debug("Starting up a thread pool with " + std::to_string(num_workers) + " members");

        // Store the number of worker threads
        this->num_workers = num_workers;
        // Allocate the array of threads
        this->workers = new worker_thread*[num_workers];

        // Initialize the pool of worker threads
        for (int i = 0; i < num_workers; i++)
            this->workers[i] = new worker_thread(true);
    }

    worker_pool::~worker_pool()
    {
        // Free each individual worker
        for (int i = 0; i < this->num_workers; i++)
            delete this->workers[i];
        // Free the worker array memory
        delete[] workers;
    }

    //TODO BETTER LOAD BALANCING
    void worker_pool::execute(std::function<void()> task)
    {
        // Select the next round-robin thread
        auto worker_index = this->round_robin.fetch_add(1) % this->num_workers;
        auto worker = this->workers[worker_index];
        // Enqueue the provied task in that thread
        worker->execute(task);
    }
}