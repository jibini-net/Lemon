#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

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
     * Infinitely sits on a thread and executes a queue of tasks on that thread.
     * This allows multiple threads to queue tasks for execution on a collection
     * of worker threads, or for secondary threads to post tasks for execution
     * on the main thread (or a thread containing a graphical context).
     * 
     * Queued execution on the main thread is useful for GLFW windowing
     * operations, as many GLFW operations may only be called from the main
     * application thread.  Execution may also be queued in a thread which
     * maintains an OpenGL or OpenAL context.  Any thread can then enqueue
     * render and audio calls.
     * 
     * A worker thread can either be parked on an existing thread, or passed a
     * flag to start a new thread.  If the thread start flag is set, a new
     * thread will be spawned upon construction of a new worker thread.
     * 
     * This class relies on the usage of STL deques and threads.  The STL deque
     * is not inherently thread-safe, so any implementation of this class must
     * provide adequate synchronization for addition and execution of the task
     * queue.  Execution of the queue should execute the entirety of the queue,
     * leaving it empty.  Synchronization is undefined for whether tasks may be
     * added to the queue mid-execution. 
     * 
     * @brief Parks a thread and allows queuing of tasks for execution.
     * @author Zach Goethel
     */
    class worker_thread
    {
    private:
        /**
         * Locally maintained queue of functions which should be executed.
         * Execution is performed in a first-in first-out (FIFO) fashion. This
         * queue implementation may not be thread-safe, so access should be
         * synchronized by this worker thread container.
         * 
         * The entirety of this queue will be executed upon an update cycle of
         * the worker thread.  When this queue is empty, the worker thread has
         * no tasks and will remain idle.
         * 
         * @brief Task queue for this worker thread.
         */
        std::deque<std::function<void()>> execution_queue;

        /**
         * Indicates whether this worker thread is running on a parked thread.
         * A worker thread may not be parked on two threads, as that may cause
         * unexpected or undefined behavior with two threads pulling from the
         * task queue.
         * 
         * @brief Set to true when this worker thread has a parked thread.
         */
        bool is_parked = false;

        /**
         * Synchronization mutex for accessing the execution queue, locking 
         * while reading from and writing to the queue.
         * 
         * This class relies on the usage of STL deques and threads.  The STL
         * deque is not inherently thread-safe, so any implementation of this
         * class must provide adequate synchronization for addition and
         * execution of the task queue.  Execution of the queue should execute
         * the entirety of the queue, leaving it empty.  Synchronization is
         * undefined for whether tasks may be added to the queue mid-execution. 
         * 
         * @brief Synchronization for accessing the execution queue.
         */
        std::mutex queue_mutex;

        /**
         * Synchronization which allows the worker thread to wait and be
         * notified once a new task is enqueued in the task queue.  This allows
         * threads to sleep rather than infinitely poll the queue and waste CPU
         * cycles doing nothing.
         * 
         * @brief Condition which, once notified, will start a queue execution.
         */
        std::condition_variable execute_condition;

        /**
         * @brief Synchronization mutex for the execution conditional variable.
         */
        std::mutex execute_mutex;

    public:
        /**
         * Creates a new worker thread.  A worker thread can either park on an
         * existing thread, or spawn a new thread on which to perform task
         * execution.  See the class documentation for more information on the
         * behavior when this flag is set.
         * 
         * Set the flag to false for context threads which are already running.
         * If a context is current in an existing thread, manually park a
         * worker thread on the context thread.
         * 
         * Set the flag to true for threads which are simply for multithreaded
         * task execution, or if a context will be created after the worker
         * thread is started.  The created thread will automatically be parked,
         * so no manual call to the parking method is required.
         * 
         * @brief Constructs and starts a new worker thread object.
         * @param create_thread Whether the worker thread should spawn a new 
         *      thread on which to perform task execution.
         */
        worker_thread(bool create_thread = true);

        /**
         * This function will infinitely hang and execute queued tasks as they
         * are added.  The task queue will be repeatedly executed fully, such
         * that each iteration over the queue leaves the queue empty.
         * 
         * When the queue is empty, this function will wait until a new task is
         * added.  The thread will then unblock to execute the task.
         * 
         * The internal queue implementation may not be thread-safe, so access
         * to it must be synchronized during adding and execution.
         * 
         * @brief Function which sits and waits on the worker thread.
         */
        void park();

        /**
         * Adds the provided function to the worker thread's execution queue.
         * The task will be executed at the next iteration over the task queue.
         * Execution is performed in a first-in first-out (FIFO) fashion.
         * 
         * The internal queue implementation may not be thread-safe, so access
         * to it must be synchronized during adding and execution.
         * 
         * @brief Enqueues the task to execute on this worker thread.
         * @param task Function containing the task to queue for execution.
         */
        void execute(std::function<void()> task);
    };

    /**
     * Represents a load-balanced collection of several worker threads.  A set
     * of worker threads will be created an managed, resulting in a unified
     * interface which will automatically execute provided tasks in the most
     * efficient manner.
     * 
     * The default number of threads is the hardware concurrency value, likely
     * the number of logical processors of the host system.
     * 
     * The specific implementation of load-balancing is undefined, but expect a
     * distribution of tasks to worker threads via a round-robbin selection or
     * via finding the current shortest queue.  Once an adequate workload is
     * assigned relatively evenly across worker threads, execution should remain
     * faster than single-threading despite lackluster load-balancing.
     * 
     * @brief A collection of several load-balanced worker threads.
     * @author Zach Goethel
     */
    class worker_pool
    {
    private:
        /**
         * A dynamically allocated array of dynamically allocated worker
         * threads; this array must be properly allocated and deallocated upon
         * startup and shutdown of the worker thread pool.
         * 
         * @brief A dynamically allocated array of pointers to worker threads.
         */
        worker_thread** workers;

        /**
         * This value specifies the number of threads which were created and
         * started upon creation of this worker thread pool.  This number is
         * usually equal to the number of the host system's logical cores, but
         * this value can be overridden via the constructor.
         * 
         * A record of this count is maintained as the worker array is
         * dynamically allocated and deleted.
         * 
         * @brief The number of worker threads assigned to this worker pool.
         */
        int num_workers;

    public:
        /**
         * Upon construction of a worker thread pool, several worker threads
         * will be created and started.  The number of threads is reliant on the
         * user's request, but when no number is provided, the number of threads
         * will match the number of logical cores on the host system.
         * 
         * The constructor will block until all threads are successfully
         * started, and tasks can be assigned immediately following construction.
         * 
         * @brief Constructs and starts a new pool of worker threads.
         * @param num_workers The number of threads to start and assign to this
         *      pool, which defaults to the system's number of logical cores.
         */
        worker_pool(int num_workers = std::thread::hardware_concurrency());

        /**
         * This destructor will cause worker threads to cease executing and
         * terminate.  The worker threads will also be destroyed as a result.
         * 
         * This method will hang until all worker threads have terminated and
         * their memory has been deallocated.
         * 
         * @brief Destroys and deallocates the worker thread pool.
         */
        ~worker_pool();
    };
}