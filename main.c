#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "gl.h"
#include "glfw3.h"

#include "shader.h"
#include "cglm/call.h"

// i dont see people do this often, but lets see what happens
#include "voxel_mesh_gen.c"

#define MOUSE_SENSITIVITY 0.001
#define CAMERA_SPEED 1.0

static void glfw_error_callback(int error, const char* desc){
    printf("GLFW_ERROR: %d ---\t %s\n", error, desc);
}

int main(){
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()){
        printf("glfw init fail!\n");
        return 1;
    }

    uint32_t window_width = 1024, window_height = 1024;

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Window", NULL, NULL);

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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glViewport(0, 0, window_width, window_height);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    // set up da matrices
    mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;

    float* mesh_vert_data = NULL;
    uint32_t mesh_vert_size = 0;
    uint32_t* mesh_indices_data = NULL;
    uint32_t mesh_indices_size = 0;

    uint32_t res = 30;
    float surface_value = 0.0;

    gen_voxel_mesh(
        &mesh_vert_data, &mesh_vert_size,
        &mesh_indices_data, &mesh_indices_size,
        res
    );



    /*
    {

        bool cube_show_map[res][res][res];

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
                    cube_show_map[z][y][x] = cube_show;
                    if (!cube_show) continue;
                    
                    mesh_vert_size += 8 * 3 * sizeof(float);
                    mesh_indices_size += 6 * 6 * sizeof(uint32_t);            
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

                    bool cube_show = cube_show_map[z][y][x];
                    if (!cube_show) continue;

                    uint32_t cube_vert_begin = mesh_vert_index/3;

                    for (int8_t cz = 0; cz < 2; cz++){
                        for (int8_t cy = 0; cy < 2; cy++){
                            for (int8_t cx = 0; cx < 2; cx++){
                                vec3 vert_offset = {
                                    ((float)cx - 0.5) / res, // -0.5 so it's from 0 to 1 to -0.5 to 0.5
                                    ((float)cy - 0.5) / res,
                                    ((float)cz - 0.5) / res
                                };

                                mesh_vert_data[mesh_vert_index++] = cube_pos[0] + vert_offset[0];
                                mesh_vert_data[mesh_vert_index++] = cube_pos[1] + vert_offset[1];
                                mesh_vert_data[mesh_vert_index++] = cube_pos[2] + vert_offset[2];
                            }
                        }
                    }

                    // -z -y -x = 0
                    // -z -y +x = 1
                    // -z +y -x = 2
                    // -z +y +x = 3
                    // +z -y -x = 4
                    // +z -y +x = 5
                    // +z +y -x = 6
                    // +z +y +x = 7

                    bool create_face[2];

                    create_face[0] = !cube_show_map[z-1][y][x];     // Should -Z show?
                    create_face[1] = !cube_show_map[z+1][y][x];     // Should +Z show?

                    // Face -Z
                    // Face +Z (-Z + 4)

                    for (uint8_t i = 0; i < 2; i++){
                        if (!create_face[i]) continue;

                        mesh_indices_data[mesh_indices_index + (1 - i)] = cube_vert_begin + 0 + 4 * i;
                        // ^ this one's different from the other ones, it works, idk why this one needs to be different
                        mesh_indices_data[mesh_indices_index + i]       = cube_vert_begin + 1 + 4 * i;
                        mesh_indices_data[mesh_indices_index + 2]       = cube_vert_begin + 2 + 4 * i;

                        mesh_indices_index += 3;

                        mesh_indices_data[mesh_indices_index + i]       = cube_vert_begin + 1 + 4 * i;
                        mesh_indices_data[mesh_indices_index + (1 - i)] = cube_vert_begin + 2 + 4 * i;
                        mesh_indices_data[mesh_indices_index + 2]       = cube_vert_begin + 3 + 4 * i;

                        mesh_indices_index += 3;
                    }

                    create_face[0] = !cube_show_map[z][y-1][x];     // Should -Y show?
                    create_face[1] = !cube_show_map[z][y+1][x];     // Should +Y show?

                    // Face -Y
                    // Face +Y (-Y + 2)
                    for (uint8_t i = 0; i < 2; i++){
                        if (!create_face[i]) continue;

                        mesh_indices_data[mesh_indices_index + i]       = cube_vert_begin + 0 + 2 * i;
                        mesh_indices_data[mesh_indices_index + (1 - i)] = cube_vert_begin + 1 + 2 * i;
                        mesh_indices_data[mesh_indices_index + 2]       = cube_vert_begin + 4 + 2 * i;

                        mesh_indices_index += 3;

                        mesh_indices_data[mesh_indices_index + i]       = cube_vert_begin + 1 + 2 * i;
                        mesh_indices_data[mesh_indices_index + (1 - i)] = cube_vert_begin + 5 + 2 * i;
                        mesh_indices_data[mesh_indices_index + 2]       = cube_vert_begin + 4 + 2 * i;

                        mesh_indices_index += 3;
                    }

                    create_face[0] = !cube_show_map[z][y][x-1];     // Should -X show?
                    create_face[1] = !cube_show_map[z][y][x+1];     // Should +X show?

                    // Face -X
                    // Face +X (-X + 1)
                    for (uint8_t i = 0; i < 2; i++){
                        if (!create_face[i]) continue;

                        mesh_indices_data[mesh_indices_index + i]       = cube_vert_begin + 0 + i;
                        mesh_indices_data[mesh_indices_index + (1 - i)] = cube_vert_begin + 6 + i;
                        mesh_indices_data[mesh_indices_index + 2]       = cube_vert_begin + 2 + i;

                        mesh_indices_index += 3;

                        mesh_indices_data[mesh_indices_index + i]       = cube_vert_begin + 0 + i;
                        mesh_indices_data[mesh_indices_index + (1 - i)] = cube_vert_begin + 4 + i;
                        mesh_indices_data[mesh_indices_index + 2]       = cube_vert_begin + 6 + i;

                        mesh_indices_index += 3;
                    }
               
                }
            }
        }

        mesh_indices_size = mesh_indices_index * sizeof(uint32_t);
        mesh_indices_data = realloc(mesh_indices_data, mesh_indices_size);

    }
    */

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
        glm_perspective(fov, window_width/window_height, 0.001, 100.0, projection_matrix);

        glUseProgram(main_program);
        glBindVertexArray(VAO);

        glUniformMatrix4fv(glGetUniformLocation(main_program, "model"), 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "view"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "projection"), 1, GL_FALSE, &projection_matrix[0][0]);

        glDrawElements(GL_TRIANGLES, mesh_indices_size/sizeof(uint32_t), GL_UNSIGNED_INT, NULL);
        // glDrawArrays(GL_POINTS, 0, mesh_vert_size/3/sizeof(float));

        end_frame_time = glfwGetTime();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}