#ifndef VOXEL_MESH_GEN_H
#define VOXEL_MESH_GEN_H

#include <stdint.h>

void gen_voxel_mesh(
    float** mesh_vert_data,
    uint32_t* mesh_vert_size,
    uint32_t** mesh_indices_data,
    uint32_t* mesh_indices_size,
    uint32_t res
);

#endif
