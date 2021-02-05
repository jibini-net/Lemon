#include "worker_thread.h"

#include <chrono>
#include <thread>

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
            throw std::runtime_error("Cannot park a worker thread on two separate threads");
        // Set the parked flag to true to avoid the above issue
        is_parked = true;

        // Loop until the park flag is disabled (likely infinite)
        while (is_parked)
        {
            // Claim the execution mutex for waiting
            std::unique_lock<std::mutex> execute_lock(this->execute_mutex);
            // Wait until the execution queue is not empty
            if (execution_queue.size() == 0)
                execute_condition.wait(execute_lock);

            // Claim the queue mutex for dequeue
            std::unique_lock<std::mutex> queue_lock(this->queue_mutex);

            // Iterate over the entire queue until empty
            while (this->execution_queue.size() > 0)
            {
                try
                {
                    // Execute each queued task, first-in first-out
                    this->execution_queue.front()();
                } catch(const std::exception& ex)
                {
                    auto error = ex.what();
                    this->log.error(std::string(error));
                }

                // Remove the executed task from the queue
                this->execution_queue.pop_front();
            }
        }
    }

    void worker_thread::execute(std::function<void()> task)
    {
        // Claim the queue mutex for enqueue
        std::unique_lock<std::mutex> queue_lock(this->queue_mutex);

        // Queue the provided task
        this->execution_queue.push_back(task);
        // Notify the execution thread that the queue has a task
        this->execute_condition.notify_all();
    }

    worker_pool::worker_pool(int num_workers)
    {
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
}