#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int getpixel(unsigned char *data, int i, int j)
{
    uint32_t pos = (i << 9) + j;
    uint32_t idx = 0;
    uint32_t sum = 0;
    uint32_t xor = 0x55;

    while (idx <= 0x44B89 && sum <= pos)
    {
        int8_t next_idx = data[idx] ^ xor;

        if (next_idx < 0)
        {
            next_idx -= 0x80; // clears the msb
            idx += 4;
        }
        else
            idx += next_idx * 3 + 1;

        sum += next_idx;
    }

    return idx;
}

int main(void)
{
    FILE *f = fopen("data", "r");
    if (!f)
        return 1;
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);

    fseek(f, 0x425, SEEK_SET);
    unsigned char *data = malloc(len - 0x425);
    if (!data)
        return 1;
    fread(data, 1, len - 0x425, f);
    fclose(f);

    unsigned int blocks[8] = {5, 1, 4, 7, 0, 2, 6, 3};

    for (int i = 0; i < 512; ++i)
    {
        for (int j = 0; j < 512; ++j)
        {
            int block_i = blocks[i / 64];
            int block_j = blocks[j / 64];

            int new_i = (block_i << 6) | (i & 63);
            int new_j = (block_j << 6) | (j & 63);

            int pixel = getpixel(data + 3, new_i, new_j);
            int pixel_val = data[pixel] + data[pixel + 1] + data[pixel + 2];
            if (pixel_val > 0xc7)
                putchar('.');
            else
                putchar('o');
        }

        putchar('\n');
    }

    free(data);

    return 0;
}
