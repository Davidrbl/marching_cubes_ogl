#ifndef MARCHING_CUBES_MESH_GEN_H
#define MARCHING_CUBES_MESH_GEN_H

#include <stdint.h>

float surface_lerp_value(float a, float b, float surface);

void gen_marching_cubes_mesh(
    float** mesh_vert_data,
    uint32_t* mesh_vert_size,
    float* value_map,
    const float surface_value,
    uint32_t res
);

void gen_marching_cubes_mesh_uint8(
    float** mesh_vert_data,
    uint32_t* mesh_vert_size,
    uint8_t* value_map,
    const float surface_value,
    uint32_t res
);

#endif
