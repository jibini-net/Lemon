# Lemon
_Multithreaded rendering engine framework_

Work in progress

## Platform Specifications

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
