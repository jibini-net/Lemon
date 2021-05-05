package net.jibini.lemon.concurrency.impl

import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.Semaphore
import kotlinx.coroutines.sync.withLock

import net.jibini.lemon.concurrency.Worker

import org.slf4j.LoggerFactory

import java.lang.IllegalStateException

/**
 * Simple implementation of a worker thread which allows queueing and execution
 * of tasks. The queue of tasks is held as a [Function] array with sliding input
 * and output indices. Queue indices and array space are protected by a mutex.
 *
 * Two counting semaphores maintain how many empty buffer slots are available
 * as well as how many tasks are available in the buffer. These two will change
 * inversely, but the separation into two semaphores allows more efficient
 * waiting for either empty slots or newly filled slots; attempting to pull a
 * task from an empty queue or write a task to a full queue will hang and wait.
 *
 * @author Zach Goethel
 */
class WorkerThreadImpl(bufferSize: Int) : Worker
{
    private val log = LoggerFactory.getLogger(javaClass)

    /**
     * Mutex lock which protects access to the input and output indices. Mutual
     * exclusion of these variables avoids serving the same task twice or
     * overwriting a queued task.
     */
    private val mutex = Mutex()

    /**
     * The thread which currently holds the mutex lock; used to avoid hard locks
     * in a manner similar to a recursive mutex.
     */
    private var mutexHolder: Thread? = null

    /**
     * Task buffer as represented by an array of functions; the array is
     * allocated once at initialization and is reused as task indices wrap
     * around.
     */
    private val buffer = arrayOfNulls<() -> Unit>(bufferSize)

    /**
     * Index of the next empty spot in the buffer to add a task.
     */
    private var indexIn = 0

    /**
     * Index of the next spot in the buffer from which to take a task.
     */
    private var indexOut = 0

    /**
     * A semaphore which tracks the currently available tasks in the buffer.
     * Consumers of tasks should wait on this semaphore for available tasks.
     */
    private val availableTasks = Semaphore(bufferSize, bufferSize)

    /**
     * A semaphore which tracks the currently available empty space in the
     * buffer. Producers of tasks should wait on this semaphore for available
     * empty slots.
     */
    private val availableSlots = Semaphore(bufferSize, 0)

    /**
     * This state represents whether the worker thread is active and working.
     */
    private var alive = false

    /**
     * Performs the provided task and blocks until it has been executed on the
     * target thread. This is performed by wrapping the provided task and
     * waiting for signaling from a binary semaphore.
     *
     * @param task The task to run; this method will block until it is complete.
     */
    private fun performWait(task: () -> Unit)
    {
        val await = Semaphore(1, 1)

        perform(
        {
            task()
            await.release()
        }, false)

        runBlocking {
            await.acquire()
        }
    }

    override fun perform(task: () -> Unit, wait: Boolean)
    {
        if (wait)
        {
            performWait(task); return
        }

        runBlocking {
            // Wait for an empty task slot in the buffer
            availableSlots.acquire()

            val t =
            {
                // Write the provided task to the buffer
                buffer[indexIn++] = task
                indexIn %= buffer.size
            }

            if (mutexHolder == Thread.currentThread()) t()
            else
                mutex.withLock {
                    mutexHolder = Thread.currentThread()
                    t()
                    mutexHolder = null
                }
        }

        // Signal that a new task is available in the buffer
        availableTasks.release()
    }

    override fun useThisThread()
    {
        if (alive)
            throw IllegalStateException("Attempted to start a worker thread which was already started")
        alive = true

        // Loop until the thread is no longer active
        //TODO INTERRUPT ON DEATH
        while (alive)
        {
            runBlocking {
                // Wait for a task to be written to the buffer
                availableTasks.acquire()

                val t =
                {
                    // Remove a task from the buffer
                    val task = buffer[indexOut]
                    buffer[indexOut++] = null
                    indexOut %= buffer.size

                    try
                    {
                        // Execute the task within a try block
                        task!!()
                    } catch (ex: Exception)
                    {
                        log.error("Error occurred in posted worker thread task", ex)
                    }
                }

                if (mutexHolder == Thread.currentThread()) t()
                else
                    mutex.withLock {
                        mutexHolder = Thread.currentThread()
                        t()
                        mutexHolder = null
                    }
            }

            // Signal that another buffer slot is open
            availableSlots.release()
        }
    }

    override fun destroy()
    {
        if (!alive) return

        alive = false
    }
}