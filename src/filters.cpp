#include "filters.hpp"
#include <cmath>
#include <functional>
#include <thread>
#include <vector>

const int MAX_THREADS = std::thread::hardware_concurrency();

void do_threads(std::function<void(int,int)> lambda, int elements) {
    int thread_elements = elements / MAX_THREADS;
    int start_element = 0;
    int end_element = thread_elements;
    std::vector<std::thread> threads;
    for (int i = 0; i < MAX_THREADS; i++) {
        if (i == MAX_THREADS - 1) { end_element = elements; }
        threads.push_back(std::thread(lambda, start_element, end_element));
        start_element = end_element;
        end_element += thread_elements;
    }

    for (size_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

// Our image comes in UNCOMPRESSED GRAYSCALE format (8bit per pixel),
// so I'll be assuming that all inputs to this function are of that type for now.
void ImageDilate(Image* image, int kernel_size) {
    size_t img_memsize = sizeof(unsigned char) * image->width * image->height;
    unsigned char* image_data = (unsigned char*) image->data;
    unsigned char* image_copy = (unsigned char*) malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    int kernel_radius = kernel_size / 2;
    
    auto lambda = [image, image_data, image_copy, img_memsize, kernel_radius](int start_row, int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < image->width; x++) {
               // offset = x pixels + y * width pixels
               int copy_offset = sizeof(unsigned char) * x + sizeof(unsigned char) * image->width * y;
   
               for (int i = -kernel_radius; i <= kernel_radius; i++) {
                   for (int j = -kernel_radius; j <= kernel_radius; j++) {
                       int data_offset = sizeof(unsigned char) * (x + i)
                                       + sizeof(unsigned char) * image->width * (y - j);
                       
                       if (data_offset > (int) img_memsize || data_offset < 0) { continue; }
                       if (image_data[data_offset] == 255) {
                           image_copy[copy_offset] = 255;
                           goto next_pixel;
                       }
                   }
               }
               
               next_pixel:
           }
       }
    };

    int rows = image->height;
    do_threads(lambda, rows);

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

void ImageErode(Image *image, int kernel_size) {
    size_t img_memsize = sizeof(unsigned char) * image->width * image->height;
    unsigned char* image_data = (unsigned char*) image->data;
    unsigned char* image_copy = (unsigned char*) malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    int kernel_radius = kernel_size / 2;
    
    auto lambda = [image, image_data, image_copy, img_memsize, kernel_radius](int start_row, int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < image->width; x++) {
               // offset = x pixels + y * width pixels
               int copy_offset = sizeof(unsigned char) * x + sizeof(unsigned char) * image->width * y;
   
               for (int i = -kernel_radius; i <= kernel_radius; i++) {
                   for (int j = -kernel_radius; j <= kernel_radius; j++) {
                       int data_offset = sizeof(unsigned char) * (x + i)
                                       + sizeof(unsigned char) * image->width * (y - j);
                       
                       if (data_offset > (int) img_memsize || data_offset < 0) { continue; }
                       if (image_data[data_offset] == 0) {
                           image_copy[copy_offset] = 0;
                           goto next_pixel;
                       }
                   }
               }
               
               next_pixel:
           }
       }
    };

    int rows = image->height;
    do_threads(lambda, rows);

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

void ImageThreshold(Image *image, unsigned char threshold) {
    unsigned char* image_data = (unsigned char*) image->data;
    
    auto lambda = [image_data, threshold](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = sizeof(unsigned char) * t;
            
            unsigned char pixel = image_data[offset];
            
            if (pixel < threshold) { image_data[offset] = 0; }
            else { image_data[offset] = 255; }
        }
    };
    
    int pixels = image->height * image->width;
    do_threads(lambda, pixels);
}

void ImageInvert(Image *image) {
    unsigned char* image_data = (unsigned char*) image->data;
    
    auto lambda = [image_data](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = sizeof(unsigned char) * t;
            image_data[offset] = 255 - image_data[offset];
        }
    };
    
    int pixels = image->height * image->width;
    do_threads(lambda, pixels);
}

void ImagePow(Image *image, float expo) {
    unsigned char* image_data = (unsigned char*) image->data;
    
    auto lambda = [image_data, expo](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = sizeof(unsigned char) * t;
            float pixel = ((float) image_data[offset]) / 255.0f;
            pixel = pow(pixel, expo);
            image_data[offset] = (unsigned char) (pixel * 255.0f);
        }
    };
    
    int pixels = image->height * image->width;
    do_threads(lambda, pixels);
}