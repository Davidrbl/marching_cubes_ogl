#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "bmp_imp.h"

void bmp_load(char* const* addresses, uint32_t num_addresses, uint8_t** bitmap, uint32_t* bmp_res){
    uint32_t insert_offset = 0;

    bool bitmap_initted = false;

    for (uint32_t i = 0; i < num_addresses; i++){
        const char* address = addresses[i];

        FILE* fp = fopen(address, "r");
        if (!fp){
            fprintf(stderr, "couldn't open file: %s\n", address);
        }

        uint32_t file_size;
        uint32_t bmp_offset;

        fseek(fp, 0x02, SEEK_SET); // the bmp file size is at 0x02
        fread(&file_size, sizeof(uint32_t), 1, fp); // the file size is 4 bytes

        fseek(fp, 0x0A, SEEK_SET); // the bmp offset is at 0x0A
        fread(&bmp_offset, sizeof(uint32_t), 1, fp); // the offset is 4 bytes

        uint32_t map_size = file_size - bmp_offset;

        uint32_t res = (uint32_t)sqrt(map_size);

        if (!bitmap_initted){
            *bitmap = malloc(res*res*res);
            printf("malloced %d bytes\n", res*res*res);
            assert(*bitmap);

            *bmp_res = res;

            bitmap_initted = true;
        }

        for (uint8_t j = 0; j < 7; j++){
            // !!! DIT IS NIET GOED !!!
            // we zetten nu elke 'plak' zes keer in memory, om in elke dimensie dezelfde resolutie te krijgen
            // dit is erg memory en tijd inefficient
            // en moeten we fixen
            fseek(fp, bmp_offset, SEEK_SET);
            fread(*bitmap + insert_offset, map_size, sizeof(uint8_t), fp);
            insert_offset += map_size;
        }

        fclose(fp);
    }

    printf("insert_offset: %u\n", insert_offset);
}

uint8_t value_in_bmp(uint8_t* bmp, uint32_t bmp_res, float x, float y, float z){
    uint32_t ix = floorf(x * bmp_res);
    uint32_t iy = floorf(y * bmp_res);
    uint32_t iz = floorf(z * bmp_res);

    return bmp[iz*bmp_res*bmp_res + iy*bmp_res + ix];
}

