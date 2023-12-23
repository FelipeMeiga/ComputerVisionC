#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BITMAP_MAGIC_NUMBER 19778

typedef struct {
    int32_t size;
    int32_t app_id;
    int32_t offset;
} bmp_file_header_t;

typedef struct {
    int32_t header_size;
    int32_t width;
    int32_t height;
    int16_t num_planes;
    int16_t bpp;
    int32_t compression;
    int32_t image_size;
    int32_t horizontal_resolution;
    int32_t vertical_resolution;
    int32_t colors_used;
    int32_t colors_important;
} bmp_bitmap_info_header_t;

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;
} bmp_palette_element_t;

int loadBMP(const char* filename, unsigned cha** data, int* width, int* height);
int saveBMP(const char* filename, const unsigned char* data, int width, int height);
void applyEdgeDetection(unsigned char* data, int width, int height);
void applyGaussianBlur(unsigned char* data, int width, int height);
