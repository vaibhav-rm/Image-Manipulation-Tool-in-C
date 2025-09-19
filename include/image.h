#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

// Pixel structure for RGB color
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;

// Image structure using 2D array of Pixels
typedef struct {
    Pixel **data; // 2D array of pixels
    int width;
    int height;
    int channels; // 3 = RGB, 4 = RGBA
} Image;

// Load & Save
Image* loadImage(const char *filename);
void saveImage(const char *filename, Image *img);
void freeImage(Image *img);

// Basic operations
void applyGrayscale(Image *img);
void invertColors(Image *img);
void adjustBrightness(Image *img, int value);
void adjustContrast(Image *img, float factor);

// Transformations
Image* rotateImage(Image *img, int angle);
void flipImage(Image *img, int mode);
Image* resizeImage(Image *img, int newWidth, int newHeight);

// Filters
void applyBlur(Image *img);
void applySharpen(Image *img);
void applyEdgeDetection(Image *img);

#endif

