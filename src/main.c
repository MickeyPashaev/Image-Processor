#include "image.h"
#include "image_io.h"
#include "utils.h"
#include <getopt.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    OPT_INFO,
    OPT_RECT,
    OPT_LEFT_UP,
    OPT_RIGHT_DOWN,
    OPT_THICKNESS,
    OPT_COLOR,
    OPT_FILL,
    OPT_FILL_COLOR,
    OPT_ORNAMENT,
    OPT_PATTERN,
    OPT_COUNT,
    OPT_ROTATE,
    OPT_ANGLE
};

struct option long_options[] = {{"input", required_argument, 0, 'i'},
                                {"output", required_argument, 0, 'o'},
                                {"help", no_argument, 0, 'h'},
                                {"info", no_argument, 0, OPT_INFO},

                                {"rect", no_argument, 0, OPT_RECT},
                                {"left_up", required_argument, 0, OPT_LEFT_UP},
                                {"right_down", required_argument, 0, OPT_RIGHT_DOWN},
                                {"thickness", required_argument, 0, OPT_THICKNESS},
                                {"color", required_argument, 0, OPT_COLOR},
                                {"fill", no_argument, 0, OPT_FILL},
                                {"fill_color", required_argument, 0, OPT_FILL_COLOR},

                                {"ornament", no_argument, 0, OPT_ORNAMENT},
                                {"pattern", required_argument, 0, OPT_PATTERN},
                                {"count", required_argument, 0, OPT_COUNT},

                                {"rotate", no_argument, 0, OPT_ROTATE},
                                {"angle", required_argument, 0, OPT_ANGLE},

                                {0, 0, 0, 0}};

int main(int argc, char *argv[])
{
    printf("Course work for option 4.15, created by Mekhman Pashaev\n");
    if (argc == 1) {
        print_help();
        return 0;
    }

    int rect_mode = 0;
    int ornament_mode = 0;
    int rotate_mode = 0;

    int help_mode = 0;
    int info_mode = 0;

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int thickness = 0;
    char *color = NULL;
    int fill_flag = 0;
    char *fill_color = NULL;
    int count = 0;
    char *pattern = NULL;
    int angle = 0;

    int left_up_set = 0;
    int right_down_set = 0;

    int opt;
    char *input_file = NULL;
    char *output_file = NULL;

    while ((opt = getopt_long(argc, argv, "i:o:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'h':
                help_mode = 1;
                break;
            case OPT_INFO:
                info_mode = 1;
                break;
            case OPT_RECT:
                rect_mode = 1;
                break;
            case OPT_LEFT_UP:
                if (parse_coords(optarg, &x1, &y1)) {
                    printf("Error parsing left_up coordinates\n");
                    return 40;
                }
                left_up_set = 1;
                break;
            case OPT_RIGHT_DOWN:
                if (parse_coords(optarg, &x2, &y2)) {
                    printf("Error parsing right_down coordinates\n");
                    return 40;
                }
                right_down_set = 1;
                break;
            case OPT_THICKNESS:
                thickness = atoi(optarg);
                if (thickness <= 0) {
                    printf("Error. Thickness must be > 0\n");
                    return 41;
                }
                break;
            case OPT_COLOR:
                color = optarg;
                break;
            case OPT_FILL:
                fill_flag = 1;
                break;
            case OPT_FILL_COLOR:
                fill_color = optarg;
                break;
            case OPT_ORNAMENT:
                ornament_mode = 1;
                break;
            case OPT_PATTERN:
                if (strcmp(optarg, "rectangle") != 0 && strcmp(optarg, "circle") != 0 &&
                    strcmp(optarg, "semicircles") != 0) {
                    printf("Error. There is no such pattern\n");
                    return 46;
                }
                pattern = optarg;
                break;
            case OPT_COUNT:
                count = atoi(optarg);
                if (count <= 0) {
                    printf("Count must be > 0\n");
                    return 42;
                }
                break;
            case OPT_ROTATE:
                rotate_mode = 1;
                break;
            case OPT_ANGLE:
                angle = atoi(optarg);
                if (angle != 90 && angle != 180 && angle != 270) {
                    printf("Wrong angle\n");
                    return 46;
                }
                break;
            case '?':
                return 40;
            default:
                printf("Unknown flag\n");
                return 42;
        }
    }

    if (help_mode) {
        print_help();
        return 0;
    }

    int modes_cnt = rect_mode + ornament_mode + rotate_mode + info_mode;
    if (modes_cnt > 1) {
        printf("Error: Only one operation can be chosen.\n");
        return 43;
    }
    if (modes_cnt == 0) {
        printf("Error: No operation chosen.\n");
        return 43;
    }

    if (!input_file && optind < argc) {
        input_file = argv[argc - 1];
    }

    if (!input_file) {
        printf("Error: No input file chosen.\n");
        return 45;
    }

    if (!output_file) {
        output_file = "out.png";
    }

    if (strcmp(input_file, output_file) == 0) {
        printf("Error: Input and output files cannot be the same.\n");
        return 42;
    }

    PNG image;
    if (read_png_file(input_file, &image)) {
        printf("Error reading PNG file\n");
        return 45;
    }

    if (info_mode) {
        print_info_image(&image);
        free_png(&image);
        return 0;
    }

    if (rect_mode) {
        if (!left_up_set || !right_down_set || !thickness || !color) {
            printf("Error: Missing necessary arguments\n");
            free_png(&image);
            return 44;
        }
        if (fill_flag && !fill_color) {
            printf("Error: Missing fill_color\n");
            free_png(&image);
            return 44;
        }

        int r, g, b;
        int fr = 0, fg = 0, fb = 0;

        if (parse_color(color, &r, &g, &b)) {
            printf("Error parsing color\n");
            free_png(&image);
            return 40;
        }

        if (fill_flag) {
            if (parse_color(fill_color, &fr, &fg, &fb)) {
                printf("Error parsing fill_color\n");
                free_png(&image);
                return 40;
            }
        }
        draw_rect(&image, x1, y1, x2, y2, thickness, r, g, b, fill_flag, fr, fg, fb);
    }

    else if (rotate_mode) {
        if (!left_up_set || !right_down_set || !angle) {
            printf("Error: Missing necessary arguments\n");
            free_png(&image);
            return 43;
        }
        rotate_png(&image, x1, y1, x2, y2, angle);
    }

    else if (ornament_mode) {
        if (!pattern || !color) {
            printf("Error: Missing necessary arguments\n");
            free_png(&image);
            return 43;
        }

        if ((strcmp(pattern, "rectangle") == 0 || strcmp(pattern, "semicircles") == 0) &&
            (!count || !thickness)) {
            printf("Error: Missing necessary arguments\n");
            free_png(&image);
            return 43;
        }

        int r, g, b;
        if (parse_color(color, &r, &g, &b)) {
            printf("Error parsing color\n");
            free_png(&image);
            return 40;
        }
        draw_ornament(&image, pattern, thickness, count, r, g, b);
    }

    if (write_png_file(output_file, &image)) {
        printf("Error writing PNG\n");
        free_png(&image);
        return 45;
    }

    free_png(&image);
    return 0;
}