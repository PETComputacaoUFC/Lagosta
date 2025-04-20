#include "imgtools.hpp"
#include "raylib.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

float XDerivative(Image* image, int x, int y) {
    if (x == 0) {
        // forwards
        return GetPixelF(image, x + 1, y) - GetPixelF(image, x, y);
    } else if (x == image->width - 1) {
        // backwards
        return GetPixelF(image, x, y) - GetPixelF(image, x - 1, y);
    }
    // central
    return (GetPixelF(image, x + 1, y) - GetPixelF(image, x - 1, y)) / 2.0f;
}

float YDerivative(Image* image, int x, int y) {
    if (y == 0) {
        // forwards
        return GetPixelF(image, x, y + 1) - GetPixelF(image, x, y);
    } else if (y == image->height - 1) {
        // backwards
        return GetPixelF(image, x, y) - GetPixelF(image, x, y - 1);
    }
    // central
    return (GetPixelF(image, x, y + 1) - GetPixelF(image, x, y - 1)) / 2.0f;
}

GradientVector::GradientVector(float x, float y) {
    this->x = x;
    this->y = y;
}

GradientVector::GradientVector(Image* image, int x, int y) {
    this->x = XDerivative(image, x, y);
    this->y = YDerivative(image, x, y);
}

Gradient::Gradient(Image* image) {
    width = image->width;
    height = image->height;
    data = (GradientVector*) malloc(width * height * sizeof(GradientVector));
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            data[width * y + x] = GradientVector(image, x, y);
        }
    }
    printf("\n");
}

Image Gradient::image() {
    Image i = LoadImage("");
    i.data = malloc(width * height); // unsigned char é tamanho 1 ent n importa o sizeof
    i.width = width;
    i.height = height;
    i.mipmaps = 1;
    i.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;

    unsigned char* img_data = (unsigned char*) i.data;
    for (int v = 0; v < width * height; v++) {
        img_data[v] = (unsigned char) std::clamp(data[v].magnitude() * 255.0f, 0.0f, 255.0f);
    }
    return i;
}