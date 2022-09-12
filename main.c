#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "gl.h"
#include "glfw3.h"

#include "shader.h"
#include "cglm/call.h"

static inline float noise3d(float x, float y, float z){
    x = x * 2 - 1;
    y = y * 2 - 1;
    z = z * 2 - 1;

    float d = sqrtf(x*x + y*y + z*z) / sqrtf(2);
    d = 1.0 - d;
    d = d * 2 - 1;

    return d;

}

// #include "noise.h"
#include "new_noise.h"

// i dont see people do this often, but lets see what happens
#include "voxel_mesh_gen.c"
#include "marching_cubes_mesh_gen.c"

#include "bmp_imp.c"

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

    const char* addresses[] = {
        "ct/i36.bmp",
        "ct/i37.bmp",
        "ct/i38.bmp",
        "ct/i39.bmp",
        "ct/i40.bmp",
        "ct/i41.bmp",
        "ct/i42.bmp",
        "ct/i43.bmp",
        "ct/i44.bmp",
        "ct/i45.bmp",
        "ct/i46.bmp",
        "ct/i47.bmp",
        "ct/i48.bmp",
        "ct/i49.bmp",
        "ct/i50.bmp",
        "ct/i51.bmp",
        "ct/i52.bmp",
        "ct/i53.bmp",
        "ct/i54.bmp",
        "ct/i55.bmp",
        "ct/i56.bmp",
        "ct/i57.bmp",
        "ct/i58.bmp",
        "ct/i59.bmp",
        "ct/i60.bmp",
        "ct/i61.bmp",
        "ct/i62.bmp",
        "ct/i63.bmp",
        "ct/i64.bmp",
        "ct/i65.bmp",
        "ct/i66.bmp",
        "ct/i67.bmp",
        "ct/i68.bmp",
        "ct/i69.bmp",
        "ct/i70.bmp",
        "ct/i71.bmp",
        "ct/i72.bmp",
        "ct/i73.bmp",
        "ct/i74.bmp",
        "ct/i75.bmp",
        "ct/i76.bmp",
        "ct/i77.bmp",
        "ct/i78.bmp",
        "ct/i79.bmp",
        "ct/i80.bmp",
        "ct/i81.bmp",
        "ct/i82.bmp",
        "ct/i83.bmp",
        "ct/i84.bmp",
        "ct/i85.bmp",
        "ct/i86.bmp",
        "ct/i87.bmp",
        "ct/i88.bmp",
        "ct/i89.bmp",
        "ct/i90.bmp",
        "ct/i91.bmp",
        "ct/i92.bmp",
        "ct/i93.bmp",
        "ct/i94.bmp",
        "ct/i95.bmp",
        "ct/i96.bmp",
        "ct/i97.bmp",
        "ct/i98.bmp",
        "ct/i99.bmp",
        "ct/i100.bmp",
        "ct/i101.bmp",
        "ct/i102.bmp"
    };

    uint32_t num_addresses = sizeof(addresses)/sizeof(char*);

    uint8_t* bitmap_data = NULL;
    // bmp_load(addresses, num_addresses, &bitmap_data);
    printf("Loaded bmp succesfully!\n");

    float* mesh_vert_data = NULL;
    uint32_t mesh_vert_size = 0;
    uint32_t* mesh_indices_data = NULL;
    uint32_t mesh_indices_size = 0;

    uint32_t res = 20;
    float surface_value = 0.0;

    // gen_voxel_mesh(
    //     &mesh_vert_data, &mesh_vert_size,
    //     &mesh_indices_data, &mesh_indices_size,
    //     res
    // );

    double before_gen_time = glfwGetTime();

    float* value_map = malloc((res+1)*(res+1)*(res+1) * sizeof(float));

    for (uint32_t z = 0; z <= res; z++){
        for (uint32_t y = 0; y <= res; y++){
            for (uint32_t x = 0; x <= res; x++){
                // bool solid = rand() % 2 == 0;
                float value = ((float)rand() / (float)RAND_MAX) * 2.0 - 1.0;
                // float value = (x > 10 && x < 30);
                // float value = 0.0;
                if (x == 0 || x == res || y == 0 || y == res || z == 0 || z == res) value = -1.0;
                // value_map[y][z][x] = value;
                value_map[y*(res+1)*(res+1) + z*(res+1) + x] = value;

            }
        }
    }

    // gen_marching_cubes_mesh(
    //     &mesh_vert_data, &mesh_vert_size,
    //     value_map, 0.0,
    //     res
    // );

    printf("Generating mesh took %f s\n", glfwGetTime() - before_gen_time);

    printf("mesh_vert_size/sizeof(float) = %u\nmesh_indices_size/sizeof(uint32_t) = %u\n", mesh_vert_size/sizeof(float), mesh_indices_size/sizeof(uint32_t));    

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

    vec3 cam_pos = {0.0, 0.0, 0.0};
    vec2 cam_rot = {0.0, 0.0}; // Pitch --- Yaw
    vec3 cam_fwd = GLM_VEC3_ZERO_INIT;

    double prev_mouse_x;
    double prev_mouse_y;
    glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    float camera_speed = 0.2;
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
        if (glfwGetKey(window, GLFW_KEY_B)) {
            for (uint32_t i = 0; i < mesh_vert_size/3/sizeof(float); i++){
                printf("vert %u\t%f --- %f --- %f\n", i,
                    mesh_vert_data[i*3+0], mesh_vert_data[i*3+1], mesh_vert_data[i*3+2] 
                );
            }
            printf("\n");
        }
        if (glfwGetKey(window, GLFW_KEY_1)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (glfwGetKey(window, GLFW_KEY_2)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        gen_marching_cubes_mesh(
            &mesh_vert_data, &mesh_vert_size,
            value_map, sin(begin_frame_time),
            res
        );

        glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);
        free(mesh_vert_data);

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

        // glm_scale_uni(model_matrix, 10.0f);
        glm_look(cam_pos, cam_fwd, (vec3){0.0, 1.0, 0.0}, view_matrix);
        glm_perspective(fov, window_width/window_height, 0.001, 100.0, projection_matrix);

        glUseProgram(main_program);
        glBindVertexArray(VAO);

        glUniformMatrix4fv(glGetUniformLocation(main_program, "model"), 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "view"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "projection"), 1, GL_FALSE, &projection_matrix[0][0]);

        glDrawElements(GL_TRIANGLES, mesh_indices_size/sizeof(uint32_t), GL_UNSIGNED_INT, NULL);
        glDrawArrays(GL_TRIANGLES, 0, mesh_vert_size/3/sizeof(float));

        end_frame_time = glfwGetTime();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}