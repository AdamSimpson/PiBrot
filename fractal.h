#ifndef MANDEL_FRAC_H
#define MANDEL_FRAC_H

typedef struct
{
    int start_row;
    int num_rows;
    unsigned char *pixels;
} WORK_DATA;

typedef struct
{
    int num_cols;
    int num_rows;
    double x_start;
    double y_start;
    double radius;
    double spacing;
} FRAC_INFO;


unsigned char MSetPixel(const FRAC_INFO *info, double cx, double cy);
void calcPixels(const FRAC_INFO *info, WORK_DATA *data);
void init_fractal(FRAC_INFO *info);

#endif
