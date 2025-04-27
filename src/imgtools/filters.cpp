#include "filters.hpp"

#include <sys/types.h>

#include <algorithm>
#include <cstring>

#include "imgtools.hpp"


// O valor de cada pixel se torna o MAIOR valor dos pixels em sua vizinhança
void ImageDilate(Image* image, int kernel_radius) {
    size_t img_memsize = image->width * image->height;
    uint8_t* image_data = (uint8_t*)image->data;
    uint8_t* image_copy = (uint8_t*)malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    auto lambda = [image, image_copy, kernel_radius](int start_row, int end_row) {
        int width = image->width;
        int height = image->height;
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < width; x++) {
                int copy_offset = x + width * y;

                for (int i = -kernel_radius; i <= kernel_radius; i++) {
                    int ky = y + i;
                    if (ky < 0 || ky >= height - 1) { continue; }
                    for (int j = -kernel_radius; j <= kernel_radius; j++) {
                        int kx = x + j;
                        if (kx < 0 || kx >= width - 1) { continue; }

                        uint8_t pixel = GetPixel(*image, kx, ky);
                        if (pixel == 255) {
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
    DoThreaded(lambda, rows);

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

// O valor de cada pixel se torna o MENOR valor dos pixels em sua vizinhança
void ImageErode(Image* image, int kernel_radius) {
    size_t img_memsize = image->width * image->height;
    uint8_t* image_data = (uint8_t*)image->data;
    uint8_t* image_copy = (uint8_t*)malloc(img_memsize);
    memcpy(image_copy, image_data, img_memsize);

    auto lambda = [image, image_copy, kernel_radius](int start_row, int end_row) {
        int width = image->width;
        int height = image->height;
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < width; x++) {
                int copy_offset = x + width * y;

                for (int i = -kernel_radius; i <= kernel_radius; i++) {
                    int ky = y + i;
                    if (ky < 0 || ky >= height - 1) { continue; }
                    for (int j = -kernel_radius; j <= kernel_radius; j++) {
                        int kx = x + j;
                        if (kx < 0 || kx >= width - 1) { continue; }

                        uint8_t pixel = GetPixel(*image, kx, ky);
                        if (pixel == 0) {
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
    DoThreaded(lambda, rows);

    memcpy(image_data, image_copy, img_memsize);
    free(image_copy);
}

// Colore preto cada pixel abaixo do threshold, e branco cada pixel igual ou acima.
void ImageThreshold(Image* image, uint8_t threshold) {
    uint8_t* image_data = (uint8_t*)image->data;

    auto lambda = [image_data, threshold](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = t;

            uint8_t pixel = image_data[offset];

            if (pixel < threshold) {
                image_data[offset] = 0;
            } else {
                image_data[offset] = 255;
            }
        }
    };

    int pixels = image->height * image->width;
    DoThreaded(lambda, pixels);
}

// Trata cada pixel de uma imagem como um número entre 0 e 1, e eleva ele a um exponente
void ImagePow(Image* image, float expo) {
    uint8_t* image_data = (uint8_t*)image->data;

    auto lambda = [image_data, expo](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = t;
            float pixel = ((float)image_data[offset]) / 255.0f;
            pixel = pow(pixel, expo);
            image_data[offset] = (uint8_t)(pixel * 255.0f);
        }
    };

    int pixels = image->height * image->width;
    DoThreaded(lambda, pixels);
}

// Realiza uma inversão simples na cor da imagem (pixel = 255 - pixel)
// Essa função é mais rápida que a função do raylib pq é multithreaded
// e não faz certos checks (incluindo o de uma imagem ser colorida)
void ImageColorInvertFast(Image* image) {
    uint8_t* img_data = (uint8_t*)image->data;
    auto lambda = [img_data](int start_pixel, int end_pixel) {
        for (int pixel = start_pixel; pixel < end_pixel; pixel++) {
            img_data[pixel] = 255 - img_data[pixel];
        }
    };
    DoThreaded(lambda, image->width * image->height);
}

// Gera o gradiente de uma imagem normalizado entre 0 e 1 (0 e 255)
void ImageNormalizedGradient(Image* image) {
    // não podemos fazer a modificação in-place, uma iteração vai afetar o resultado da outra.
    uint8_t* grad_data = (uint8_t*)malloc(image->width * image->height);

    auto lambda = [grad_data, image](int start_row, int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < image->width; x++) {
                int pixel = x + y * image->width;
                float dx = XDerivative(*image, x, y);
                float dy = YDerivative(*image, x, y);
                grad_data[pixel] =
                    (uint8_t)std::clamp(VectorMagnitude(dx, dy) * 255.0f, 0.0f, 255.0f);
            }
        }
    };

    int rows = image->height;
    DoThreaded(lambda, rows);

    free(image->data);
    image->data = grad_data;
}

void ImageKernelConvolutionFast(Image* image, Kernel kernel) {
    uint8_t* img_copy = (uint8_t*)malloc(image->width * image->height * sizeof(uint8_t));

    auto lambda = [kernel, img_copy, image](int start_row, int end_row) {
        int kernel_width = (int)sqrt((double)kernel.size);
        int kernel_radius = kernel_width / 2;

        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < image->width; x++) {
                int offset = x + y * image->width;
                float sum = 0.0f;

                for (int ky = -kernel_radius; ky <= kernel_radius; ky++) {
                    for (int kx = -kernel_radius; kx <= kernel_radius; kx++) {
                        float kvalue =
                            kernel.data[(kx + kernel_radius) + (ky + kernel_radius) * kernel_width];
                        float kpixel = (float)GetPixelSafe(*image, x + kx, y + ky);
                        sum += kpixel * kvalue;
                    }
                }

                img_copy[offset] = (uint8_t)std::clamp(sum, 0.0f, 255.0f);
            }
        }
    };

    int rows = image->height;
    DoThreaded(lambda, rows);

    free(image->data);
    image->data = img_copy;
}
