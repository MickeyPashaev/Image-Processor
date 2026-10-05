#include "utils.h"
#include <png.h>
#include <stdlib.h>

void print_help()
{
    printf("Help-mode:\n\n");
    printf("Use this flags to choose input and output files:\n");
    printf("--input (-i) <FILE>\n");
    printf("--output (-o) <FILE>\n");
    printf("Use this flags to choose operation:\n\n");

    printf("--rect - to draw rectangle\n");
    printf("Rectangle flags:\n");
    printf("--left_up x1.y1 - upper-left corner\n");
    printf("--right_down x2.y2 - lower right corner\n");
    printf("--thickness <T> - line thickness\n");
    printf("--color RRR.GGG.BBB - color of lines\n");
    printf("--fill - to fill rectangle\n");
    printf("--fill_color RRR.GGG.BBB - color of filling\n\n");

    printf("--ornament - to draw a frame\n");
    printf("Ornament flags:\n");
    printf("--pattern <rectangle/circle/semicircles> - frame type\n");
    printf("--color RRR.GGG.BBB - color of frame\n");
    printf("--thickness <T> - thickness of frame lines\n");
    printf("--count <C> - count of frames\n\n");

    printf("--rotate - to rotate area of image\n");
    printf("Rotate flags:\n");
    printf("--left_up x1.y1 - upper-left corner\n");
    printf("--right_down x2.y2 - lower right corner\n");
    printf("--angle <90/180/270> - choose rotation angle\n\n");

    printf("--info - to show image information\n");
    printf("-h --help - to show help information\n");
}

void print_info_image(PNG *image)
{
    printf("Info image:\n");
    printf("Width: %d\nHeight: %d\nBit depth: %d\nColor type: %d\n", image->width, image->height,
           image->bit_depth, image->color_type);
}

int parse_coords(char *coords, int *x, int *y)
{
    if (sscanf(coords, "%d.%d", x, y) != 2) {
        return 1;
    }
    return 0;
}

int parse_color(const char *str, int *r, int *g, int *b)
{
    if (!str) {
        return 1;
    }

    if (sscanf(str, "%d.%d.%d", r, g, b) != 3) {
        return 1;
    }

    if (*r < 0 || *r > 255 || *g < 0 || *g > 255 || *b < 0 || *b > 255) {
        return 1;
    }
    return 0;
}

int get_channels(const PNG *image)
{
    if (!image) {
        return 0;
    }
    if (image->color_type == PNG_COLOR_TYPE_RGB) {
        return 3;
    }
    if (image->color_type == PNG_COLOR_TYPE_RGBA) {
        return 4;
    }
    return 0;
}

png_bytep get_pixel(PNG *image, int x, int y)
{
    if (!image) {
        return NULL;
    }

    if (x < 0 || x >= (int)image->width || y < 0 || y >= (int)image->height) {
        return NULL;
    }

    int channels = get_channels(image);
    if (channels == 0) {
        return NULL;
    }

    return &(image->row_pointers[y][x * channels]);
}

void set_pixel(PNG *image, int x, int y, int r, int g, int b)
{
    png_bytep px = get_pixel(image, x, y);
    if (!px) {
        return;
    }

    int channels = get_channels(image);

    px[0] = r;
    px[1] = g;
    px[2] = b;

    if (channels == 4) {
        px[3] = 255;
    }
}

void free_png(PNG *image)
{
    if (image->row_pointers) {
        for (int y = 0; y < (int)image->height; y++) {
            free(image->row_pointers[y]);
        }
        free(image->row_pointers);
    }
    png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
}

void swap_nums(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_pixel(png_bytep p1, png_bytep p2, int ch)
{
    for (int i = 0; i < ch; i++) {
        png_byte tmp = p1[i];
        p1[i] = p2[i];
        p2[i] = tmp;
    }
}

void rotate_point(int x, int y, int w, int h, int angle, int *nx, int *ny)
{
    if (angle == 90) {
        *nx = y;
        *ny = w - 1 - x;
    } else {
        *nx = h - 1 - y;
        *ny = x;
    }
}

void free_buf(png_bytep *buf, int size)
{
    for (int i = 0; i < size; i++) {
        free(buf[i]);
    }
    free(buf);
}

void draw_filled_circle(PNG *image, int cx, int cy, int radius, int r, int g, int b)
{
    if (cx + radius < 0 || cx - radius >= (int)image->width || cy + radius < 0 ||
        cy - radius >= (int)image->height) {
        return;
    }

    int start_y = (cy - radius < 0) ? -cy : -radius;
    int end_y = (cy + radius >= (int)image->height) ? (int)image->height - 1 - cy : radius;
    int start_x = (cx - radius < 0) ? -cx : -radius;
    int end_x = (cx + radius >= (int)image->width) ? (int)image->width - 1 - cx : radius;

    for (int y = start_y; y <= end_y; y++) {
        int y_sq = y * y;
        int r_sq = radius * radius;
        for (int x = start_x; x <= end_x; x++) {
            if (x * x + y_sq <= r_sq) {
                set_pixel(image, cx + x, cy + y, r, g, b);
            }
        }
    }
}

void draw_line(PNG *image, int x1, int y1, int x2, int y2, int thickness, int r, int g, int b)
{
    int radius = thickness / 2;

    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        draw_filled_circle(image, x1, y1, radius, r, g, b);

        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}
