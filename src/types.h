#pragma once

#include <png.h>

typedef struct {
    png_structp png_ptr;
    png_infop info_ptr;

    png_uint_32 width;
    png_uint_32 height;
    png_byte bit_depth;
    png_byte color_type;
    png_bytep *row_pointers;
    png_byte filter_method;
    png_byte interlace_method;
} PNG;
