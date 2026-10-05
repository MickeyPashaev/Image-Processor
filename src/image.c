#include "image.h"
#include "utils.h"
#include <math.h>
#include <png.h>
#include <stdlib.h>
#include <string.h>

// task 1
void draw_rect(PNG *image, int x1, int y1, int x2, int y2, int thickness, int r, int g, int b, int fill,
               int fr, int fg, int fb)
{
    draw_line(image, x1, y1, x2, y1, thickness, r, g, b);
    draw_line(image, x2, y1, x2, y2, thickness, r, g, b);
    draw_line(image, x2, y2, x1, y2, thickness, r, g, b);
    draw_line(image, x1, y2, x1, y1, thickness, r, g, b);

    if (fill) {
        if (x1 > x2) {
            swap_nums(&x1, &x2);
        }
        if (y1 > y2) {
            swap_nums(&y1, &y2);
        }

        int offset = thickness / 2;
        for (int y = y1 + offset + 1; y < y2 - offset; y++) {
            for (int x = x1 + offset + 1; x < x2 - offset; x++) {
                set_pixel(image, x, y, fr, fg, fb);
            }
        }
    }
}

void rotate_180(PNG *img, int x1, int y1, int height, int width, int ch)
{
    for (int dy = 0; dy < height / 2; dy++) {
        int up_y = y1 + dy;
        int down_y = y1 + (height - 1 - dy);
        for (int dx = 0; dx < width; dx++) {
            int left_x = x1 + dx;
            int right_x = x1 + (width - 1 - dx);
            png_bytep a = get_pixel(img, left_x, up_y);
            png_bytep b = get_pixel(img, right_x, down_y);
            swap_pixel(a, b, ch);
        }
    }

    if (height % 2 == 1) {
        int mid_y = y1 + height / 2;

        for (int dx = 0; dx < width / 2; dx++) {
            png_bytep a = &(img->row_pointers[mid_y][(x1 + dx) * ch]);
            png_bytep b = &(img->row_pointers[mid_y][(x1 + (width - 1 - dx)) * ch]);
            swap_pixel(a, b, ch);
        }
    }
}

void prepare_area(PNG *img, int *x1, int *y1, int *x2, int *y2)
{
    if (*x1 > *x2)
        swap_nums(x1, x2);
    if (*y1 > *y2)
        swap_nums(y1, y2);

    if (*x1 < 0)
        *x1 = 0;
    if (*y1 < 0)
        *y1 = 0;
    if (*x2 >= img->width)
        *x2 = img->width;
    if (*y2 >= img->height)
        *y2 = img->height;
}

png_bytep *create_buf(int rows, int cols, int ch)
{
    png_bytep *buf = malloc(rows * sizeof(png_bytep));

    for (int i = 0; i < rows; i++) {
        buf[i] = malloc(cols * ch);
    }

    return buf;
}

void fill_rotate_buf(PNG *img, png_bytep *buf, int x1, int y1, int width, int height, int angle, int ch)
{
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            png_bytep src = get_pixel(img, x1 + dx, y1 + dy);
            int nx, ny;
            rotate_point(dx, dy, width, height, angle, &nx, &ny);
            png_bytep dst = &(buf[ny][nx * ch]);
            memcpy(dst, src, ch);
        }
    }
}

void copy_rotated(PNG *img, png_bytep *buffer, int x1, int y1, int width, int height, int out_w, int out_h,
                  int ch)
{
    int cx = x1 + width / 2;
    int cy = y1 + height / 2;
    int dst_x1 = cx - out_w / 2;
    int dst_y1 = cy - out_h / 2;
    int offset_x = 0;
    int offset_y = 0;
    int copy_w = out_w;
    int copy_h = out_h;

    if (dst_x1 < 0) {
        offset_x = -dst_x1;
        copy_w += dst_x1;
        dst_x1 = 0;
    }

    if (dst_y1 < 0) {
        offset_y = -dst_y1;
        copy_h += dst_y1;
        dst_y1 = 0;
    }

    if (dst_x1 + copy_w > img->width) {
        copy_w = img->width - dst_x1;
    }

    if (dst_y1 + copy_h > img->height) {
        copy_h = img->height - dst_y1;
    }

    if (copy_w > 0 && copy_h > 0) {
        for (int dy = 0; dy < copy_h; dy++) {
            for (int dx = 0; dx < copy_w; dx++) {
                png_bytep src = &(buffer[dy + offset_y][(dx + offset_x) * ch]);
                png_bytep dst = get_pixel(img, dst_x1 + dx, dst_y1 + dy);
                memcpy(dst, src, ch);
            }
        }
    }
    free_buf(buffer, out_h);
}

