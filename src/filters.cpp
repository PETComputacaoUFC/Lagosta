#include "filters.hpp"

// Our image comes in UNCOMPRESSED GRAYSCALE format (8bit per pixel),
// so I'll be assuming that all inputs to this function are of that type for now.
void ImageDilate(Image* image, int kernelSize) {
    size_t img_memsize = sizeof(unsigned char) * image->width * image->height;
    unsigned char* image_data = (unsigned char*) image->data;
    unsigned char* image_copy = (unsigned char*) malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    int kernelRadius = kernelSize / 2;

    for (int x = kernelRadius; x < image->width - kernelRadius; x++) {
        for (int y = kernelRadius; y < image->height; y++) {
            // offset = x pixels + y * width pixels
            int copy_offset = sizeof(unsigned char) * x + sizeof(unsigned char) * image->width * y;

            for (int i = -kernelRadius; i <= kernelRadius; i++) {
                for (int j = -kernelRadius; j <= kernelRadius; j++) {
                    int data_offset = sizeof(unsigned char) * (x + i)
                                    + sizeof(unsigned char) * image->width * (y - j);
                    if (image_data[data_offset] == 255) {
                        image_copy[copy_offset] = 255;
                        goto next_pixel;
                    }
                }
            }
            
            next_pixel:
        }
    }

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

void ImageErode(Image *image, int kernelSize) {
    size_t img_memsize = sizeof(unsigned char) * image->width * image->height;
    unsigned char* image_data = (unsigned char*) image->data;
    unsigned char* image_copy = (unsigned char*) malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    int kernelRadius = kernelSize / 2;

    for (int x = kernelRadius; x < image->width - kernelRadius; x++) {
        for (int y = kernelRadius; y < image->height; y++) {
            // offset = x pixels + y * width pixels
            int copy_offset = sizeof(unsigned char) * x + sizeof(unsigned char) * image->width * y;

            for (int i = -kernelRadius; i <= kernelRadius; i++) {
                for (int j = -kernelRadius; j <= kernelRadius; j++) {
                    int data_offset = sizeof(unsigned char) * (x + i)
                                    + sizeof(unsigned char) * image->width * (y - j);
                    if (image_data[data_offset] == 0) {
                        image_copy[copy_offset] = 0;
                        goto next_pixel;
                    }
                }
            }
            
            next_pixel:
        }
    }

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

void ImageThreshold(Image *image, unsigned char threshold) {
    unsigned char* image_data = (unsigned char*) image->data;
    for (int t = 0; t < image->height * image->width; t++) {
        int offset = sizeof(unsigned char) * t;
        
        unsigned char pixel = image_data[offset];
        
        if (pixel < threshold) { image_data[offset] = 0; }
        else { image_data[offset] = 255; }
    }
}

void ImageInvert(Image *image) {
    unsigned char* image_data = (unsigned char*) image->data;
    for (int t = 0; t < image->height * image->width; t++) {
        int offset = sizeof(unsigned char) * t;
        image_data[offset] = 255 - image_data[offset];
    }
}