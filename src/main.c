#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cglm/call.h>

#include "shader.h"

#include "noise3d.h"

#include "voxel_mesh_gen.h"
#include "marching_cubes_mesh_gen.h"

#include "bmp_imp.h"

#define MOUSE_SENSITIVITY 0.001
#define CAMERA_SPEED 1.0

#define USE_CT 1

static void glfw_error_callback(int error, const char* desc){
    printf("GLFW_ERROR: %d ---\t %s\n", error, desc);
}

void GLAPIENTRY gl_debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    int32_t length,
    const GLchar* message,
    const void* userParam
){
    // GL_DEBUG_SEVERITY_HIGH;
    // GL_DEBUG_SEVERITY_MEDIUM;
    // GL_DEBUG_SEVERITY_LOW;

    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
        fprintf(stderr, "GL_ERROR \nsource: %x \ntype: %x \nid: %u \nseverity: %x \nlength: %d \nmessage: %s\nuserParam: %p\n",
            source, type, id, severity, length, message, userParam);
}

int main(void){
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()){
        printf("glfw init fail!\n");
        return 1;
    }

    uint32_t window_width = 1024, window_height = 1024;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "march those cubes", NULL, NULL);

    if (!window){
        printf("create window fail!\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
        printf("load gl fail!\n");
        return 1;
    }

    glfwSwapInterval(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_debug_callback, NULL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    uint32_t main_program;

    create_program(
        "src/shaders/main.vert",
        NULL,
        NULL,
        NULL,
        "src/shaders/main.frag",
        &main_program
    );

    glViewport(0, 0, window_width, window_height);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    // set up da matrices
    mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 view_matrix = GLM_MAT4_IDENTITY_INIT;
    mat4 projection_matrix = GLM_MAT4_IDENTITY_INIT;

#if USE_CT
    #define CT_COUNT_BEGIN 36
    #define CT_COUNT_END 102

    // we define these so the addresses array will have a constant size for the compiler, to avoid variable length arrays

    char* addresses[CT_COUNT_END - CT_COUNT_BEGIN + 1];

    uint32_t num_addresses = CT_COUNT_END-CT_COUNT_BEGIN + 1;

    uint32_t address_str_len = sizeof("ct/IXXX.bmp")/sizeof(char);

    for (uint32_t ct_count = CT_COUNT_BEGIN; ct_count <= CT_COUNT_END; ct_count++)
    {
        uint32_t i = ct_count - CT_COUNT_BEGIN;

        addresses[i] = calloc(address_str_len, sizeof(char));

        sprintf(addresses[i], "ct/I%u.bmp", ct_count);
    }

    uint8_t* bitmap_data = NULL;
    uint32_t bitmap_res = 0;
    bmp_load(addresses, num_addresses, &bitmap_data, &bitmap_res);
    printf("Loaded bmp succesfully!\nres = %u\n", bitmap_res);

    for (uint32_t i = 0; i <= CT_COUNT_END - CT_COUNT_BEGIN; i++)
    {
        free(addresses[i]);
    }
#endif
    float* mesh_vert_data = NULL;
    uint32_t mesh_vert_size = 0;
    uint32_t* mesh_indices_data = NULL;
    uint32_t mesh_indices_size = 0;

    uint32_t res = 40;

    float* value_map = malloc((res+1)*(res+1)*(res+1) * sizeof(float));


    for (uint32_t z = 0; z <= res; z++){
        for (uint32_t y = 0; y <= res; y++){
            for (uint32_t x = 0; x <= res; x++){
                float value = ((float)rand() / (float)RAND_MAX) * 2.0 - 1.0;

                if (x == 0 || x == res || y == 0 || y == res || z == 0 || z == res) value = -1.0;
                value_map[y*(res+1)*(res+1) + z*(res+1) + x] = value;

            }
        }
    }


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

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 3*sizeof(float));
    glVertexArrayElementBuffer(VAO, EBO);

    float surface_value = .7f;

#if USE_CT
    gen_marching_cubes_mesh_uint8(
        &mesh_vert_data, &mesh_vert_size,
        bitmap_data, surface_value,
        bitmap_res - 1
    );
#endif

    glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);

    vec3 cam_pos = {.482504f, .210423f, 1.234449f};
    vec2 cam_rot = {.1125f, .011999f}; // Pitch --- Yaw
    vec3 cam_fwd = GLM_VEC3_ZERO_INIT;

    double prev_mouse_x;
    double prev_mouse_y;
    glfwGetCursorPos(window, &prev_mouse_x, &prev_mouse_y);
    double mouse_x;
    double mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    float camera_speed = 0.0025f;
    float mouse_sensitivity = 0.0015f;
    float fov = GLM_PI_2f;

    double begin_frame_time = glfwGetTime();
    double dt;
    uint64_t frame = 0;

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        // Time stuff
        begin_frame_time = glfwGetTime();

#if USE_CT
        if (glfwGetKey(window, GLFW_KEY_UP) || glfwGetKey(window, GLFW_KEY_DOWN)) {
            surface_value += (float) (glfwGetKey(window, GLFW_KEY_UP) - glfwGetKey(window, GLFW_KEY_DOWN)) * 0.05f;
            gen_marching_cubes_mesh_uint8(
                &mesh_vert_data, &mesh_vert_size,
                bitmap_data, surface_value,
                bitmap_res - 1
            );

            glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);
        }
#endif
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, 1);
        if (glfwGetKey(window, GLFW_KEY_1)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (glfwGetKey(window, GLFW_KEY_2)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (glfwGetKey(window, GLFW_KEY_M)){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (glfwGetKey(window, GLFW_KEY_N)){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

#if USE_CT
#else
        gen_marching_cubes_mesh(
            &mesh_vert_data, &mesh_vert_size,
            value_map, sin(glfwGetTime()),
            res
        );

        glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);
        free(mesh_vert_data);
#endif

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
        mul = (float) (glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S));
        cam_pos[0] += sinf(-cam_rot[1]) * mul * camera_speed;
        cam_pos[2] += -cosf(cam_rot[1]) * mul * camera_speed;

        mul = (float) (glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A));
        cam_pos[0] += cosf(cam_rot[1]) * mul * camera_speed;
        cam_pos[2] += sinf(-cam_rot[1]) * mul * camera_speed;

        mul = (float) (glfwGetKey(window, GLFW_KEY_SPACE) - glfwGetKey(window, GLFW_KEY_LEFT_SHIFT));
        cam_pos[1] += mul * camera_speed;

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

        glDrawArrays(GL_TRIANGLES, 0, mesh_vert_size/3/sizeof(float));

        glfwSwapBuffers(window);

        dt = glfwGetTime() - begin_frame_time;
        if (frame % 100 == 0) printf("dt: %f\n", dt*1000);

        frame++;
    }

#if USE_CT
    free(bitmap_data);
#endif

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
