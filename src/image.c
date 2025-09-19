#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "image.h"
#include <jpeglib.h>
#include <png.h>

// Detect extension
static const char* getExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot ? dot + 1 : "";
}

// ---------------- JPG / JPEG ----------------
Image* loadJPEG(const char *filename) {
    FILE *infile = fopen(filename, "rb");
    if (!infile) return NULL;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int channels = cinfo.output_components; // should be 3 (RGB)

    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++)
        img->data[i] = malloc(width * sizeof(Pixel));

    unsigned char *buffer = malloc(width * channels);

    while (cinfo.output_scanline < cinfo.output_height) {
        int row = cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        for (int col = 0; col < width; col++) {
            img->data[row][col].red   = buffer[col*3];
            img->data[row][col].green = buffer[col*3 + 1];
            img->data[row][col].blue  = buffer[col*3 + 2];
        }
    }

    free(buffer);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return img;
}

// Save JPEG image
void saveJPEG(const char *filename, Image *img) {
    FILE *outfile = fopen(filename, "wb");
    if (!outfile) {
        printf("Failed to open %s for writing\n", filename);
        return;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = img->width;
    cinfo.image_height = img->height;
    cinfo.input_components = 3; // RGB
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    unsigned char *buffer = malloc(img->width * 3);
    JSAMPROW row_pointer[1];

    while (cinfo.next_scanline < cinfo.image_height) {
        int row = cinfo.next_scanline;
        for (int col = 0; col < img->width; col++) {
            buffer[col*3]     = img->data[row][col].red;
            buffer[col*3 + 1] = img->data[row][col].green;
            buffer[col*3 + 2] = img->data[row][col].blue;
        }
        row_pointer[0] = buffer;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    free(buffer);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

int savePNG(const char *filename, Image *img) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) return 0;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) return 0;

    png_infop info = png_create_info_struct(png);
    if (!info) return 0;

    if (setjmp(png_jmpbuf(png))) return 0;

    png_init_io(png, fp);

    // Set header
    png_set_IHDR(
        png, info, img->width, img->height,
        8, (img->channels == 4 ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB),
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    // Allocate row buffer
    unsigned char *row = malloc(img->width * img->channels);
    
    // Write image rows
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            row[x*3]     = img->data[y][x].red;
            row[x*3 + 1] = img->data[y][x].green;
            row[x*3 + 2] = img->data[y][x].blue;
        }
        png_write_row(png, row);
    }
    
    free(row);

    png_write_end(png, NULL);

    fclose(fp);
    png_destroy_write_struct(&png, &info);

    return 1; // success
}


// ---------------- PNG ----------------
Image* loadPNG(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    setjmp(png_jmpbuf(png));

    png_init_io(png, fp);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    int color_type = png_get_color_type(png, info);
    int bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_strip_alpha(png);

    png_read_update_info(png, info);

    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->channels = 3; // RGB
    img->data = malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++)
        img->data[i] = malloc(width * sizeof(Pixel));

    png_bytep row = malloc(png_get_rowbytes(png, info));

    for (int y = 0; y < height; y++) {
        png_read_row(png, row, NULL);
        for (int x = 0; x < width; x++) {
            img->data[y][x].red   = row[x*3];
            img->data[y][x].green = row[x*3 + 1];
            img->data[y][x].blue  = row[x*3 + 2];
        }
    }

    free(row);
    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);
    return img;
}

// TODO: savePNG() (similar logic)


// ---------------- Dispatcher ----------------
Image* loadImage(const char *filename) {
    const char *ext = getExt(filename);
    if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0)
        return loadJPEG(filename);
    else if (strcasecmp(ext, "png") == 0)
        return loadPNG(filename);
    else {
        printf("Unsupported format: %s\n", ext);
        return NULL;
    }
}

void saveImage(const char *filename, Image *img) {
    const char *ext = getExt(filename);
    if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0)
        saveJPEG(filename, img);
    else if (strcasecmp(ext, "png") == 0)
        savePNG(filename, img);
    else
        printf("Unsupported format: %s\n", ext);
}

void freeImage(Image *img) {
    if (img) {
        if (img->data) {
            for (int i = 0; i < img->height; i++) {
                free(img->data[i]);
            }
            free(img->data);
        }
        free(img);
    }
}

