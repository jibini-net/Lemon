#include "context.h"

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
    void context::perform(std::function<void()> task, bool wait)
    {
        // Queue directly to worker thread
        if (wait)
            this->thread.execute_wait(task);
        else
            this->thread.execute(task);
    }
}