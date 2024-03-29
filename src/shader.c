#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

void create_shader(const char* path, GLenum type, uint32_t* dest) {
    *dest = glCreateShader(type);

    FILE* file_pointer = fopen(path, "rb");

    if (!file_pointer) {
        exit(42);
    }

    fseek(file_pointer, 0, SEEK_END);

    size_t file_len = ftell(file_pointer);

    fseek(file_pointer, 0, SEEK_SET);

    GLchar* source = malloc((file_len + 1) * sizeof(GLchar));

    fread(source, sizeof(GLchar), file_len, file_pointer);
    source[file_len] = '\0';

    fclose(file_pointer);

    glShaderSource(*dest, 1, (const GLchar* const*)&source, NULL);

    glCompileShader(*dest);

    int32_t succes = 0;
    glGetShaderiv(*dest, GL_COMPILE_STATUS, &succes);
    if (succes != GL_TRUE) {
        int32_t error_len = 0;
        glGetShaderiv(*dest, GL_INFO_LOG_LENGTH, &error_len);
        GLchar error_string[error_len + 1];
        glGetShaderInfoLog(*dest, error_len, &error_len, error_string);

        glDeleteShader(*dest);

        printf("SHADER COMPILE ERROR: %s\n%s\n", path, error_string);
        exit(1);
    }

    free(source);
}

void create_program_compute(const char* compute_shader_path, uint32_t* dest) {
    *dest = glCreateProgram();

    uint32_t compute_shader = 0;
    create_shader(compute_shader_path, GL_COMPUTE_SHADER, &compute_shader);

    glAttachShader(*dest, compute_shader);

    glLinkProgram(*dest);

    int32_t succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        int32_t error_len = 0;
        glGetShaderiv(compute_shader, GL_INFO_LOG_LENGTH, &error_len);
        GLchar error_string[error_len + 1];
        glGetShaderInfoLog(compute_shader, error_len, &error_len, error_string);

        printf("SHADER LINK ERROR\n");
        printf("%s\n", error_string);
        exit(1);
    }

    glDeleteShader(compute_shader);
}

void create_program(
    const char* vertex_shader_path,
    const char* tessellation_control_shader_path,
    const char* tessellation_evaluation_shader_path,
    const char* geometry_shader_path,
    const char* fragment_shader_path,
    uint32_t* dest
) {
    *dest = glCreateProgram();

    uint32_t vertex_shader = 0;
    uint32_t tessellation_control_shader = 0;
    uint32_t tessellation_evaluation_shader = 0;
    uint32_t geometry_shader = 0;
    uint32_t fragment_shader = 0;

    if (vertex_shader_path) {
        create_shader(vertex_shader_path, GL_VERTEX_SHADER, &vertex_shader);
        glAttachShader(*dest, vertex_shader);
    }
    if (tessellation_control_shader_path) {
        create_shader(tessellation_control_shader_path, GL_TESS_CONTROL_SHADER, &tessellation_control_shader);
        glAttachShader(*dest, tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_path) {
        create_shader(tessellation_evaluation_shader_path, GL_TESS_EVALUATION_SHADER, &tessellation_evaluation_shader);
        glAttachShader(*dest, tessellation_evaluation_shader);
    }
    if (geometry_shader_path) {
        create_shader(geometry_shader_path, GL_GEOMETRY_SHADER, &geometry_shader);
        glAttachShader(*dest, geometry_shader);
    }
    if (fragment_shader_path) {
        create_shader(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);
        glAttachShader(*dest, fragment_shader);
    }

    glLinkProgram(*dest);

    int32_t succes = 0;
    glGetProgramiv(*dest, GL_LINK_STATUS, &succes);
    if (succes != GL_TRUE) {
        printf("SHADER LINK ERROR\n");
        exit(1);
    }

    if (vertex_shader_path) {
        glDeleteShader(vertex_shader);
    }
    if (tessellation_control_shader_path) {
        glDeleteShader(tessellation_control_shader);
    }
    if (tessellation_evaluation_shader_path) {
        glDeleteShader(tessellation_evaluation_shader);
    }
    if (geometry_shader_path) {
        glDeleteShader(geometry_shader);
    }
    if (fragment_shader_path) {
        glDeleteShader(fragment_shader);
    }
}
