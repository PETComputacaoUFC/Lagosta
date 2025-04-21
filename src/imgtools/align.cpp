#include "align.hpp"
#include <cstdio>
#include <cstdlib>
#include "raylib.h"
#include "imgtools.hpp"

int ImageTestTemplate(Image *image, Image *plate, int x, int y) {
    int match = 0;
    for (int plate_y = 0; plate_y < plate->height; plate_y++) {
        for (int plate_x = 0; plate_x < plate->width; plate_x++ ) {
            unsigned char img_pixel = GetPixelSafe(image, x + plate_x, y + plate_y);
            unsigned char plate_pixel = GetPixel(plate, plate_x, plate_y);
            match += 255 - abs(img_pixel - plate_pixel);
        }
    }
    return match;
}

Vector2 ImageFindTemplate(Image *image, Image *plate) {
    int match_x = 0, match_y = 0;
    int best_match = 0;

    for (int img_y = 0; img_y < image->height; img_y++) {
        for (int img_x = 0; img_x < image->width; img_x++) {
            int match = ImageTestTemplate(image, plate, img_x, img_y);
            if (match > best_match) {
                best_match = match;
                match_x = img_x;
                match_y = img_y;
            }
        }
    }

    return { (float) match_x, (float) match_y };
}

struct Range {
    float start;
    float stop;
    float step;
    bool inclusive;

    Range(float start, float end, float step = 1)
        : start(start), stop(end), step(step), inclusive(false) {}
    Range(float start, float end, float step, bool inclusive)
        : start(start), stop(end), step(step), inclusive(inclusive) {}

    struct Iterator {
        float value;
        float step;

        Iterator(float value, float step) : value(value), step(step) {}
        Iterator &operator++() { value += step; return *this; }
        bool operator!=(const Iterator &other) const { return (value - other.value) * step < 0.0f; }
        float operator*() const { return value; }
    };

    Iterator begin() const { return Iterator(start, step); }
    Iterator end() const {
        const float FLOAT_EPSILON = 4e-7; // erro estimado por float - número mágico ;P
        float offset = -step * !inclusive; // offset dependendo se o range é inclusivo
        float error = FLOAT_EPSILON * std::abs(stop - start) / step; // erro de float: epsilon * número de passos (nº de somas)
        return Iterator(stop + offset + error, step);
    }
};