#include "cglm/call.h"
#include <stdlib.h>
#include <stdint.h>

void gen_voxel_mesh(
    float** mesh_vert_data,
    uint32_t* mesh_vert_size,
    uint32_t** mesh_indices_data,
    uint32_t* mesh_indices_size,
    uint32_t res
){
    float surface_value = 0.0;

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
                
                *mesh_vert_size += 8 * 3 * sizeof(float);
                *mesh_indices_size += 6 * 6 * sizeof(uint32_t);            
            }
        }
    }

    *mesh_vert_data = malloc(*mesh_vert_size);
    *mesh_indices_data = malloc(*mesh_indices_size);

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

                            (*mesh_vert_data)[mesh_vert_index++] = cube_pos[0] + vert_offset[0];
                            (*mesh_vert_data)[mesh_vert_index++] = cube_pos[1] + vert_offset[1];
                            (*mesh_vert_data)[mesh_vert_index++] = cube_pos[2] + vert_offset[2];
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

                    (*mesh_indices_data)[mesh_indices_index + (1 - i)] = cube_vert_begin + 0 + 4 * i;
                    // ^ this one's different from the other ones, it works, idk why this one needs to be different
                    (*mesh_indices_data)[mesh_indices_index + i]       = cube_vert_begin + 1 + 4 * i;
                    (*mesh_indices_data)[mesh_indices_index + 2]       = cube_vert_begin + 2 + 4 * i;

                    mesh_indices_index += 3;

                    (*mesh_indices_data)[mesh_indices_index + i]       = cube_vert_begin + 1 + 4 * i;
                    (*mesh_indices_data)[mesh_indices_index + (1 - i)] = cube_vert_begin + 2 + 4 * i;
                    (*mesh_indices_data)[mesh_indices_index + 2]       = cube_vert_begin + 3 + 4 * i;

                    mesh_indices_index += 3;
                }

                create_face[0] = !cube_show_map[z][y-1][x];     // Should -Y show?
                create_face[1] = !cube_show_map[z][y+1][x];     // Should +Y show?

                // Face -Y
                // Face +Y (-Y + 2)
                for (uint8_t i = 0; i < 2; i++){
                    if (!create_face[i]) continue;

                    (*mesh_indices_data)[mesh_indices_index + i]       = cube_vert_begin + 0 + 2 * i;
                    (*mesh_indices_data)[mesh_indices_index + (1 - i)] = cube_vert_begin + 1 + 2 * i;
                    (*mesh_indices_data)[mesh_indices_index + 2]       = cube_vert_begin + 4 + 2 * i;

                    mesh_indices_index += 3;

                    (*mesh_indices_data)[mesh_indices_index + i]       = cube_vert_begin + 1 + 2 * i;
                    (*mesh_indices_data)[mesh_indices_index + (1 - i)] = cube_vert_begin + 5 + 2 * i;
                    (*mesh_indices_data)[mesh_indices_index + 2]       = cube_vert_begin + 4 + 2 * i;

                    mesh_indices_index += 3;
                }

                create_face[0] = !cube_show_map[z][y][x-1];     // Should -X show?
                create_face[1] = !cube_show_map[z][y][x+1];     // Should +X show?

                // Face -X
                // Face +X (-X + 1)
                for (uint8_t i = 0; i < 2; i++){
                    if (!create_face[i]) continue;

                    (*mesh_indices_data)[mesh_indices_index + i]       = cube_vert_begin + 0 + i;
                    (*mesh_indices_data)[mesh_indices_index + (1 - i)] = cube_vert_begin + 6 + i;
                    (*mesh_indices_data)[mesh_indices_index + 2]       = cube_vert_begin + 2 + i;

                    mesh_indices_index += 3;

                    (*mesh_indices_data)[mesh_indices_index + i]       = cube_vert_begin + 0 + i;
                    (*mesh_indices_data)[mesh_indices_index + (1 - i)] = cube_vert_begin + 4 + i;
                    (*mesh_indices_data)[mesh_indices_index + 2]       = cube_vert_begin + 6 + i;

                    mesh_indices_index += 3;
                }
            
            }
        }
    }

    *mesh_indices_size = mesh_indices_index * sizeof(uint32_t);
    *mesh_indices_data = realloc(*mesh_indices_data, *mesh_indices_size);

}