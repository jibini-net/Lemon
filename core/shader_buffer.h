#pragma once

#include <functional>

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
     * @brief A shared buffer between video and main memory which can be mapped.
     * @author Zach Goethel
     */
    class shader_buffer
    {
    protected:
        logger log { "Shader Data"};

    public:
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
            T* mapped = this->map_typed<T>(read, write);
            action(mapped);
            this->unmap();
        }

        virtual void put(void* data, int size)
        { }
    };
}