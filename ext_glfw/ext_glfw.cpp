#include "ext_glfw.h"

namespace lemon
{
    // Initialize static context counter
    std::atomic<int> glfw_context::context_count(0);

    glfw_context::glfw_context() : context()
    {
        static worker_thread polling(true);

        // GLFW operations must be executed on the main thread
        // Operation will hang until executed
        main_thread.execute_wait([&]()
        {
            // Initialize if this is the first/only context
            if (glfw_context::context_count.fetch_add(1) == 0)
            {
                // Attempt to initialize GLFW library
                if (!glfwInit())
                    throw std::runtime_error("Could not initialize GLFW windowing library");
                
                this->log.info("Initialized global GLFW context");

                polling.execute([&]()
                {
                    while (context_count > 0)
                    {
                        main_thread.execute_wait(::glfwPollEvents);
                        std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    }
                });
            }
        });
    }

    glfw_context::~glfw_context()
    {
        // GLFW operations must be executed on the main thread
        // Operation will hang until executed
        main_thread.execute_wait([&]()
        {
            // Terminate if this is the last/only context
            if (glfw_context::context_count.fetch_sub(1) == 1)
            {
                this->log.info("Terminating global GLFW context");

                // Terminate the GLFW library
                glfwTerminate();
            }
        });
    }
}