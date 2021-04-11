#include "bootstrap.h"

#include "ext_opengl/ext_opengl.h"
#include "ext_opengl/gl_ssbo.h"
#include "ext_opengl/gl_program.h"
#include "ext_glfw/ext_glfw.h"

#include <iostream>
#include <thread>
#include <filesystem>
#include <math.h>

#include "application.h"
#include "logger.h"
#include "worker_thread.h"
#include "resource.h"
#include "static_mesh.h"

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
    // Define bootstrapped primary workers
    worker_thread main_thread(false);
    worker_pool primary_pool;

    /**
     * Iteratively finds the provided delimiter and trims the start of the
     * string until no instance of the delmiter remains.  Removed portions of
     * the input string are pushed back onto the provided output vector.
     * Instances of the delimiter will be removed in the produced elements.
     * 
     * If no instances of the delimiter exist in the input stream, the output
     * vector will have one element (the input value).
     * 
     * @brief Splits the provided string and adds the elements to the output.
     * @param str Input string to split into elements.
     * @param delim Delimiter by which to separate the string.
     * @param output Vector reference which will have elements added.
     */
    void split_string(std::string str, std::string delim, std::vector<std::string>& output)
    {
        size_t index;
        
        // Search for the next delimiter index
        while ((index = str.find(delim)) != std::string::npos)
        {
            // Push back the element onto the output vector
            output.push_back(str.substr(0, index));
            // Remove the characters from the input string
            str.erase(0, index + delim.size());
        }

        // Push back the last remaining element
        output.push_back(str);
    }

    /**
     * This function will be the first task posted to the main thread for
     * execution.  It may post future tasks, but must not hang infinitely.
     * 
     * Its primary purpose is to initialize the rendering engine and any
     * required contexts for applications maintained by this runtime instance.
     * This may also include creating additional threads and contexts, loading
     * configuration, and allocating resources as necessary to start
     * applications.
     * 
     * @brief Runtime instance initialization and bootstrapping method.
     */
    void start()
    {
        auto src_vert = read_file("shaders/default.vert");
        auto src_frag = read_file("shaders/default.frag");

        static gl_context gl(4, 6, true, true);
        static gl_program shader(gl, src_vert, src_frag);

        gl.perform([]()
        {
            GLuint vertex_array;
            glGenVertexArrays(1, &vertex_array);
            glBindVertexArray(vertex_array);
        });

        static std::vector<shader_buffer*> blocks;

        {
            std::string fname = "models/lucy.obj";
            const float heuristic = 0.03575f * 1.25f;
            long long model_size = std::filesystem::file_size(fname);

            static logger log("OBJ Loader");

            struct vec3
            {
                float x, y, z;
            };

            std::vector<vec3> vertices;
            vertices.reserve((int)(model_size * heuristic));
            std::vector<vec3> vertex_normals;
            vertex_normals.reserve((int)(model_size * heuristic));

            log.debug("Reserving heuristic buffer of "
                 + std::to_string(vertices.capacity())
                 + " vertices");

            render_data* current = nullptr;
            int i = 0;

            read_file(fname, false, [&](std::string line)
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
                                log.debug("Allocating next mesh block of "
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

        static application app(gl, [&](double delta)
        {
            gl.perform([]()
            {
                glViewport(0, 0, 1400, 900);

                glEnable(GL_DEPTH_TEST);
                glClear(GL_DEPTH_BUFFER_BIT);
            });

            for (int i = 0; i < blocks.size(); i++)
            {
                auto b = (gl_ssbo*)blocks[i];
                b->bind_base();

                gl.perform([]()
                {
                    glDrawArrays(GL_TRIANGLES, 0, MESH_BLOCK_SIZE);
                });

                primary_pool.execute([=]()
                {
                    b->map_scoped<render_data>(true, true, [=](auto mapped)
                    {
                        int k = 0;
                        for (k; k < mapped->num_vertices; k++)
                        {
                            auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
                            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
                            auto sec = (double)millis / 1000.0;

                            auto x = &mapped->vertices[k].position[0];
                            auto y = &mapped->vertices[k].position[1];
                            auto z = &mapped->vertices[k].position[2];

                            //*y += (float)delta * cos((double)(sec + *x / 3));
                            //*x += (float)delta * sin((double)(sec + *x / 3));
                            auto dist = sqrt(*x * *x + *z * *z);
                            auto angl = atan2(*z, *x);
                            *x -= (float)(sin(angl) * delta * dist);
                            *z += (float)(cos(angl) * delta * dist);
                        }
                    });
                });
            }
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