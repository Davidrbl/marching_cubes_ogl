#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>

#include "gl.h"

void create_shader(const char* path, GLenum type, uint32_t* dest);

void create_program_compute(const char* compute_shader_path, uint32_t* dest);

void create_program(
    const char* vertex_shader_path,
    const char* tessellation_control_shader_path,
    const char* tessellation_evaluation_shader_path,
    const char* geometry_shader_path,
    const char* fragment_shader_path,
    uint32_t* dest
);

#endif // SHADER_H
