#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

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
    class latch
    {
    protected:
        std::mutex mutex;

        std::condition_variable condition;

    public:
        std::atomic_int count;

        latch(int initial = 1);

        void wait();

        void count_down();

        void count_up();
    };
}
