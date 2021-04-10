#include "application.h"

#include <time.h>
#include <chrono>
#include <math.h>

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

typedef std::chrono::high_resolution_clock high_res;
typedef std::chrono::seconds sec;
typedef std::chrono::nanoseconds nano;

#define STD_DEV_WARNING 17.0

#define delta_sec(e, s) std::chrono::duration_cast<sec>(e - s).count()
#define delta_nano(e, s) std::chrono::duration_cast<nano>(e - s).count()

namespace lemon
{
    void application::start()
    {
        auto last_update = high_res::now();
        int frame_count = 0;
        int update_time = 5;

        auto last_time = last_update;
        double max = -1.0, min = -1.0;

        double mean = 0.0, mean2 = 0.0;
        double variance = 0.0;

        while (this->app_context->is_alive())
        {
            try
            {
                // Run each frame
                this->loop();
            } catch(const std::exception& ex)
            {
                auto error = ex.what();
                log.error(error);
            }

            this->app_context->update();

            {   /* FRAME COUNTING */
                frame_count++;
                auto time = high_res::now();

                int frame_time = (int)delta_nano(time, last_time);
                double frames = 1000000000.0 / frame_time;

                if (frames > max || max == -1.0)
                    max = frames;
                if (frames < min || min == -1.0)
                    min = frames;

                last_time = time;

                /* STANDARD DEVIATION */
                double delta = frames - mean;
                mean += delta / frame_count;
                mean2 += delta * (frames - mean);
                variance = mean2 / frame_count;

                /* LOG OUTPUT */
                if (delta_sec(time, last_update) >= update_time)
                {
                    float rate = (float)frame_count / update_time;
                    double percent_dev = sqrt(variance) / rate * 100;

                    this->log.debug(std::to_string((int)rate) + " +/- "
                        + std::to_string(percent_dev) + "% fps"
                        + " (\033[1;31m" + std::to_string((int)min) + "\033[0;37m"
                        + ", \033[1;32m" + std::to_string((int)max) + "\033[0;37m"
                        + ")");
                    if (percent_dev >= STD_DEV_WARNING)
                        this->log.warn("Abnormal variation in frametimes detected; "
                             + std::to_string(percent_dev)
                             + "% is above the warning threshold");

                    last_update = time;
                    frame_count = 0;

                    max = min = -1.0f;
                    mean = mean2 = variance = 0.0;
                }
            }
        }

        this->log.debug("App context has died on current thread");
        delete this->base_hold;
    }
}
