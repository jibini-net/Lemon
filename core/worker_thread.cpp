#include "worker_thread.h"

#include <chrono>

#include "logger.h"

namespace lemon
{
    worker_thread::worker_thread(bool create_thread)
    {
        if (create_thread)
        {
            owns_thread = true;
            thread_handle = std::thread(&worker_thread::park, this);
        }
    }

    worker_thread::~worker_thread()
    {
        if (owns_thread)
        {
            is_parked.store(false);
            semaphore.release();

            if (thread_handle.joinable())
                thread_handle.join();
        }
    }

    void worker_thread::park()
    {
        if (is_parked.exchange(true))
            throw std::runtime_error("Cannot park a single worker thread more than once concurrently");

        thread_id.store(std::this_thread::get_id());

        while (is_parked.load())
        {
            semaphore.acquire();

            if (!is_parked.load())
                break;

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
        this->execution_queue.add(task);
        semaphore.release();
    }

    void worker_thread::execute_wait(std::function<void()> task)
    {
        if (std::this_thread::get_id() == thread_id.load())
        {
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

        std::counting_semaphore<1> l { 0 };

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

        this->round_robin.store(0);

        for (int i = 0; i < num_workers; i++)
            this->workers.push_back(std::make_unique<worker_thread>(true));
    }

    worker_pool::~worker_pool()
    {
        for (auto& worker : workers)
        {
            worker.reset();
        }
        workers.clear();
    }

    void worker_pool::execute(std::function<void()> task)
    {
        auto worker_index = this->round_robin.fetch_add(1) % (int)this->workers.size();
        this->workers[worker_index]->execute(task);
    }
}
