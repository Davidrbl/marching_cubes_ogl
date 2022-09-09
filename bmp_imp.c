#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

void bmp_load(const char** addresses, uint32_t num_addresses, uint8_t** bitmap){
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
            *bitmap = malloc(map_size * num_addresses);
            assert(*bitmap);
            bitmap_initted = true;
        }

        fseek(fp, bmp_offset, SEEK_SET);
        // printf("")
        fread(*bitmap + insert_offset, map_size, sizeof(uint8_t), fp);
        insert_offset += map_size;

        printf("file_size = %u\tbmp_offset = %u\tres = %u\n", file_size, bmp_offset, res);

        fclose(fp);
    }
}