#ifndef MANDEL_FRAC_H
#define MANDEL_FRAC_H

#include <stdbool.h>

typedef struct
{
    int rank; // MPI rank
    int start_row;
    int num_rows;
    unsigned char *pixels;
} WORK_DATA;

typedef struct
{
    int num_cols;
    int num_rows;
    int rows_taken; // rows taken so far for computation
    double x_start;
    double y_start;
    double radius;
    double spacing;
    int channels; // Number of color channels per pixel
    bool color;
} FRAC_INFO;

void MSetColorPixels(FRAC_INFO *info, unsigned char* pixels,  double cx, double cy);
unsigned char MSetPixel(const FRAC_INFO *info, double cx, double cy);
void calcColorPixels(const FRAC_INFO *info, WORK_DATA *data);
void calcPixels(const FRAC_INFO *info, WORK_DATA *data);
void init_fractal(FRAC_INFO *info, bool color, int pixel_width);

#endif
