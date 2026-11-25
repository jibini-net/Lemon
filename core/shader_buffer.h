#pragma once

#include <functional>

#include "logger.h"
#include "resource.h"

namespace lemon
{
    /**
     * @brief A shared buffer between video and main memory which can be mapped.
     * @author Zach Goethel
     */
    class shader_buffer : public resource
    {
    protected:
        logger log { "Shader Data"};

        std::mutex mutex;

    public:
        shader_buffer(std::shared_ptr<context> in_context) : resource(in_context)
        { }

        virtual ~shader_buffer()
        { }

        virtual void* map(bool read, bool write)
        { return nullptr; }

        template <typename T>
        T* map_typed(bool read, bool write)
        {
            return static_cast<T*>(map(read, write));
        }

        virtual void unmap()
        { }

        template <typename T>
        void map_scoped(bool read, bool write, std::function<void(T* mapped)> action)
        {
            std::lock_guard<std::mutex> lock(mutex);

            T* mapped = this->map_typed<T>(read, write);
            action(mapped);
            this->unmap();
        }

        virtual void put(void* data, int size)
        { }
    };
}