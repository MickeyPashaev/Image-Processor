#include "image_io.h"
#include "utils.h"
#include <png.h>
#include <stdlib.h>

int read_png_file(char *file_name, PNG *image)
{
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        return 42;
    }

    png_byte header[8];
    if (fread(header, 1, 8, fp) != 8) {
        fclose(fp);
        return 42;
    }

    if (png_sig_cmp(header, 0, 8)) {
        fclose(fp);
        return 42;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return 42;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return 42;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        return 42;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int bit_depth = 0;
    int color_type = 0;
    int interlace_method = 0;
    int compression_method = 0;
    int filter_method = 0;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method,
                 &compression_method, &filter_method);

    if (bit_depth != 8) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 42;
    }

    if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGBA) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 42;
    }

    image->png_ptr = png_ptr;
    image->info_ptr = info_ptr;
    image->width = (int)width;
    image->height = (int)height;
    image->bit_depth = (png_byte)bit_depth;
    image->color_type = (png_byte)color_type;
    image->interlace_method = (png_byte)interlace_method;
    image->filter_method = (png_byte)filter_method;
    image->row_pointers = NULL;

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    image->row_pointers = malloc(sizeof(png_bytep) * image->height);
    if (!image->row_pointers) {
        png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
        fclose(fp);
        return 42;
    }

    for (int y = 0; y < (int)image->height; y++) {
        image->row_pointers[y] = malloc(rowbytes);
        if (!image->row_pointers[y]) {
            for (int i = 0; i < y; i++) {
                free(image->row_pointers[i]);
            }
            free(image->row_pointers);
            png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
            fclose(fp);
            return 42;
        }
    }

    png_read_image(image->png_ptr, image->row_pointers);
    png_read_end(image->png_ptr, NULL);
    fclose(fp);
    return 0;
}

// запись PNG
int write_png_file(char *file_name, PNG *image)
{
    FILE *fp = fopen(file_name, "wb");
    if (!fp) {
        return 1;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, image->width, image->height, image->bit_depth, image->color_type,
                 image->interlace_method, PNG_COMPRESSION_TYPE_BASE, image->filter_method);

    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, image->row_pointers);
    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return 0;
}