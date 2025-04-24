#include "filters.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "imgtools.hpp"


// O valor de cada pixel se torna o MAIOR valor dos pixels em sua vizinhança
void ImageDilate(Image *image, int kernel_radius) {
    size_t img_memsize = image->width * image->height;
    uint8_t *img_data = (uint8_t *)image->data;
    uint8_t *img_copy = (uint8_t *)malloc(img_memsize);
    memcpy(img_copy, img_data, img_memsize);

    int width = image->width;
    int height = image->height;
    auto lambda = [width, height, img_data, img_copy, kernel_radius](int start_row, int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < width; x++) {
                int copy_offset = x + width * y;

                // Pega o maior valor da vizinhança
                uint8_t max_pixel = 0;
                for (int ky = y - kernel_radius; ky <= y + kernel_radius; ky++) {
                    if (ky < 0 || ky >= height) { continue; }
                    for (int kx = x - kernel_radius; kx <= x + kernel_radius; kx++) {
                        if (kx < 0 || kx >= width) { continue; }

                        size_t data_offset = kx + width * ky;
                        uint8_t kpixel = img_data[data_offset];
                        max_pixel = kpixel > max_pixel ? kpixel : max_pixel;
                    }
                }

                img_copy[copy_offset] = max_pixel;
            }
        }
    };

    int rows = image->height;
    DoThreaded(lambda, rows);

    image->data = img_copy;
    free(img_data);
}

// O valor de cada pixel se torna o MENOR valor dos pixels em sua vizinhança
void ImageErode(Image *image, int kernel_radius) {
    size_t img_memsize = image->width * image->height;
    uint8_t *img_data = (uint8_t *)image->data;
    uint8_t *img_copy = (uint8_t *)malloc(img_memsize);
    memcpy(img_copy, img_data, img_memsize);

    int width = image->width;
    int height = image->height;
    auto lambda = [width, height, img_data, img_copy, kernel_radius](int start_row, int end_row) {
        for (int y = start_row; y < end_row; y++) {
            for (int x = 0; x < width; x++) {
                int copy_offset = x + width * y;

                // Pega o menor valor da vizinhança
                uint8_t min_pixel = 255;
                for (int ky = y - kernel_radius; ky <= y + kernel_radius; ky++) {
                    if (ky < 0 || ky >= height) { continue; }
                    for (int kx = x - kernel_radius; kx <= x + kernel_radius; kx++) {
                        if (kx < 0 || kx >= width) { continue; }

                        size_t data_offset = kx + width * ky;
                        uint8_t kpixel = img_data[data_offset];
                        min_pixel = kpixel < min_pixel ? kpixel : min_pixel;
                    }
                }

                img_copy[copy_offset] = min_pixel;
            }
        }
    };

    int rows = image->height;
    DoThreaded(lambda, rows);

    image->data = img_copy;
    free(img_data);
}

// Colore preto cada pixel abaixo do threshold, e branco cada pixel igual ou acima.
void ImageThreshold(Image *image, uint8_t threshold) {
    uint8_t *img_data = (uint8_t *)image->data;

    auto lambda = [img_data, threshold](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = t;

            uint8_t pixel = img_data[offset];

            if (pixel < threshold) {
                img_data[offset] = 0;
            } else {
                img_data[offset] = 255;
            }
        }
    };

    int pixels = image->height * image->width;
    DoThreaded(lambda, pixels);
}

// Trata cada pixel de uma imagem como um número entre 0 e 1, e eleva ele a um exponente
void ImagePow(Image *image, float expo) {
    uint8_t *img_data = (uint8_t *)image->data;

    auto lambda = [img_data, expo](int start_pixel, int end_pixel) {
        for (int t = start_pixel; t < end_pixel; t++) {
            int offset = t;
            float pixel = ((float)img_data[offset]) / 255.0f;
            pixel = pow(pixel, expo);
            img_data[offset] = (uint8_t)(pixel * 255.0f);
        }
    };

    int pixels = image->height * image->width;
    DoThreaded(lambda, pixels);
}

// Realiza uma inversão simples na cor da imagem (pixel = 255 - pixel)
// Essa função é mais rápida que a função do raylib pq é multithreaded
// e não faz certos checks (incluindo o de uma imagem ser colorida)
void ImageColorInvertFast(Image *image) {
    uint8_t *img_data = (uint8_t *)image->data;
    auto lambda = [img_data](int start_pixel, int end_pixel) {
        for (int pixel = start_pixel; pixel < end_pixel; pixel++) {
            img_data[pixel] = 255 - img_data[pixel];
        }
    };
    DoThreaded(lambda, image->width * image->height);
}

// Gera o gradiente de uma imagem normalizado entre 0 e 1 (0 e 255)
void ImageNormalizedGradient(Image *image) {
    // não podemos fazer a modificação in-place, uma iteração vai afetar o resultado da outra.
    uint8_t *grad_data = (uint8_t *)malloc(image->width * image->height);

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

void ImageKernelConvolutionFast(Image *image, Kernel kernel) {
    uint8_t *img_copy = (uint8_t *)malloc(image->width * image->height * sizeof(uint8_t));

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
