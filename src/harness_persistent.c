#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <png.h>

#define MAX_DIM 1000

__AFL_FUZZ_INIT();

typedef struct
{
    unsigned char *data;
    size_t size;
    size_t pos;
} mem_src_t;

static void mem_read(png_structp png, png_bytep out, png_size_t n)
{
    mem_src_t *src = png_get_io_ptr(png);
    if (n > src->size - src->pos)
    {
        png_error(png, "read past end");
        return;
    }
    memcpy(out, src->data + src->pos, n);
    src->pos += n;
}

int main(void)
{
    __AFL_INIT();

    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;

    while (__AFL_LOOP(1000))
    {
        size_t len = __AFL_FUZZ_TESTCASE_LEN;

        if (len < 8)
            continue;

        mem_src_t src = {buf, len, 0};

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png)
            continue;

        png_infop info = png_create_info_struct(png);
        if (!info)
        {
            png_destroy_read_struct(&png, NULL, NULL);
            continue;
        }

        if (setjmp(png_jmpbuf(png)))
        {
            png_destroy_read_struct(&png, &info, NULL);
            continue;
        }

        png_set_read_fn(png, &src, mem_read);
        png_read_info(png, info);

        uint32_t width = png_get_image_width(png, info);
        uint32_t height = png_get_image_height(png, info);

        if (width == 0 || height == 0 || width > MAX_DIM || height > MAX_DIM)
        {
            png_destroy_read_struct(&png, &info, NULL);
            continue;
        }

        png_set_expand(png);
        png_set_strip_16(png);
        png_set_gray_to_rgb(png);
        png_read_update_info(png, info);

        size_t rowbytes = png_get_rowbytes(png, info);

        png_bytep *rows = malloc(sizeof(png_bytep) * height);
        if (!rows)
        {
            png_destroy_read_struct(&png, &info, NULL);
            continue;
        }

        int ok = 1;
        for (uint32_t i = 0; i < height; i++)
        {
            rows[i] = malloc(rowbytes);
            if (!rows[i])
            {
                for (uint32_t j = 0; j < i; j++)
                    free(rows[j]);
                free(rows);
                png_destroy_read_struct(&png, &info, NULL);
                ok = 0;
                break;
            }
        }

        if (!ok)
            continue;

        png_read_image(png, rows);
        png_read_end(png, NULL);

        for (uint32_t i = 0; i < height; i++)
            free(rows[i]);
        free(rows);
        png_destroy_read_struct(&png, &info, NULL);
    }

    return 0;
}