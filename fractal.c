#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "fractal.h"

unsigned char MSetPixel(const FRAC_INFO *info, double cx, double cy)
{

    static const int maxIter = 500;
    static const double binBailout = 500;
    static const double huge = 1e120;
    static const double overflow = 1e300;

    double spacing = info->spacing;
    double radius = info->radius;

    double pixVal = 0.0;
        
    double x = 0.0 ,y = 0.0;
    double x2 = 0.0, y2 = 0.0;
    double dist = 0.0;
    double xOrbit = 0.0, yOrbit = 0.0;
    int iter = 0;
    double tmp;
    bool flag = false;
    double xder = 0.0, yder = 0.0;
    double yBailout = 0.0;
    bool binBailed = false;
        
    while(iter < maxIter)
    {
        if(flag)
            break;

        tmp = x2-y2+cx;
        y = 2*x*y+cy;
        x = tmp;
        x2 = x*x;
        y2 = y*y;
        iter++;
            
        tmp = 2*(xOrbit*xder-yOrbit*yder)+1;
        yder = 2*(yOrbit*xder + xOrbit*yder);
        xder = tmp;
        flag = fmax(fabs(xder), fabs(yder)) > overflow;
            
        //If too large of a bailout is used the binary looks bad
        //This should collect the first y after we reach binBailout
        if(x2 + y2 > binBailout && !binBailed) {
            yBailout = y;
            binBailed =  true;
        }
            
        if (x2 + y2 > huge) {
            dist = log(x2+y2)*sqrt(x2+y2)/sqrt(xder*xder+yder*yder);
            break;
        }
            
        xOrbit = x;
        yOrbit = y;
    }
        
    // Distance estimator coloring
    if(dist <= radius)
        pixVal = pow(dist/radius, 1.0/3.0);
    // "Padding" between binary and distance
    else if(iter > 120)
        pixVal = 1.0;
    // Binary black
    else if(yBailout > 0)
        pixVal = 0.0;
    // Binary white
    else
        pixVal = 1.0;

    //Implicit cast to char
    return round(pixVal*255);

}

void calcPixels(const FRAC_INFO *info, WORK_DATA *data)
{
    int nx = info->num_cols;
    int ny = data->num_rows;
    double spacing = info->spacing;
    double x_start = info->x_start;
    double y_start = info->y_start + (data->start_row*spacing);
    int ix,iy;
    double cx, cy;

    for(iy=0; iy<ny; iy++) {
        cy = y_start + iy*spacing;
        for(ix=0; ix<nx; ix++) {
            cx = x_start + ix*spacing;
            data->pixels[iy*nx+ix] = MSetPixel(info, cx, cy);
        }
    }
}

void init_fractal(FRAC_INFO *info)
{
    //Dimensions of grid
    const double xMin = -.745429-0.000030;
    const double xMax = xMin + 0.000060;
    const double yMin = 0.113008-0.000030;
    const double yMax = yMin + 0.000060;

    //number of pixels in x and y
    const int nx = 600;
    const int ny = ceil(nx*(yMax-yMin)/(xMax-xMin));
    const double spacing = (xMax-xMin)/(nx-1);

    printf("nx: %d, ny: %d\n",nx,ny);
    const double threshold = 1.0;

    // Set frac info struct
    info->num_cols = nx;
    info->num_rows = ny;
    info->x_start = xMin;
    info->y_start = yMin;
    info->radius = threshold * 0.5*spacing;
    info->spacing = spacing;

}
