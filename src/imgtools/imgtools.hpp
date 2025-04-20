#pragma once
#include "raylib.h"
#include <cmath>
#include <cstdlib>

struct GradientVector {
    float x;
    float y;

    GradientVector(float x, float y);
    GradientVector(Image* image, int x, int y);

    inline float magnitude() { return sqrt(x*x + y*y); }
    inline float fast_magnitude() { return std::abs(x) + std::abs(y); }
};

struct Gradient {
    GradientVector* data;
    int width, height;

    Gradient(Image* image);

    Image image();

    // unsafe getter
    inline GradientVector* operator[](int y) {
        return data + y * width * sizeof(GradientVector);
    }
    // safe getter
    inline GradientVector at(int x, int y) {
        if (static_cast<unsigned>(x) >= static_cast<unsigned>(width)
        || static_cast<unsigned>(y) >= static_cast<unsigned>(height)) {
            return GradientVector { 0.0f, 0.0f };
        }
        return data[x + y * width];
    };
};


// unsafe getter
inline unsigned char GetPixel(Image *image, int x, int y) {
    return ((unsigned char*)image->data)[x + y * image->width];
}
// safe getter
inline unsigned char GetPixelSafe(Image *image, int x, int y) {
    if (static_cast<unsigned>(x) >= static_cast<unsigned>(image->width)
    || static_cast<unsigned>(y) >= static_cast<unsigned>(image->height)) {
        return 0;
    }
    return ((unsigned char*)image->data)[x + y * image->width];
}

inline float GetPixelF(Image* image, int x, int y) { return ((float) GetPixel(image, x, y)) / 255.0f; }
inline float GetPixelFSafe(Image* image, int x, int y) { return ((float) GetPixelSafe(image, x, y)) / 255.0f; }