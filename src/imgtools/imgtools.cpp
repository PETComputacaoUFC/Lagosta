#include "imgtools.hpp"

#include <algorithm>

#include "raylib.h"

float XDerivative(Image *image, int x, int y) {
    bool forwards = (x == 0);
    bool backwards = (x == image->width - 1);
    bool off1 = (forwards | !backwards);
    bool off2 = (backwards | !forwards);

    return (GetPixelF(image, x + off1, y) - GetPixelF(image, x - off2, y)) / (float)(off1 + off2);
}

float YDerivative(Image *image, int x, int y) {
    bool forwards = (y == 0);
    bool backwards = (y == image->height - 1);
    bool off1 = (forwards | !backwards);
    bool off2 = (backwards | !forwards);

    return (GetPixelF(image, x, y + off1) - GetPixelF(image, x, y - off2)) / (float)(off1 + off2);
}

inline float Magnitude(float x, float y) { return sqrtf(x * x + y * y); }

PixelVector FilterImage(Image *image, std::function<bool(Pixel)> filter) {
    uint8_t *img_data = (uint8_t *)image->data;
    PixelVector v = {};

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Pixel pixel(x, y, img_data[x + y * image->width]);
            if (filter(pixel)) {
                v.push_back(pixel);
            }
        }
    }

    return v;
}

PixelVector FilterImageThreshold(Image *image, uint8_t threshold) {
    uint8_t *img_data = (uint8_t *)image->data;
    PixelVector v = {};

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            uint8_t value = img_data[x + y * image->width];
            if (value >= threshold) {
                v.push_back({x, y, value});
            }
        }
    }

    return v;
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
            grad_data[pixel] = (uint8_t)std::clamp(Magnitude(dx, dy) * 255.0f, 0.0f, 255.0f);
        }
    }

    free(image->data);
    image->data = grad_data;
}