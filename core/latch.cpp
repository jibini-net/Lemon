#include "latch.h"

#include "logger.h"

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
    latch::latch(int initial)
    {
        this->count = initial;
    }

    void latch::wait()
    {
        while (this->count > 0)
        {
            std::unique_lock<std::mutex> lock(this->mutex);
        
            if (this->count > 0)
                this->condition.wait(lock);
        }
    }

    void latch::count_down()
    {
        if (this->count > 0)
        {
            while (!this->mutex.try_lock())
                this->condition.notify_all();
            
            this->count--;
            this->condition.notify_all();

            this->mutex.unlock();
        }
    }

    void latch::count_up()
    {
        while (!this->mutex.try_lock())
            this->condition.notify_all();
        
        this->count++;
        this->condition.notify_all();

        this->mutex.unlock();
    }
}
