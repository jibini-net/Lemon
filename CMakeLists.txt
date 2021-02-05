cmake_minimum_required (VERSION 3.8)

project ("Lemon")

#
# Export to root and library directories upon build
#
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#
# Include source roots from project and GLFW/GLEW
#
include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_SOURCE_DIR}/glfw/include)
include_directories (${PROJECT_SOURCE_DIR}/glew/include)

#
# Register Lemon core and extension libraries
#
add_library (LemonCore STATIC
    "core/worker_thread.h"
    "core/worker_thread.cpp"

    "core/logger.h"
    "core/logger.cpp"
    )

# Linux build requires linking of thread libraries
if (UNIX)
    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads REQUIRED)
    target_link_libraries (LemonCore PRIVATE Threads::Threads)
endif (UNIX)

# Extension implementation for GLFW
add_library (LemonExt_GLFW STATIC "ext_glfw/ext_glfw.cpp")
# GLFW extension requires core library
target_link_libraries (LemonExt_GLFW LemonCore)

# Extension implementation for OpenGL
add_library (LemonExt_OpenGL STATIC "ext_opengl/ext_opengl.cpp")
# OpenGL extension requires core library
target_link_libraries (LemonExt_OpenGL LemonCore)
# OpenGL extension requires GLFW
target_link_libraries (LemonExt_OpenGL LemonExt_GLFW)

# Extension implementation for Vulkan
add_library (LemonExt_Vulkan STATIC "ext_vulkan/ext_vulkan.cpp")
# Vulkan extension requires core library
target_link_libraries (LemonExt_OpenGL LemonCore)
# Vulkan extension requires GLFW
target_link_libraries (LemonExt_OpenGL LemonExt_GLFW)

#
# Lemon core executable for context bootstrap
#
add_executable (LemonRuntime
    "bootstrap.cpp"
    )
# Lemon runtime requires core library
target_link_libraries (LemonRuntime LemonCore)
# Lemon runtime requires the binding libraries
target_link_libraries (LemonRuntime LemonExt_OpenGL)
target_link_libraries (LemonRuntime LemonExt_Vulkan)

#
# System OpenGL library (must be installed)
#
if (UNIX)
    set (OpenGL_GL_PREFERENCE "GLVND")
endif (UNIX)
find_package (OpenGL REQUIRED)
target_link_libraries (LemonExt_OpenGL OpenGL::GL)

#
# System Vulkan library (must be installed)
#
find_package (Vulkan REQUIRED)
target_link_libraries (LemonExt_Vulkan Vulkan::Vulkan)

#
# GLFW v3.3.2 (extract into project directory)
#
add_subdirectory ("glfw")
target_link_libraries (LemonExt_GLFW ${GLFW_LIBRARIES})

#
# GLEW v2.1.0 (extract into project directory)
#
# Fix for building with 'x64-Release' configuration:
# ================
# Comment out the following lines from the GLEW CMakeLists.txt:
#
# target_link_libraries (glew LINK_PRIVATE -BASE:0x62AA0000)
# target_link_libraries (glew LINK_PRIVATE -nodefaultlib -noentry)
#
add_subdirectory ("glew/build/cmake")
target_link_libraries (LemonExt_OpenGL glew_s ${GLEW_LIBRARIES})