// task 3
void rotate_png(PNG *img, int x1, int y1, int x2, int y2, int angle)
{
    prepare_area(img, &x1, &y1, &x2, &y2);

    int ch = get_channels(img);
    int height = y2 - y1;
    int width = x2 - x1;

    if (angle == 180) {
        rotate_180(img, x1, y1, height, width, ch);
        return;
    }

    int out_w = height;
    int out_h = width;

    png_bytep *buffer = create_buf(out_h, out_w, ch);
    fill_rotate_buf(img, buffer, x1, y1, width, height, angle, ch);
    copy_rotated(img, buffer, x1, y1, width, height, out_w, out_h, ch);
}

void draw_rect_90(PNG *image, int x1, int y1, int x2, int y2, int thickness, int r, int g, int b)
{
    if (x1 > x2) {
        swap_nums(&x1, &x2);
    }

    if (y1 > y2) {
        swap_nums(&y1, &y2);
    }

    int out = thickness / 2;

    for (int y = y1 - out; y < y1 + out; y++) { // up line
        for (int x = x1 - out; x < x2 + out; x++) {
            set_pixel(image, x, y, r, g, b);
        }
    }

    for (int y = y2 - out; y < y2 + out; y++) { // down line
        for (int x = x1 - out; x < x2 + out; x++) {
            set_pixel(image, x, y, r, g, b);
        }
    }

    for (int x = x1 - out; x < x1 + out; x++) { // left line
        for (int y = y1 - out; y < y2 + out; y++) {
            set_pixel(image, x, y, r, g, b);
        }
    }

    for (int x = x2 - out; x < x2 + out; x++) { // right line
        for (int y = y1 - out; y < y2 + out; y++) {
            set_pixel(image, x, y, r, g, b);
        }
    }
}

void draw_rectangle_ornament(PNG *image, int r, int g, int b, int thickness, int count)
{
    for (int i = 0; i < count; i++) {
        int offset = thickness / 2 + i * 2 * thickness;

        int x1 = offset;
        int y1 = offset;
        int x2 = image->width - 1 - offset;
        int y2 = image->height - 1 - offset;

        draw_rect_90(image, x1, y1, x2, y2, thickness, r, g, b);
    }
}

void draw_circle_ornament(PNG *image, int r, int g, int b)
{
    int width = image->width;
    int height = image->height;
    int radius = (width < height ? width : height) / 2;
    int cx = width / 2;
    int cy = height / 2;
    int rad_sq = radius * radius;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy >= rad_sq) {
                set_pixel(image, x, y, r, g, b);
            }
        }
    }
}

void draw_semicircle(PNG *image, int cx, int cy, int radius, int thickness, int r, int g, int b)
{
    int in = thickness / 2;
    int out = thickness / 2;

    int r_in = radius - in;
    int r_out = radius + out;

    if (r_in < 0) {
        r_in = 0;
    }

    int r_in_sq = r_in * r_in;
    int r_out_sq = r_out * r_out;

    for (int y = cy - r_out; y <= cy + r_out; y++) {
        if (y < 0 || y >= image->height)
            continue;
        for (int x = cx - r_out; x <= cx + r_out; x++) {
            if (x < 0 || x >= image->width)
                continue;

            int dx = x - cx;
            int dy = y - cy;
            int dist_sq = dx * dx + dy * dy;

            if (dist_sq >= r_in_sq && dist_sq <= r_out_sq) {
                set_pixel(image, x, y, r, g, b);
            }
        }
    }
}

void draw_semicircles_ornament(PNG *image, int thickness, int count, int r, int g, int b)
{
    int width = image->width;
    int height = image->height;

    // Размеры радиусов по бокам и сверху/снизу
    int radius_w = (int)ceil(width / (2.0 * count));
    int radius_h = (int)ceil(height / (2.0 * count));

    // верх и низ
    for (int i = 0; i < count; i++) {
        int cx = radius_w + i * 2 * radius_w;

        draw_semicircle(image, cx, 0, radius_w, thickness, r, g, b);
        draw_semicircle(image, cx, height - 1, radius_w, thickness, r, g, b);
    }

    // лево и право
    for (int i = 0; i < count; i++) {
        int cy = radius_h + i * 2 * radius_h;

        draw_semicircle(image, 0, cy, radius_h, thickness, r, g, b);
        draw_semicircle(image, width - 1, cy, radius_h, thickness, r, g, b);
    }
}

// task 2
void draw_ornament(PNG *image, const char *pattern, int thickness, int count, int r, int g, int b)
{
    if (strcmp(pattern, "rectangle") == 0) {
        draw_rectangle_ornament(image, r, g, b, thickness, count);
    } else if (strcmp(pattern, "circle") == 0) {
        draw_circle_ornament(image, r, g, b);
    } else if (strcmp(pattern, "semicircles") == 0) {
        draw_semicircles_ornament(image, thickness, count, r, g, b);
    }
}