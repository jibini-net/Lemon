#include "bootstrap.h"

#include <iostream>
#include <thread>
#include <math.h>
#include <filesystem>

#include "application.h"
#include "logger.h"
#include "worker_thread.h"
#include "resource.h"
#include "static_mesh.h"

#include "ext_opengl/ext_opengl.h"
#include "ext_opengl/gl_ssbo.h"
#include "ext_opengl/gl_program.h"
#include "ext_glfw/ext_glfw.h"

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
    worker_thread main_thread(false);
    worker_pool primary_pool;

    void split_string(std::string str, std::string delim, std::vector<std::string>& output)
    {
        size_t index;

        while ((index = str.find(delim)) != std::string::npos)
        {
            std::string sub = str.substr(0, index);
            output.push_back(sub);

            str.erase(0, index + delim.size());
        }

        output.push_back(str);
    }

    /**
     * This function will be the first task posted to the main thread for execution.
     * It may post future tasks, but must not hang infinitely.
     * 
     * Its primary purpose is to initialize the rendering engine and any required
     * contexts for applications maintained by this runtime instance. This may also
     * include creating additional threads and contexts, loading configuration, and
     * allocating resources as necessary to start applications.
     * 
     * @brief Runtime instance initialization and bootstrapping method.
     */
    void start()
    {
        auto src_vert = read_file("shaders/default.vert");
        auto src_frag = read_file("shaders/default.frag");

        static gl_context gl(4, 6, true, true);
        static gl_program shader(gl, src_vert, src_frag);

        gl.perform([&]()
        {
            GLuint vertex_array;
            glGenVertexArrays(1, &vertex_array);
            glBindVertexArray(vertex_array);
        });

        static std::vector<shader_buffer*> blocks;

        {
            const float heuristic = 0.03575f * 1.25f;
            long long model_size = std::filesystem::file_size("models/lucy.obj");

            static logger log("OBJ Loader");

            struct vec3
            {
                float x, y, z;
            };

            std::vector<vec3> vertices;
            vertices.reserve((int)(model_size * heuristic));
            std::vector<vec3> vertex_normals;
            vertex_normals.reserve((int)(model_size * heuristic));

            log.debug("Reserving heuristic buffer of " + std::to_string(vertices.capacity())
                 + " vertices");

            render_data* current = nullptr;
            int i = 0;

            read_file("models/lucy.obj", false, [&](std::string line)
            {
                if (line.size() > 0)
                {
                    std::vector<std::string> elements, sub_elements;
                    split_string(line, " ", elements);

                    vec3 arr;

                    switch (line[0])
                    {
                        case 'v':
                            arr =
                            {
                                .x = strtof(elements[1].c_str(), NULL),
                                .y = strtof(elements[2].c_str(), NULL),
                                .z = strtof(elements[3].c_str(), NULL)
                            };

                            if (line[1] == 'n')
                                vertex_normals.push_back(arr);
                            else
                                vertices.push_back(arr);
                            break;

                        case 'f':
                            if (current == nullptr || i == MESH_BLOCK_SIZE)
                            {
                                log.debug("Allocating next mesh render data block of "
                                     + std::to_string(MESH_BLOCK_SIZE)
                                     + " vertices");

                                if (blocks.size() > 0)
                                    blocks.back()->unmap();

                                blocks.push_back(new gl_ssbo(gl, 0));
                                blocks.back()->put(new render_data, sizeof(render_data));
                                current = blocks.back()->map_typed<render_data>(true, true);

                                i = 0;
                            }

                            for (int j = 0; j < 3; j++)
                            {
                                sub_elements.clear();
                                split_string(elements[j + 1], "/", sub_elements);

                                auto vert = vertices[strtol(sub_elements[0].c_str(), NULL, 10) - 1];
                                auto norm = vertex_normals[strtol(sub_elements[2].c_str(), NULL, 10) - 1];

                                current->vertices[i++] =
                                {
                                    /* VERT */ { vert.x, vert.y, vert.z, 1.0f },
                                    /* DIFF */ { norm.x,  norm.y,  norm.z, 1.0f },
                                    /* NORM */ { norm.x,  norm.y,  norm.z }, { -1.0f },
                                    /* TEXC */ { 0.0f,  1.0f }, {-1.0f, -1.0f }
                                };
                            }
                    }
                }
            });

            log.info("Mesh loaded with "
                 + std::to_string((blocks.size() - 1) * MESH_BLOCK_SIZE + i)
                 + " vertices ("
                 + std::to_string(blocks.size())
                 + " allocated blocks)");
            
            if (blocks.size() > 0)
            {
                current->num_vertices = i;
                blocks.back()->unmap();
            }
        }

        static application app(gl, [&]()
        {
            gl.perform([&]()
            {
                glViewport(0, 0, 1400, 900);

                glEnable(GL_DEPTH_TEST);
                glClear(GL_DEPTH_BUFFER_BIT);

                for (int i = 0; i < blocks.size(); i++)
                {
                    auto b = (gl_ssbo*)blocks[i];
                    b->bind_base();

                    glDrawArrays(GL_TRIANGLES, 0, MESH_BLOCK_SIZE);
                }
            });
        });
    }
}

/**
 * This application entrypoint parks the main thread for context- and windowing-
 * related tasks. The main thread will be parked by a worker thread which will
 * accept any operations which must be run on the main thread for thread-safety
 * reasons. This may include GLFW polling and other context-related operations.
 * 
 * It also spawns a collection of worker threads for arbitrary tasks. The number
 * of threads created is subject to change. These threads are global and may be
 * used by multiple contexts; they are not specific to any single context.
 * 
 * @brief Bootstraps the runtime in preparation for application operations.
 */
int main(void)
{
    static lemon::logger log("(Bootstrap)");

    log.info("\033[1;33m===============================================================");
    log.info("                 \033[1;31mLemon\033[0m created by \033[1;31mZach Goethel");
    log.info("\033[1;33m===============================================================");

    lemon::main_thread.execute(lemon::start);
    lemon::main_thread.park();

    return 0;
}