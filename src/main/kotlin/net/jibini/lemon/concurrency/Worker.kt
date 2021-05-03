package net.jibini.lemon.concurrency

import net.jibini.lemon.Destroyable
import net.jibini.lemon.concurrency.impl.WorkerThreadImpl
import net.jibini.lemon.concurrency.impl.WorkerPoolImpl

import kotlin.concurrent.thread

/**
 * Performs a queue of tasks on a dedicated thread. Any thread can enqueue a
 * task to be performed on a separate thread, allowing for easier multitasking
 * and sharing of a certain thread's resources.
 *
 * An example implementation could allow for execution of tasks on the main
 * thread from secondary threads. Another could include executing graphical
 * commands in a thread which has an active OpenGL context (while the calling
 * thread does not).
 *
 * @author Zach Goethel
 */
interface Worker : Destroyable
{
    /**
     * Performs the provided task on the target thread. If the wait flag is set,
     * this function will block until the task has been executed.
     *
     * @param task The task to enqueue for execution on the target thread.
     * @param wait Whether to wait for the provided task to complete; if set to
     *      true, this function will block (hang) until the provided task has
     *      been executed by the worker thread.
     */
    fun perform(task: () -> Unit, wait: Boolean = true)

    /**
     * If a worker thread is not started, calling this method will start the
     * worker on the current thread.
     *
     * For example, call this on the main thread to allow secondary threads to
     * post tasks to the main thread for execution. In this case, the main
     * thread's worker would have to be initialized without starting.
     */
    fun useThisThread()

    companion object
    {
        /**
         * Convenience value which stores the number of processors.
         */
        private val threads = Runtime.getRuntime().availableProcessors()

        /**
         * Creates a worker thread with the provided number of buffer slots.
         *
         * @param bufferSize The number of slots in the task buffer. This
         *      determines how many tasks can be queued for execution. Once the
         *      buffer is full, enqueues will hang until a buffer slot is
         *      available.
         * @param start Whether a thread should be created and started for this
         *      worker. If set to true, a separate thread will branch to perform
         *      this worker's tasks.
         * @return The created worker to which tasks can be assigned.
         */
        fun create(bufferSize: Int = 512, start: Boolean = true): Worker
        {
            val worker = WorkerThreadImpl(bufferSize)
            if (start)
                thread(name = "Worker", start = true, isDaemon = true, block = worker::useThisThread)

            return worker
        }

        /**
         * Creates a load-balanced group of worker threads.
         *
         * @param workers The number of workers in this pool. This defaults to
         *      the number of available processors in this runtime.
         * @param bufferSize The number of slots in the task buffer. This
         *      determines how many tasks can be queued for execution. Once the
         *      buffer is full, enqueues will hang until a buffer slot is
         *      available.
         * @return The created worker pool to which tasks can be assigned.
         */
        fun createPool(workers: Int = threads, bufferSize: Int = 512): Worker = WorkerPoolImpl(workers, bufferSize)
    }
}