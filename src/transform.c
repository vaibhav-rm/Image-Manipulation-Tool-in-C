#include "image.h"
#include <stdlib.h>

Image* rotateImage(Image *img, int angle) {
    if (angle != 90) return img; // only 90° for now

    Image *rot = malloc(sizeof(Image));
    rot->width = img->height;
    rot->height = img->width;

    rot->data = malloc(rot->height * sizeof(Pixel*));
    for (int i = 0; i < rot->height; i++)
        rot->data[i] = malloc(rot->width * sizeof(Pixel));

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            rot->data[j][rot->width - i - 1] = img->data[i][j];
        }
    }

    return rot;
}

void flipImage(Image *img, int mode) {
    if (mode == 0) { // horizontal
        for (int i = 0; i < img->height; i++) {
            for (int j = 0; j < img->width / 2; j++) {
                Pixel temp = img->data[i][j];
                img->data[i][j] = img->data[i][img->width - j - 1];
                img->data[i][img->width - j - 1] = temp;
            }
        }
    }
}

Image* resizeImage(Image *img, int newWidth, int newHeight) {
    Image *resized = malloc(sizeof(Image));
    resized->width = newWidth;
    resized->height = newHeight;

    resized->data = malloc(newHeight * sizeof(Pixel*));
    for (int i = 0; i < newHeight; i++)
        resized->data[i] = malloc(newWidth * sizeof(Pixel));

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            int srcX = x * img->width / newWidth;
            int srcY = y * img->height / newHeight;
            resized->data[y][x] = img->data[srcY][srcX];
        }
    }

    return resized;
}

