#pragma once

////////////////////////////////////////////////////////////////////////////////
//                          Lemon 3D Graphics Engine                          //
//                    COPYRIGHT (c) 2021 by ZACH GOETHEL                      //
//  ------------------------------------------------------------------------  //
//  Use freely.  This code is published under the MIT permissive open-source  //
//  license.  For more information, see the license file included with this   //
//  repository.  Good luck, and enjoy!                                        //
//  ------------------------------------------------------------------------  //
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
    #include <semaphore>
#elif __unix__
    #include <semaphore.h>

    // namespace std
    // {
    //     class counting_semaphore
    //     {
    //         private:
    //             sem_t sem;

    //         public:
    //             counting_semaphore(int n, int init)
    //             {
    //                 sem_init(&sem, 0, n);
    //                 for (int i = init; i < n; i++)
    //                     sem_wait(&sem);
    //             }

    //             ~counting_semaphore()
    //             {
    //                 sem_destroy(&sem);
    //             }

    //             void acquire()
    //             {
    //                 sem_wait(&sem);
    //             }

    //             void release()
    //             {
    //                 sem_post(&sem);
    //             }
    //     };
    // }
#endif