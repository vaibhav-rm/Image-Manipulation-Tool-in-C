#include "image.h"
#include <math.h>

void applyGrayscale(Image *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            uint8_t gray = (p->red + p->green + p->blue) / 3;
            p->red = p->green = p->blue = gray;
        }
    }
}

void invertColors(Image *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            p->red   = 255 - p->red;
            p->green = 255 - p->green;
            p->blue  = 255 - p->blue;
        }
    }
}

void adjustBrightness(Image *img, int value) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            p->red   = fmin(255, fmax(0, p->red + value));
            p->green = fmin(255, fmax(0, p->green + value));
            p->blue  = fmin(255, fmax(0, p->blue + value));
        }
    }
}

void adjustContrast(Image *img, float factor) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            p->red   = fmin(255, fmax(0, (p->red - 128) * factor + 128));
            p->green = fmin(255, fmax(0, (p->green - 128) * factor + 128));
            p->blue  = fmin(255, fmax(0, (p->blue - 128) * factor + 128));
        }
    }
}

