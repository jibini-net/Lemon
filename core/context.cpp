#include "context.h"

namespace lemon
{
    void context::perform(std::function<void()> task, bool wait)
    {
        // Queue directly to worker thread
        if (wait)
            this->thread.execute_wait(task);
        else
            this->thread.execute(task);
    }
}