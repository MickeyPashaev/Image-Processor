#pragma once

#include "types.h"
#include <png.h>

void print_help();
int get_channels(const PNG *image);
png_bytep get_pixel(PNG *image, int x, int y);
void set_pixel(PNG *image, int x, int y, int r, int g, int b);
int parse_coords(char *coords, int *x, int *y);
int parse_color(const char *str, int *r, int *g, int *b);
void free_png(PNG *image);
void swap_nums(int *a, int *b);
void swap_pixel(png_bytep p1, png_bytep p2, int ch);
void rotate_point(int x, int y, int w, int h, int angle, int *nx, int *ny);
void free_buf(png_bytep *buf, int size);
void draw_line(PNG *image, int x1, int y1, int x2, int y2, int thickness, int r, int g, int b);
