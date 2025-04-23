#include "filters.hpp"

#include <cmath>
#include <cstring>
#include <functional>
#include <thread>

#include "imgtools.hpp"

const int MAX_THREADS = std::thread::hardware_concurrency();

void do_threads(std::function<void(int, int)> lambda, int elements) {
    int thread_elements = elements / MAX_THREADS;
    int start_element = 0;
    int end_element = thread_elements;
    std::vector<std::thread> threads;
    for (int i = 0; i < MAX_THREADS; i++) {
        if (i == MAX_THREADS - 1) {
            end_element = elements;
        }
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
void ImageDilate(Image *image, int kernel_size) {
    size_t img_memsize = sizeof(uint8_t) * image->width * image->height;
    uint8_t *image_data = (uint8_t *)image->data;
    uint8_t *image_copy = (uint8_t *)malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    int kernel_radius = kernel_size / 2;

    auto lambda = [image, image_data, image_copy, img_memsize, kernel_radius](int start_row,
                                                                              int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < image->width; x++) {
                // offset = x pixels + y * width pixels
                int copy_offset = sizeof(uint8_t) * x + sizeof(uint8_t) * image->width * y;

                for (int i = -kernel_radius; i <= kernel_radius; i++) {
                    for (int j = -kernel_radius; j <= kernel_radius; j++) {
                        int data_offset =
                            sizeof(uint8_t) * (x + i) + sizeof(uint8_t) * image->width * (y - j);

                        if (data_offset > (int)img_memsize || data_offset < 0) {
                            continue;
                        }
                        if (image_data[data_offset] == 255) {
                            image_copy[copy_offset] = 255;
                            goto next_pixel;
                        }
                    }
                }

            next_pixel:;
            }
        }
    };

    int rows = image->height;
    do_threads(lambda, rows);

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

void ImageErode(Image *image, int kernel_size) {
    size_t img_memsize = sizeof(uint8_t) * image->width * image->height;
    uint8_t *image_data = (uint8_t *)image->data;
    uint8_t *image_copy = (uint8_t *)malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    int kernel_radius = kernel_size / 2;

    auto lambda = [image, image_data, image_copy, img_memsize, kernel_radius](int start_row,
                                                                              int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < image->width; x++) {
                // offset = x pixels + y * width pixels
                int copy_offset = sizeof(uint8_t) * x + sizeof(uint8_t) * image->width * y;

                for (int i = -kernel_radius; i <= kernel_radius; i++) {
                    for (int j = -kernel_radius; j <= kernel_radius; j++) {
                        int data_offset =
                            sizeof(uint8_t) * (x + i) + sizeof(uint8_t) * image->width * (y - j);

                        if (data_offset > (int)img_memsize || data_offset < 0) {
                            continue;
                        }
                        if (image_data[data_offset] == 0) {
                            image_copy[copy_offset] = 0;
                            goto next_pixel;
                        }
                    }
                }

            next_pixel:;
            }
        }
    };

    int rows = image->height;
    do_threads(lambda, rows);

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

void ImageThreshold(Image *image, uint8_t threshold) {
    uint8_t *image_data = (uint8_t *)image->data;

    auto lambda = [image_data, threshold](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = sizeof(uint8_t) * t;

            uint8_t pixel = image_data[offset];

            if (pixel < threshold) {
                image_data[offset] = 0;
            } else {
                image_data[offset] = 255;
            }
        }
    };

    int pixels = image->height * image->width;
    do_threads(lambda, pixels);
}

void ImageInvert(Image *image) {
    uint8_t *image_data = (uint8_t *)image->data;

    auto lambda = [image_data](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = sizeof(uint8_t) * t;
            image_data[offset] = 255 - image_data[offset];
        }
    };

    int pixels = image->height * image->width;
    do_threads(lambda, pixels);
}

void ImagePow(Image *image, float expo) {
    uint8_t *image_data = (uint8_t *)image->data;

    auto lambda = [image_data, expo](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = sizeof(uint8_t) * t;
            float pixel = ((float)image_data[offset]) / 255.0f;
            pixel = pow(pixel, expo);
            image_data[offset] = (uint8_t)(pixel * 255.0f);
        }
    };

    int pixels = image->height * image->width;
    do_threads(lambda, pixels);
}

// Gera o gradiente de uma imagem normalizado entre 0 e 1 (0 e 255)
void ImageNormalizedGradient(Image *image) {
    // não podemos fazer a modificação in-place, uma iteração vai afetar o resultado da outra.
    uint8_t *grad_data = (uint8_t *)malloc(image->width * image->height);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int pixel = x + y * image->width;
            float dx = XDerivative(image, x, y);
            float dy = YDerivative(image, x, y);
            grad_data[pixel] = (uint8_t)std::clamp(VectorMagnitude(dx, dy) * 255.0f, 0.0f, 255.0f);
        }
    }

    free(image->data);
    image->data = grad_data;
}