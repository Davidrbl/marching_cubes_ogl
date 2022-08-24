#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "gl.h"
#include "glfw3.h"

#include "shader.h"
#include "cglm/call.h"

#define MOUSE_SENSITIVITY 0.001
#define CAMERA_SPEED 1.0

static void glfw_error_callback(int error, const char* desc){
    printf("GLFW_ERROR: %d ---\t %s\n", error, desc);
}

static inline float noise3d(float x, float y, float z){
    return 1.0;
}

int main(){
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()){
        printf("glfw init fail!\n");
        return 1;
    }

    uint32_t window_width = 1024, window_height = 1024;

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Cock and balls", NULL, NULL);

    if (!window){
        printf("create window fail!\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
        printf("load gl fail!\n");
        return 1;
    }

    uint32_t main_program;

    create_program(
        "main.vert",
        NULL,
        NULL,
        NULL,
        "main.frag",
        &main_program
    );

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glViewport(0, 0, window_width, window_height);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    // set up da matrices
    mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;

    // Generate the mesh
    // float mesh_vert_data[] = {
    //     0.0, 0.0, 0.0,
    //     0.0, 1.0, 0.0,
    //     1.0, 0.0, 0.0,
    //     1.0, 1.0, 0.0
    // };

    // uint32_t mesh_vert_size = sizeof(mesh_vert_data);

    // uint32_t mesh_indices_data[] = {
    //     0, 2, 1,
    //     1, 2, 3
    // };

    // uint32_t mesh_indices_size = sizeof(mesh_indices_data);

    float* mesh_vert_data = NULL;
    uint32_t mesh_vert_size = 0;
    uint32_t* mesh_indices_data = NULL;
    uint32_t mesh_indices_size = 0;

    uint32_t res = 3;
    float surface_value = 0.0;

    {
        // //  get the sizes needed to store the stuff
        // for (uint32_t z = 0; z < res; z++){
        //     for (uint32_t y = 0; y < res; y++){
        //         for (uint32_t x = 0; x < res; x++){
        //             float pos_value = noise3d(x/res, y/res, z/res);
        //             bool solid = pos_value > surface_value;
        //             if (!solid) continue;
        //             // it's solid
        //             // the size of a cube = 8 vertices + 6 faces * 6 indices
        //             mesh_vert_size += 3 * 8 * sizeof(float);
        //             mesh_indices_size += 6 * 6 * sizeof(uint32_t);
        //         }
        //     }
        // }

        // mesh_vert_data = malloc(mesh_vert_size);
        // mesh_indices_data = malloc(mesh_indices_size);

        // uint32_t mesh_vert_index = 0;
        // uint32_t mesh_indices_index = 0;

        // for (uint32_t z = 0; z < res; z++){
        //     for (uint32_t y = 0; y < res; y++){
        //         for (uint32_t x = 0; x < res; x++){
        //             float pos_value = noise3d(x/res, y/res, z/res);
        //             bool solid = pos_value > surface_value;
        //             if (!solid) continue;
        //             // da bitch is solid

        //             vec3 cube_pos = {
        //                 (x + 0.5) / res,
        //                 (y + 0.5) / res,
        //                 (z + 0.5) / res
        //             };

        //             float cube_scale = 1/res;

        //             uint32_t cube_vert_begin = mesh_vert_index/3;

        //             #pragma unroll(2)
        //             for (int8_t _z = 0; _z < 2; _z++){
        //                 #pragma unroll(2)
        //                 for (int8_t _y = 0; _y < 2; _y++){
        //                     #pragma unroll(2)
        //                     for (int8_t _x = 0; _x < 2; _x++){
        //                         mesh_vert_data[mesh_vert_index++] = cube_pos[0] + (2 * _x - 1) * cube_scale;
        //                         mesh_vert_data[mesh_vert_index++] = cube_pos[1] + (2 * _y - 1) * cube_scale;
        //                         mesh_vert_data[mesh_vert_index++] = cube_pos[2] + (3 * _z - 1) * cube_scale;
        //                     }  
        //                 }   
        //             }

        //             // -z -y -x = 0
        //             // -z -y +x = 1
        //             // -z +y -x = 2
        //             // -z +y +x = 3
        //             // +z -y -x = 4
        //             // +z -y +x = 5
        //             // +z +y -x = 6
        //             // +z +y +x = 7

        //             // Face -Z
        //             for (uint8_t i = 0; i < 2; i++){
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 0 + 4 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 1 + 4 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 2 + 4 * i;

        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 1 + 4 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 2 + 4 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 3 + 4 * i;
        //             }
        //             // Face +Z (-Z + 4)
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 4;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 5;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 6;

        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 5;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 6;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 7;

        //             // Face -Y
        //             for (uint8_t i = 0; i < 2; i++){
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 0 + 2 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 1 + 2 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 4 + 2 * i;

        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 1 + 2 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 5 + 2 * i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 4 + 2 * i;
        //             }
        //             // Face +Y (-Y + 2)
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 2;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 3;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 6;

        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 3;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 7;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 6;

        //             // Face -X
        //             for (uint8_t i = 0; i < 2; i++){
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 0 + i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 6 + i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 2 + i;

        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 0 + i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 4 + i;
        //                 mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 6 + i;
        //             }
        //             // Face +X (-X + 1)
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 1;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 7;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 3;

        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 1;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 5;
        //             // mesh_indices_data[mesh_indices_index++] = cube_vert_begin + 7;
        //         }
        //     }
        // }

        // printf("mesh_vert_index = %u\nmesh_indices_index = %u\n", mesh_vert_index, mesh_indices_index);
    }

    {
        for (int8_t z = 0; z < res; z++){
            for (int8_t y = 0; y < res; y++){
                for (int8_t x = 0; x < res; x++){
                    vec3 cube_pos = {
                        (x + 0.5) / res,
                        (y + 0.5) / res,
                        (z + 0.5) / res
                    };
                    float cur_value = noise3d(cube_pos[0], cube_pos[1], cube_pos[2]);
                    bool cube_show = cur_value > surface_value;
                    if (!cube_show) continue;
                    
                    mesh_vert_size += 8 * 3 * sizeof(float);
                    
                }
            }
        }

        mesh_vert_data = malloc(mesh_vert_size);
        mesh_indices_data = malloc(mesh_indices_size);

        uint32_t mesh_vert_index = 0;
        uint32_t mesh_indices_index = 0;

        // now the actual vertex and index calculations

        for (int8_t z = 0; z < res; z++){
            for (int8_t y = 0; y < res; y++){
                for (int8_t x = 0; x < res; x++){
                    vec3 cube_pos = {
                        (x + 0.5) / res,
                        (y + 0.5) / res,
                        (z + 0.5) / res
                    };
                    float cur_value = noise3d(cube_pos[0], cube_pos[1], cube_pos[2]);
                    bool cube_show = cur_value > surface_value;
                    if (!cube_show) continue;

                    for (int8_t cz = 0; cz < 2; cz++){
                        for (int8_t cy = 0; cy < 2; cy++){
                            for (int8_t cx = 0; cx < 2; cx++){
                                vec3 vert_offset = {
                                    ((float)cx * 2.0 - 1.0) / res,
                                    ((float)cy * 2.0 - 1.0) / res,
                                    ((float)cz * 2.0 - 1.0) / res
                                };

                                printf("vert_offset --- cx : %hhd cy : %hhd cz : %hhd\n%f --- %f --- %f\n",
                                    cx, cy, cz, 
                                    vert_offset[0], vert_offset[1], vert_offset[2]    
                                );

                                mesh_vert_data[mesh_vert_index++] = cube_pos[0] + vert_offset[0];
                                mesh_vert_data[mesh_vert_index++] = cube_pos[1] + vert_offset[1];
                                mesh_vert_data[mesh_vert_index++] = cube_pos[2] + vert_offset[2];
                            }
                        }
                   }


                    // mesh_vert_data[mesh_vert_index++] = cube_pos[0];
                    // mesh_vert_data[mesh_vert_index++] = cube_pos[1];
                    // mesh_vert_data[mesh_vert_index++] = cube_pos[2];
                
                }
            }
        }


    }

    printf("mesh_vert_size = %u\nmesh_indices_size = %u\n", mesh_vert_size/sizeof(float), mesh_indices_size/sizeof(uint32_t));    

    // the stupid gpu objects
    uint32_t VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_STATIC_DRAW);
    glNamedBufferData(EBO, mesh_indices_size, mesh_indices_data, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // glEnableVertexArrayAttrib(VAO, 1);
    // glVertexArrayAttribBinding(VAO, 0, 0);
    // glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // glEnableVertexArrayAttrib(VAO, 2);
    // glVertexArrayAttribBinding(VAO, 0, 0);
    // glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 3*sizeof(float));
    glVertexArrayElementBuffer(VAO, EBO);

    vec3 cam_pos = {0.0, 0.0, 1.0};
    vec2 cam_rot = {0.0, 0.0}; // Pitch --- Yaw
    vec3 cam_fwd = GLM_VEC3_ZERO_INIT;

    double prev_mouse_x;
    double prev_mouse_y;
    glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    float camera_speed = 1.0;
    float mouse_sensitivity = 0.001;
    float fov = GLM_PI_2f;

    double begin_frame_time = glfwGetTime();
    double end_frame_time = glfwGetTime();
    double dt = end_frame_time - begin_frame_time;

    while (!glfwWindowShouldClose(window)){        
        glfwPollEvents();

        // Time stuff
        dt = end_frame_time - begin_frame_time;
        begin_frame_time = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, 1);
        if (glfwGetKey(window, GLFW_KEY_P)) printf("cam_pos: %f --- %f --- %f\n", cam_pos[0], cam_pos[1], cam_pos[2]);
        if (glfwGetKey(window, GLFW_KEY_R)) printf("cam_rot: %f --- %f\n", cam_rot[0], cam_rot[1]);
        if (glfwGetKey(window, GLFW_KEY_F)) printf("cam_fwd: %f --- %f --- %f\n", cam_fwd[0], cam_fwd[1], cam_fwd[2]);
        if (glfwGetKey(window, GLFW_KEY_L)){
            vec4 v = {0.0, 0.0, 0.0, 1.0};
            glm_mat4_mulv(model_matrix, v, v);
            glm_mat4_mulv(view_matrix, v, v);
            glm_mat4_mulv(projection_matrix, v, v);
            printf("pos: %f --- %f --- %f --- %f\n",
                v[0], v[1], v[2], v[3]
            );
        }
        if (glfwGetKey(window, GLFW_KEY_1)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (glfwGetKey(window, GLFW_KEY_2)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update the camera position and rotation
        cam_rot[0] += (float)(prev_mouse_y - mouse_y) * mouse_sensitivity; // Pitch
        cam_rot[1] += (float)(prev_mouse_x - mouse_x) * mouse_sensitivity; // Yaw

        prev_mouse_x = mouse_x;
        prev_mouse_y = mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        if (cam_rot[0] > 0.49f * GLM_PI){
            cam_rot[0] = 0.49f * GLM_PI;
        }
        
        if (cam_rot[0] < -0.49f * GLM_PI){
            cam_rot[0] = -0.49f * GLM_PI;
        }

        float a = cosf(cam_rot[0]); // cos pitch

        cam_fwd[0] = sinf(-cam_rot[1]) * a;
        cam_fwd[1] = sinf(cam_rot[0]);
        cam_fwd[2] = -cosf(cam_rot[1]) * a; // - so -z is forward

        float mul;
        mul = glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S);
        cam_pos[0] += sinf(-cam_rot[1]) * mul * camera_speed * dt;
        cam_pos[2] += -cosf(cam_rot[1]) * mul * camera_speed * dt;

        mul = glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A);
        cam_pos[0] += cosf(cam_rot[1]) * mul * camera_speed * dt;
        cam_pos[2] += sinf(-cam_rot[1]) * mul * camera_speed * dt;

        mul = glfwGetKey(window, GLFW_KEY_SPACE) - glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        cam_pos[1] += mul * camera_speed * dt;

        // Calculate da matrices for this frame
        glm_mat4_identity(model_matrix);
        glm_mat4_identity(view_matrix);
        glm_mat4_identity(projection_matrix);

        glm_look(cam_pos, cam_fwd, (vec3){0.0, 1.0, 0.0}, view_matrix);
        glm_perspective(fov, window_width/window_height, 0.1, 100.0, projection_matrix);

        glUseProgram(main_program);
        glBindVertexArray(VAO);

        glUniformMatrix4fv(glGetUniformLocation(main_program, "model"), 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "view"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "projection"), 1, GL_FALSE, &projection_matrix[0][0]);

        // glDrawElements(GL_TRIANGLES, mesh_indices_size/sizeof(uint32_t), GL_UNSIGNED_INT, NULL);
        glDrawArrays(GL_POINTS, 0, mesh_vert_size/3/sizeof(float));

        end_frame_time = glfwGetTime();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}