#include <stdio.h>
#include <stdlib.h>
#include "image.h"

int main() {
    char inputPath[256], outputPath[256];

    printf("=== Image Manipulation Tool ===\n");
    printf("Enter input image path (jpg/png/bmp): ");
    scanf("%255s", inputPath);

    Image *img = loadImage(inputPath);
    if (!img) {
        printf("❌ Failed to load image: %s\n", inputPath);
        return 1;
    }

    int choice;
    do {
        printf("\nChoose an operation:\n");
        printf("1. Grayscale\n");
        printf("2. Invert Colors\n");
        printf("3. Brightness (+50)\n");
        printf("4. Contrast (1.2x)\n");
        printf("5. Rotate 90°\n");
        printf("6. Flip Horizontal\n");
        printf("7. Blur\n");
        printf("8. Sharpen\n");
        printf("9. Edge Detection\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: applyGrayscale(img); break;
            case 2: invertColors(img); break;
            case 3: adjustBrightness(img, 50); break;
            case 4: adjustContrast(img, 1.2); break;
            case 5: img = rotateImage(img, 90); break;
            case 6: flipImage(img, 0); break;
            case 7: applyBlur(img); break;
            case 8: applySharpen(img); break;
            case 9: applyEdgeDetection(img); break;
            case 0: break;
            default: printf("⚠️ Invalid choice!\n");
        }

        if (choice != 0) {
            printf("Enter output image path (jpg/png/bmp): ");
            scanf("%255s", outputPath);
            saveImage(outputPath, img);
            printf("✅ Saved as %s\n", outputPath);
        }

    } while(choice != 0);

    freeImage(img);
    printf("👋 Exiting...\n");
    return 0;
}

