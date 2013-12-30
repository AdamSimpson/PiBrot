#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "fractal.h"

#define USE_COLOR 1

// Full color Mu-Ency implimentation, distance est./binary decomp./cont. dwell
void MSetColorPixels(FRAC_INFO *info, unsigned char* pixels,  double cx, double cy)
{
    static const int maxIter = 10000;
    static const double binBailout = 500;
    static const double escape_radius = 1000;//1e120;

    double spacing = info->spacing;
    double radius = info->radius;

    double pixVal = 0.0;

    double x = 0.0 ,y = 0.0;
    double x2 = 0.0, y2 = 0.0;
    double xOrbit = 0.0, yOrbit = 0.0;
    int iter = 0;
    double tmp;
    double xder = 0.0, yder = 0.0;
    bool binBailed = false;
    double magnitude = 0.0;
    double magnitude2 = 0.0;

    double distance = 0.0;
    double y_bailout = 0.0;
    double cont_dwell_fractional = 0.0;

    while(iter < maxIter)
    {
        tmp = x2-y2+cx;
        y = 2*x*y+cy;
        x = tmp;
        x2 = x*x;
        y2 = y*y;
        iter++;

        tmp = 2*(xOrbit*xder-yOrbit*yder)+1;
        yder = 2*(yOrbit*xder + xOrbit*yder);
        xder = tmp;

        magnitude2 = x2 + y2;
        magnitude =  sqrt(x2 + y2);

        //If too large of a bailout is used the binary looks bad
        //This should collect the first y after we reach binBailout
/*        if(magnitude2 > binBailout && !binBailed) {
            y_bailout = y;
            binBailed =  true;
        }
*/

//        if(iter < 10)
//	    y_bailout = y;

        // If the point has escaped collect the distance and continuous dwell
        if (magnitude2 > escape_radius) {
            distance = log(magnitude2)*magnitude/sqrt(xder*xder+yder*yder);
	    cont_dwell_fractional = log2(log2(magnitude)) - log2(log2(escape_radius)); // MuEncy
//	    cont_dwell_fractional = -1.0 * log(log(magnitude)/log(escape_radius)); // Wikipedia continuous coloring
	    y_bailout = y;
            break;
        }

        xOrbit = x;
        yOrbit = y;
    }

    // If point is in mandelbrot set set to white
    if(iter >= maxIter) {
        pixels[0] = 255;
        pixels[1] = 255;
        pixels[2] = 255;
        return;
    }

    // Set HSV pixels
    double hue, saturation, value;
    double angle, rad;
    int dwell;
    double final_rad;
    double d_scale;
    double P;

    dwell = iter;
    final_rad = cont_dwell_fractional; 
    d_scale = log2(distance / spacing);

    // Calculate brightness(Value)
    if (d_scale > 0)
	value = 1.0;
    else if (d_scale > -8.0)
	value = (8.0+d_scale)/8.0;
    else
	value = 0.0;

    P = log(dwell)/log(maxIter); // Originally hard coded 100000 ?

    if(P < 0.5) {
	P = 1.0 - 1.5*P;
	angle = 1.0 - P;
	rad = sqrt(P);
    }
    else {
        P = 1.5*P - 0.5;
	angle = P;
	rad = sqrt(P);
    }

    if(dwell%2) {
        value *= 0.90;
	rad *= 0.667;
    }

    if(y_bailout < 0.0)
	angle = angle + 0.02;

    angle = angle + (0.0001 * final_rad);

    hue = angle * 10.0;
    hue = hue - floor(hue);
    saturation = rad - floor(rad);

    // convert HSV to RGB

    // convert hue to degrees
    hue = hue*360.0;

    if(saturation <= 0.0) {
        pixels[0] = (unsigned char)(value * 255.0);
	pixels[1] = (unsigned char)(value * 255.0);
	pixels[2] = (unsigned char)(value * 255.0);
        return;
    }
    double hh, p, q, t, ff, r, g, b;
    long i;

    hh = hue;
    if(hh >= 360.0)
	hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = value * (1.0 - saturation);
    q = value * (1.0 - (saturation * ff));
    t = value * (1.0 - (saturation * (1.0 - ff)));

    switch(i) {
        case 0:
	    r = value;
	    g = t;
	    b = p;
	    break;
	case 1:
	    r = q;
	    g = value;
	    b = p;
	    break;
	case 2:
	    r = p;
	    g = value;
	    b = t;
	    break;
	case 3:
	    r = p;
	    g = q;
	    b = value;
	    break;
        case 4:
	    r = t;
	    g = p;
	    b = value;
	case 5:
	default:
	    r = value;
	    g = p;
	    b = q;
	    break;
    }

    unsigned char rc,gc,bc;

    rc = (unsigned char)(r * 255.0);
    gc = (unsigned char)(g * 255.0);
    bc = (unsigned char)(b * 255.0);

    pixels[0] = rc;
    pixels[1] = gc;
    pixels[2] = bc;
}

// Greyscale hybrid distance estimator/binary decomposition
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

// Calculate greyscale byte pixels
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

// calculate floating point RGB pixels
void calcColorPixels(const FRAC_INFO *info, WORK_DATA *data)
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
            MSetColorPixels(info, &data->pixels[3*(iy*nx+ix)],  cx, cy);
        }
    }
}

void init_fractal(FRAC_INFO *info, int pixel_width)
{
    //Dimensions of grid
    double xMin = -.745429-0.000005;
    double xMax = xMin + 0.000010;
    double yMin = 0.113008-0.000005;
    double yMax = yMin + 0.000010;

    #if USE_COLOR
    info->channels = 3;
    #else
    info->channels = 1;
    #endif

    //number of pixels in x and y
    // MAX texture size is 2048 on the Pi version B
    int nx = pixel_width;
    int ny = ceil(nx*(yMax-yMin)/(xMax-xMin));
    double spacing = (xMax-xMin)/(nx-1);

    printf("nx: %d, ny: %d\n",nx,ny);
    double threshold = 1.0;

    // Set frac info struct
    info->num_cols = nx;
    info->num_rows = ny;
    info->x_start = xMin;
    info->y_start = yMin;
    info->radius = threshold * 0.5*spacing;
    info->spacing = spacing;
    info->rows_taken = 0;
}
