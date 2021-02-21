# Lemon
_Multithreaded rendering engine framework_

This framework aims to provide an interface through which to write real-time responsive graphical applications.  Lemon should enable uniform access to graphical hardware’s capabilities, removing barriers of entry such as OpenGL’s threading limitations and Vulkan’s low-level management requirements.  The framework allows abstraction between several graphical APIs, filling the gap between OpenGL and Vulkan applications.

A common problem faced by OpenGL and other graphical developers is issuing graphical commands from their respective graphical contexts.  For example, an OpenGL render call can only be made from a single thread where the OpenGL context is active; similarly, OpenAL audio calls may only be made from an OpenAL thread.

This issue arises when a game or application is multithreaded, and that application attempts to render an object or play a sound from a secondary thread.

A major benefit of Vulkan over OpenGL is its ability to be parallelized via command buffers.  This framework offers an implementation which helps to work around OpenGL’s threading limitations, thus allowing multiple threads to issue OpenGL calls despite the aforementioned limitation.

Through the Lemon framework, no end-developer should have to directly interface with a specific graphical framework such as OpenGL or Vulkan.  The two should be abstracted to a common interface, which increases portability of graphical applications.  Additionally, the end-developer should not have to stress over contexts and synchronization.

## Research Goals

Through the development of this framework, multiple multithreading implementations and implementation methodologies will be applied and tested for their fitness to meet the framework’s requirements.

Two primary factors to test are the overall speed of a multithreaded graphical implementation (outputting the most frames in the shortest time) as well as the consistency of frame times and user experience (limiting the number of stutters and dropped frames).  The preferrable implementation is one which offers the latter, where a user will experience responsive and consistent feedback from their interactive applications.

Testing conditions will assume that the end-developer will not synchronize or optimize their own graphical calls, thus the Lemon framework should be able to handle, synchronize, and optimize interaction with the GPU to most efficiently perform what the application has requested.

Variations may be made to the synchronization of worker threads, implementations of thread pools, and methods of aggregating multiple render calls into fewer larger operations (reducing call overhead, allowing the GPU to better execute large tasks).

## Platform Specifications

The framework is written in C++ for cross-platform support across many systems.  The utilized context libraries are likewise cross-platform, which allows use across several UNIX operating systems and Windows.

* GLFW windowing library

* GLEW context and extension management

* Vulkan SDK

* OpenGL SDK

The library will also aim to utilize modern graphical API features with no emphasis on backwards-compatibility for old OpenGL versions or features.

* OpenGL 3.3/4.3 (core profiles; no immediate mode support)

* OpenGL ES 2.0/3.0 (GL ES 3.0 is feature-compatible with GL 4.3)

* Vulkan 1.1/1.2

Through inheritance of GLFW’s and Kronos’ APIs’ wide platform support, the library should also implicitly support several platforms and architectures.

- [x] Windows
- [x] Linux
- [x] macOS
- [x] X86
- [x] ARM32/64
