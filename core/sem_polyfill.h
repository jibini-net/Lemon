#pragma once

#ifdef _WIN32
    #include <semaphore>
#elif __unix__
    #include <semaphore.h>
#endif