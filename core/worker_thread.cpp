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
    // Counter to avoid hardlock on a given thread
    thread_local int mut_count = 0;

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
            // Claim the execution mutex for waiting
            std::unique_lock<std::mutex> execute_lock(this->execute_mutex);
            
            // Wait until the execution queue is not empty
            while (execution_queue.size() == 0)
                execute_condition.wait_for(execute_lock, std::chrono::milliseconds(100));

            // Claim the queue mutex for dequeue
            if (mut_count++ == 0)
                this->queue_mutex.lock();

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
                    log.error(error);

                    throw std::runtime_error("A fatal error has occurred");
                }

                // Remove the executed task from the queue
                this->execution_queue.pop_front();
            }

            // Unlock when this thread gives up mutex
            if (--mut_count == 0)
                this->queue_mutex.unlock();
        }
    }

    void worker_thread::execute(std::function<void()> task)
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

        // Claim the queue mutex for enqueue
        if (mut_count++ == 0)
            this->queue_mutex.lock();

        // Queue the provided task
        this->execution_queue.push_back(task);
        // Notify the execution thread that the queue has a task
        this->execute_condition.notify_all();

        // Unlock when this thread gives up mutex
        if (--mut_count == 0)
            this->queue_mutex.unlock();
    }

    void worker_thread::execute_wait(std::function<void()> task)
    {
        // Latched state to avoid hardlock
        std::atomic_bool complete(false);

        // Wrap the task with synchronization operations
        execute([=, &complete]()
        {
            task();
            
            // Notify waiting thread that task is complete
            complete = true;
            await_condition.notify_all();
        });

        // Wait until notification that task is complete
        std::unique_lock<std::mutex> lock(await_mutex);
        while (!complete.load())
            await_condition.wait_for(lock, std::chrono::milliseconds(1));
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