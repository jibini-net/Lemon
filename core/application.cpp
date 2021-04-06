#include "application.h"

#include <time.h>

#include "resource_stack.h"

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
    void application::start()
    {
        time_t last_update = time(NULL);
        int frame_count = 0;
        float update_time = 5.0;

        while (this->app_context->is_alive())
        {
            // Run each frame

            this->app_context->update();

            frame_count++;
            auto time = std::time(NULL);

            if (difftime(time, last_update) >= update_time)
            {
                float rate = (float)frame_count / update_time;
                this->log.debug(std::to_string(rate) + " fps");

                last_update = time;
                frame_count = 0;
            }
        }

        this->log.debug("App context has died on current thread");
        delete this->base_hold;
    }
}
