#pragma once

#include "types.h"

void draw_rect(PNG *image, int x1, int y1, int x2, int y2, int thickness, int r, int g, int b, int fill,
               int fr, int fg, int fb);
void rotate_png(PNG *image, int x1, int y1, int x2, int y2, int angle);
void draw_ornament(PNG *image, const char *pattern, int thickness, int count, int r, int g, int b);
void free_png(PNG *image);
void print_info_image(PNG *image);
