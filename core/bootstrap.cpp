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

#define EXT std::shared_ptr<extension>(new ext_opengl(4, 6, true, true))

namespace lemon
{
    // Define bootstrapped primary workers
    worker_thread main_thread(false);
    worker_pool primary_pool;

    std::mutex blocks_mut;

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

    class bootstrap : public application
    {
        private:
            std::shared_ptr<shader_program> shader;
            std::shared_ptr<shader_buffer> bodies;
            std::vector<std::shared_ptr<shader_buffer>> blocks;

        public:
            bootstrap() : application(EXT)
            { }

            void setup()
            {
                // Load basic shader program GLSL sources
                auto src_vert = read_file("shaders/default.vert");
                auto src_frag = read_file("shaders/default.frag");
                shader = ext->create_program(app_context, src_vert, src_frag);

                // Bind a default vertex array (required)
                app_context->perform([]()
                {
                    GLuint vertex_array;
                    glGenVertexArrays(1, &vertex_array);
                    glBindVertexArray(vertex_array);
                });

                bodies = ext->create_buffer(app_context, 1);
                bodies->put(new body_data, sizeof(body_data));
                bodies->map_scoped<body_data>(true, true, [&](auto mapped)
                {
                    auto w = 70.0f * 1.5f;
                    auto h = 45.0f * 1.5f;

                    mapped->num_bodies = 1;
                    mapped->bodies[0] =
                    {
                        // .transform = mat::ortho(-w, w, -h, h, -512.0f, 512.0f),
                        .transform = mat::perspective(14.0f / 9.0f, 3.14f / 2.0f, 0.1f, 512.0f),
                        .diffuse =  { 1.0f, 1.0f, 1.0f, 1.0f },

                        .diff = { 0.1f },
                        .spec = { 1.9f },
                        .spec_power = { 70.0f },
                        .ambient = { 0.05f }
                    };
                });

                primary_pool.execute([&]()
                {
                    while (true)
                    {
                        std::string fname = "models/xyzrgb_dragon.obj";
                        const float heuristic = 0.03575f * 1.25f;
                        long long model_size = std::filesystem::file_size(fname);

                        blocks_mut.lock(); //TODO
                        blocks.clear();
                        blocks_mut.unlock();

                        static logger log("OBJ Loader");

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
                                        arr.x = strtof(elements[1].c_str(), NULL);
                                        arr.y = strtof(elements[2].c_str(), NULL);
                                        arr.z = strtof(elements[3].c_str(), NULL);

                                        if (line[1] == 'n')
                                            vertex_normals.push_back(arr);
                                        else
                                            vertices.push_back(vec3
                                            {
                                                arr.x, arr.y, arr.z
                                            });
                                        break;

                                    case 'f':
                                        if (current == nullptr || i == MESH_BLOCK_SIZE)
                                        {
                                            log.debug("Allocating next mesh block of "
                                                + std::to_string(MESH_BLOCK_SIZE)
                                                + " vertices");

                                            if (blocks.size() > 0)
                                                blocks.back()->unmap();

                                            blocks.push_back(ext->create_buffer(app_context, 0));

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
                                                .position = { vert.x, vert.y, vert.z, 1.0f },
                                                .diffuse = { 1.0f, 1.0f, 1.0f, 1.0f },
                                                .normal_vector = { norm.x,  norm.y,  norm.z }, 
                                                .texture_coord = { 0.0f,  1.0f },
                                                .body_index = { 0U }
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
                        
                        // Unmap the last unfilled buffer if there is one
                        if (blocks.size() > 0)
                        {
                            current->num_vertices = i;
                            blocks.back()->unmap();
                        }

                        std::this_thread::sleep_for(std::chrono::seconds(10));
                    }
                });

            }

            void update(double delta)
            {
                // Prepare each frame for rendering (viewport, depth buffer)
                app_context->perform([]()
                {
                    glViewport(0, 0, 1400, 900);
                    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

                    glEnable(GL_DEPTH_TEST);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    glUniform1f(glGetUniformLocation(1, "time"), (float)glfwGetTime());
                });

                // Render each model mesh block; not yet abstracted
                blocks_mut.lock();
                for (int i = 0; i < blocks.size(); i++)
                {
                    static_cast<gl_ssbo*>(blocks[i].get())->bind_base();
                    static_cast<gl_ssbo*>(bodies.get())->bind_base();

                    app_context->perform([]()
                    {
                        glDrawArrays(GL_TRIANGLES, 0, MESH_BLOCK_SIZE);
                    });
                }
                blocks_mut.unlock();
            }

            void destroy()
            {
                this->blocks.clear();

                this->bodies.reset();
                this->shader.reset();
            }
    };
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
int main()
{
    static lemon::logger log("(Bootstrap)");

    log.info("\033[1;33m===============================================================");
    log.info("                 \033[1;31mLemon\033[0m created by \033[1;31mZach Goethel");
    log.info("\033[1;33m===============================================================");

    lemon::main_thread.execute([]()
    {
        static lemon::bootstrap app;
    });

    lemon::main_thread.park();
}