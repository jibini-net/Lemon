package net.jibini.lemon.concurrency.impl

import net.jibini.lemon.concurrency.Worker

import org.slf4j.LoggerFactory

import java.util.concurrent.atomic.AtomicInteger

/**
 * Simple worker pool implementation which allows posting tasks to multiple
 * worker threads. Several instances of the [WorkerThreadImpl] class will be
 * created to meet the worker count requirement. A thread can also call
 * [useThisThread] to become a worker in this pool.
 *
 * Creating a pool with zero workers is allowed; a thread must be added manually
 * via [useThisThread], otherwise attempting to perform a task on the pool may
 * result in unexpected behavior or instability.
 *
 * Load balancing is a simple round-robin selection of workers. Tasks posted to
 * the pool should be relatively simple and short; if large tasks are assigned
 * to a thread, subsequent tasks may take longer than expected to be executed.
 *
 * @author Zach Goethel
 */
class WorkerPoolImpl(workers: Int, private val bufferSize: Int) : Worker
{
    private val log = LoggerFactory.getLogger(javaClass)

    init
    {
        log.debug("Starting pool of {} workers with buffer size {}", workers, bufferSize)
    }

    /**
     * Mutable list of all of the workers in this pool; workers can be added,
     * but adding workers after the pool is active may result in unintended
     * or unpredictable behavior.
     */
    private val workers = MutableList(workers) { Worker.create(bufferSize, start = true) }

    /**
     * Atomic value which tracks the next worker to receive a task.
     */
    private val roundRobin = AtomicInteger(0)

    override fun perform(task: () -> Unit, wait: Boolean)
    {
        // Get the next round-robin worker
        workers[roundRobin.getAndIncrement() % workers.size].perform(task, wait)
    }

    override fun useThisThread()
    {
        // Add the current thread to the list of workers
        val worker = Worker.create(bufferSize, start = false)
        workers += worker
        // Start the created worker on this thread
        worker.useThisThread()
    }

    override fun destroy()
    {
        for (worker in workers)
            worker.destroy()
    }
}