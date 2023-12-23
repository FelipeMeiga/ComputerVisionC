#include <stdio.h>
#include <stdlib.h>
#include "compvision.h"

int main() {
    const char* input_filename = "monkey.bmp";
    const char* output_filename = "output.bmp";

    unsigned char* image_data;
    int width, height;

    if (loadBMP(input_filename, &image_data, &width, &height)) {
        printf("Error while loading image '%s'\n", input_filename);
        return 1;
    }
    printf("Image '%s' loaded successfully! Dimensions: %dx%d\n", input_filename, width, height);

    applyEdgeDetection(image_data, width, height);
    printf("Sobel filter applied to image '%s'!\n", input_filename);

    //applyGaussianBlur(image_data, width, height);
    //printf("Filtro gaussiano aplicado a imagem '%s'!\n", input_filename);

    if (saveBMP(output_filename, image_data, width, height)) {
        printf("Error while saving image '%s'\n", output_filename);
        free(image_data);
        return 1;
    }
    printf("Image saved as '%s'\n", output_filename);

    free(image_data);

    return 0;
}
