#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

#define MAX_DIM 1000

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    FILE *fp = fopen(argv[1], "rb");
    if (!fp)
        return 1;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        fclose(fp);
        return 1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 0;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    uint32_t width = png_get_image_width(png, info);
    uint32_t height = png_get_image_height(png, info);

    if (width == 0 || height == 0 || width > MAX_DIM || height > MAX_DIM)
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 0;
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
        fclose(fp);
        return 0;
    }

    for (uint32_t i = 0; i < height; i++)
    {
        rows[i] = malloc(rowbytes);
        if (!rows[i])
        {
            for (uint32_t j = 0; j < i; j++)
                free(rows[j]);
            free(rows);
            png_destroy_read_struct(&png, &info, NULL);
            fclose(fp);
            return 0;
        }
    }

    png_read_image(png, rows);
    png_read_end(png, NULL);

    for (uint32_t i = 0; i < height; i++)
        free(rows[i]);
    free(rows);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return 0;
}