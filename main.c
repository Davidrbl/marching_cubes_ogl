#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "gl.h"
#include <GLFW/glfw3.h>

#include "shader.h"
#include "cglm/call.h"

#include "noise3d.h"

#include "voxel_mesh_gen.h"
#include "marching_cubes_mesh_gen.h"

#include "bmp_imp.h"

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

    uint32_t main_program;

    create_program(
        "main.vert",
        NULL,
        NULL,
        NULL,
        "main.frag",
        &main_program
    );

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
        "ct/I36.bmp",
        "ct/I37.bmp",
        "ct/I38.bmp",
        "ct/I39.bmp",
        "ct/I40.bmp",
        "ct/I41.bmp",
        "ct/I42.bmp",
        "ct/I43.bmp",
        "ct/I44.bmp",
        "ct/I45.bmp",
        "ct/I46.bmp",
        "ct/I47.bmp",
        "ct/I48.bmp",
        "ct/I49.bmp",
        "ct/I50.bmp",
        "ct/I51.bmp",
        "ct/I52.bmp",
        "ct/I53.bmp",
        "ct/I54.bmp",
        "ct/I55.bmp",
        "ct/I56.bmp",
        "ct/I57.bmp",
        "ct/I58.bmp",
        "ct/I59.bmp",
        "ct/I60.bmp",
        "ct/I61.bmp",
        "ct/I62.bmp",
        "ct/I63.bmp",
        "ct/I64.bmp",
        "ct/I65.bmp",
        "ct/I66.bmp",
        "ct/I67.bmp",
        "ct/I68.bmp",
        "ct/I69.bmp",
        "ct/I70.bmp",
        "ct/I71.bmp",
        "ct/I72.bmp",
        "ct/I73.bmp",
        "ct/I74.bmp",
        "ct/I75.bmp",
        "ct/I76.bmp",
        "ct/I77.bmp",
        "ct/I78.bmp",
        "ct/I79.bmp",
        "ct/I80.bmp",
        "ct/I81.bmp",
        "ct/I82.bmp",
        "ct/I83.bmp",
        "ct/I84.bmp",
        "ct/I85.bmp",
        "ct/I86.bmp",
        "ct/I87.bmp",
        "ct/I88.bmp",
        "ct/I89.bmp",
        "ct/I90.bmp",
        "ct/I91.bmp",
        "ct/I92.bmp",
        "ct/I93.bmp",
        "ct/I94.bmp",
        "ct/I95.bmp",
        "ct/I96.bmp",
        "ct/I97.bmp",
        "ct/I98.bmp",
        "ct/I99.bmp",
        "ct/I100.bmp",
        "ct/I101.bmp",
        "ct/I102.bmp"
    };

    // char addresses[67][20]; // 67 strings of 20 chars

    // char temp[80] = { 0 };

    // for (uint32_t i = 36; i <= 102; i++)
    // {
    //     sprintf(addresses[i-36], "ct/i%u.bmp", i);

    // }

    // for (uint32_t i = 0; i <= (102 - 36); i++)
    // {
    //     printf("addresses[%u] = %s\n", i, addresses[i]);
    // }

    uint32_t num_addresses = sizeof(addresses)/sizeof(char*);

    uint8_t* bitmap_data = NULL;
    uint32_t bitmap_res = 0;
    bmp_load(addresses, num_addresses, &bitmap_data, &bitmap_res);
    printf("Loaded bmp succesfully!\nres = %u\n", bitmap_res);

    // first non-0 value should be at 0xE9, or 233
    // printf("%02X\n", bitmap_data[233]); // 67
    // printf("%02X\n", bitmap_data[232]); // 00



    float* mesh_vert_data = NULL;
    uint32_t mesh_vert_size = 0;
    uint32_t* mesh_indices_data = NULL;
    uint32_t mesh_indices_size = 0;

    uint32_t res = 40;

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
                // float value = rand() % 2 == 0;
                float value = ((float)rand() / (float)RAND_MAX) * 2.0 - 1.0;

                // printf("getting value at: %u %u %u\n", x, y, z);
                // float value = value_in_bmp(bitmap_data, bitmap_res, x/(res+1), y/(res+1), z/(res+1))/(float)0xFF;
                // value = value * 2.0 - 1.0;

                if (x == 0 || x == res || y == 0 || y == res || z == 0 || z == res) value = -1.0;
                value_map[y*(res+1)*(res+1) + z*(res+1) + x] = value;

            }
        }
    }

    printf("Generating data took %f s\n", glfwGetTime() - before_gen_time);

    // printf("mesh_vert_size/sizeof(float) = %llu\nmesh_indices_size/sizeof(uint32_t) = %llu\n", mesh_vert_size/sizeof(float), mesh_indices_size/sizeof(uint32_t));

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


    float surface_value = .7f;
    gen_marching_cubes_mesh_uint8(
        &mesh_vert_data, &mesh_vert_size,
        bitmap_data, surface_value,
        bitmap_res - 1
    );

    glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);
    printf("a\n");

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
    double end_frame_time = glfwGetTime();
    double dt;

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();

        // Time stuff
        dt = end_frame_time - begin_frame_time;
        begin_frame_time = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_UP) || glfwGetKey(window, GLFW_KEY_DOWN)) {
            surface_value += (float) (glfwGetKey(window, GLFW_KEY_UP) - glfwGetKey(window, GLFW_KEY_DOWN)) * 0.05f;
            gen_marching_cubes_mesh_uint8(
                &mesh_vert_data, &mesh_vert_size,
                bitmap_data, surface_value,
                bitmap_res - 1
            );

            glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);
        }
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
        if (glfwGetKey(window, GLFW_KEY_T)) {
            printf("dt -> %f ms --- framerate -> %f fps\n", dt * 1000., 1. / dt);
        }
        if (glfwGetKey(window, GLFW_KEY_M)){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (glfwGetKey(window, GLFW_KEY_N)){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        // gen_marching_cubes_mesh(
        //     &mesh_vert_data, &mesh_vert_size,
        //     value_map, sin(glfwGetTime()),
        //     res
        // );

        // gen_marching_cubes_mesh_uint8(
        //     &mesh_vert_data, &mesh_vert_size,
        //     bitmap_data, sin(glfwGetTime()),
        //     bitmap_res - 1
        // );

        // glNamedBufferData(VBO, mesh_vert_size, mesh_vert_data, GL_DYNAMIC_DRAW);
        // free(mesh_vert_data);

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

        // glm_scale_uni(model_matrix, 10.0f);
        glm_look(cam_pos, cam_fwd, (vec3){0.0, 1.0, 0.0}, view_matrix);
        glm_perspective(fov, window_width/window_height, 0.001, 100.0, projection_matrix);

        glUseProgram(main_program);
        glBindVertexArray(VAO);

        glUniformMatrix4fv(glGetUniformLocation(main_program, "model"), 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "view"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(main_program, "projection"), 1, GL_FALSE, &projection_matrix[0][0]);

        // glDrawElements(GL_TRIANGLES, mesh_indices_size/sizeof(uint32_t), GL_UNSIGNED_INT, NULL);
        glDrawArrays(GL_TRIANGLES, 0, mesh_vert_size/3/sizeof(float));

        end_frame_time = glfwGetTime();
        glfwSwapBuffers(window);
    }


    free(bitmap_data);
    free(mesh_vert_data);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
