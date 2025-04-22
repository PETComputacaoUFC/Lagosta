#pragma once
#include <sys/types.h>

#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

#include "raylib.h"

struct Range {
    float start = 0.0f;
    float end;
    float step = 1.0f;

    inline int size() const { return std::abs((end - start + step) / step); }
};

struct Pixel {
    int x;
    int y;
    uint8_t value;
};

typedef std::vector<Pixel> PixelVector;

PixelVector FilterImage(Image *image, std::function<bool(Pixel)>);

PixelVector FilterImageThreshold(Image *image, uint8_t threshold);

void ImageNormalizedGradient(Image *image);

inline unsigned char GetPixel(Image *image, int x, int y) {
    return ((unsigned char *)image->data)[x + y * image->width];
}
inline unsigned char GetPixelSafe(Image *image, int x, int y) {
    if (static_cast<unsigned>(x) >= static_cast<unsigned>(image->width)
        || static_cast<unsigned>(y) >= static_cast<unsigned>(image->height)) {
        return 0;
    }
    return ((unsigned char *)image->data)[x + y * image->width];
}

inline float GetPixelF(Image *image, int x, int y) {
    return ((float)GetPixel(image, x, y)) / 255.0f;
}
inline float GetPixelFSafe(Image *image, int x, int y) {
    return ((float)GetPixelSafe(image, x, y)) / 255.0f;
}