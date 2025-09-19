#include "image.h"
#include <stdlib.h>
#include <math.h>

void applyBlur(Image *img) {
    // Simple 3x3 average filter
    int kernel[3][3] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };
    int sum = 9;

    Image *copy = resizeImage(img, img->width, img->height);

    for (int y = 1; y < img->height-1; y++) {
        for (int x = 1; x < img->width-1; x++) {
            int r=0, g=0, b=0;
            for (int ky=-1; ky<=1; ky++) {
                for (int kx=-1; kx<=1; kx++) {
                    Pixel p = copy->data[y+ky][x+kx];
                    r += p.red * kernel[ky+1][kx+1];
                    g += p.green * kernel[ky+1][kx+1];
                    b += p.blue * kernel[ky+1][kx+1];
                }
            }
            img->data[y][x].red   = r/sum;
            img->data[y][x].green = g/sum;
            img->data[y][x].blue  = b/sum;
        }
    }
    freeImage(copy);
}

void applySharpen(Image *img) {
    // Basic sharpening kernel
    int kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };

    Image *copy = resizeImage(img, img->width, img->height);

    for (int y = 1; y < img->height-1; y++) {
        for (int x = 1; x < img->width-1; x++) {
            int r=0, g=0, b=0;
            for (int ky=-1; ky<=1; ky++) {
                for (int kx=-1; kx<=1; kx++) {
                    Pixel p = copy->data[y+ky][x+kx];
                    r += p.red   * kernel[ky+1][kx+1];
                    g += p.green * kernel[ky+1][kx+1];
                    b += p.blue  * kernel[ky+1][kx+1];
                }
            }
            img->data[y][x].red   = (r<0)?0:(r>255?255:r);
            img->data[y][x].green = (g<0)?0:(g>255?255:g);
            img->data[y][x].blue  = (b<0)?0:(b>255?255:b);
        }
    }
    freeImage(copy);
}

void applyEdgeDetection(Image *img) {
    // Sobel edge detection
    int gx[3][3] = {
        {-1,0,1},
        {-2,0,2},
        {-1,0,1}
    };
    int gy[3][3] = {
        {-1,-2,-1},
        {0,0,0},
        {1,2,1}
    };

    Image *copy = resizeImage(img, img->width, img->height);

    for (int y = 1; y < img->height-1; y++) {
        for (int x = 1; x < img->width-1; x++) {
            int rx=0, ry=0, gxv=0, gyv=0, bx=0, by=0;
            for (int ky=-1; ky<=1; ky++) {
                for (int kx=-1; kx<=1; kx++) {
                    Pixel p = copy->data[y+ky][x+kx];
                    rx += p.red   * gx[ky+1][kx+1];
                    ry += p.red   * gy[ky+1][kx+1];
                    gxv += p.green* gx[ky+1][kx+1];
                    gyv += p.green* gy[ky+1][kx+1];
                    bx += p.blue  * gx[ky+1][kx+1];
                    by += p.blue  * gy[ky+1][kx+1];
                }
            }
            int r = abs(rx)+abs(ry);
            int g = abs(gxv)+abs(gyv);
            int b = abs(bx)+abs(by);
            img->data[y][x].red   = (r>255)?255:r;
            img->data[y][x].green = (g>255)?255:g;
            img->data[y][x].blue  = (b>255)?255:b;
        }
    }
    freeImage(copy);
}

