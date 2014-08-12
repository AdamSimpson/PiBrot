/*
The MIT License (MIT)

Copyright (c) 2014 Adam Simpson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

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
void calcColorPixels(FRAC_INFO *info, WORK_DATA *data);
void calcPixels(FRAC_INFO *info, WORK_DATA *data);
void init_fractal(FRAC_INFO *info, bool color, int pixel_width);

#endif
