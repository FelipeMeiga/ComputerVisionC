#include "compvision.h"

int loadBMP(const char* filename, unsigned char** data, int* width, int* height) {
    FILE* file;
    int16_t magic_number;
    bmp_file_header_t file_header;
    bmp_bitmap_info_header_t info_header;
    
    file = fopen(filename, "rb");
    if (!file) {
        printf("Error: could not open file %s\n", filename);
        return 1;
    }
    
    fread(&magic_number, 2, 1, file);
    if (magic_number != BITMAP_MAGIC_NUMBER) {
        printf("%s is not a valid BMP file: magic number is %d whereas it should be %d\n", filename, magic_number, BITMAP_MAGIC_NUMBER);
        fclose(file);
        return 1;
    }
    
    // Read headers
    fread(&file_header, sizeof(file_header), 1, file);
    fread(&info_header, sizeof(info_header), 1, file);
    
    // Allocate memory for the image data
    *data = (unsigned char*)malloc(info_header.width * info_header.height * 4); // 4 bytes per pixel for RGBA
    if (!(*data)) {
        printf("Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    // Read pixel data
    fseek(file, file_header.offset, SEEK_SET);
    fread(*data, 4, info_header.width * info_header.height, file); // Assuming the BMP is 32 bits per pixel

    *width = info_header.width;
    *height = info_header.height;

    fclose(file);
    return 0;
}

int saveBMP(const char* filename, const unsigned char* data, int width, int height) {
    FILE* file;
    int16_t magic_number = BITMAP_MAGIC_NUMBER;
    bmp_file_header_t file_header;
    bmp_bitmap_info_header_t info_header;
    int row_padded, i, j;

    file = fopen(filename, "wb");
    if (!file) {
        printf("Error: could not create file %s\n", filename);
        return 1;
    }

    row_padded = (width * 3 + 3) & (~3); // Rows are padded to the nearest multiple of 4 bytes
    int image_size = row_padded * height;

    // Setup file header
    file_header.size = 54 + image_size;
    file_header.app_id = 0;
    file_header.offset = 54;

    // Setup info header
    info_header.header_size = 40;
    info_header.width = width;
    info_header.height = height;
    info_header.num_planes = 1;
    info_header.bpp = 24; // 24 bits per pixel for BMP
    info_header.compression = 0;
    info_header.image_size = image_size;
    info_header.horizontal_resolution = 2835; // example value (pixels per meter)
    info_header.vertical_resolution = 2835; // example value (pixels per meter)
    info_header.colors_used = 0;
    info_header.colors_important = 0;

    // Write headers
    fwrite(&magic_number, sizeof(magic_number), 1, file);
    fwrite(&file_header, sizeof(file_header), 1, file);
    fwrite(&info_header, sizeof(info_header), 1, file);

    // Write image data
    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (width * 3) % 4) % 4;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // Write pixel in BGR format
            unsigned char pixel[3];
            pixel[0] = data[(i * width + j) * 4 + 0]; // Blue
            pixel[1] = data[(i * width + j) * 4 + 1]; // Green
            pixel[2] = data[(i * width + j) * 4 + 2]; // Red
            fwrite(pixel, 3, 1, file);
        }
        fwrite(padding, 1, paddingSize, file); // Write padding for each row
    }

    fclose(file);
    return 0;
}

void applyEdgeDetection(unsigned char* data, int width, int height) {
    int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    unsigned char* tempData = (unsigned char*)malloc(width * height * 4);
    memcpy(tempData, data, width * height * 4);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float gx = 0.0;
            float gy = 0.0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int iy = y + ky;
                    int ix = x + kx;

                    // Edges: extension
                    iy = (iy < 0) ? 0 : (iy >= height) ? height - 1 : iy;
                    ix = (ix < 0) ? 0 : (ix >= width) ? width - 1 : ix;

                    int pixelPos = (iy * width + ix) * 4;
                    float gray = (tempData[pixelPos] + tempData[pixelPos + 1] + tempData[pixelPos + 2]) / 3.0;

                    gx += sobelX[ky + 1][kx + 1] * gray;
                    gy += sobelY[ky + 1][kx + 1] * gray;
                }
            }

            float magnitude = sqrt(gx * gx + gy * gy);
            magnitude = (magnitude > 255) ? 255 : magnitude;

            int pos = (y * width + x) * 4;
            for (int i = 0; i < 3; i++) {
                data[pos + i] = (unsigned char)magnitude;
            }
        }
    }

    free(tempData);
}

void applyGaussianBlur(unsigned char* data, int width, int height) {
    // Kernel for convolution
    float kernel[5][5] = {
        {1.0 / 256, 4.0 / 256, 6.0 / 256, 4.0 / 256, 1.0 / 256},
        {4.0 / 256, 16.0 / 256, 24.0 / 256, 16.0 / 256, 4.0 / 256},
        {6.0 / 256, 24.0 / 256, 36.0 / 256, 24.0 / 256, 6.0 / 256},
        {4.0 / 256, 16.0 / 256, 24.0 / 256, 16.0 / 256, 4.0 / 256},
        {1.0 / 256, 4.0 / 256, 6.0 / 256, 4.0 / 256, 1.0 / 256}
    };

    // Image buffer
    unsigned char* tempData = (unsigned char*)malloc(width * height * 4);
    memcpy(tempData, data, width * height * 4);

    // Convolution
    for (int y = 2; y < height - 2; y++) {
        for (int x = 2; x < width - 2; x++) {
            float sum[4] = {0.0, 0.0, 0.0, 0.0};
            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int pixelPos = ((y + ky) * width + (x + kx)) * 4;
                    for (int i = 0; i < 4; i++) {
                        sum[i] += kernel[ky + 2][kx + 2] * tempData[pixelPos + i];
                    }
                }
            }
            int pos = (y * width + x) * 4;
            for (int i = 0; i < 4; i++) {
                data[pos + i] = (unsigned char)(sum[i]);
            }
        }
    }

    free(tempData);
}
