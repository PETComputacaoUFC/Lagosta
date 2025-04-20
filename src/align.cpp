#include "align.hpp"
#include "raylib.h"
#include <cstdio>
#include <cstdlib>

unsigned char GetPixel(Image* image, int x, int y) {
    int offset = x + y * image->width;
    if (offset >= image->width * image->height) { return 0; } // pixel fora da imagem
    return ( (unsigned char*) image->data)[offset];
}

int ImageTestTemplate(Image* image, Image* plate, int x, int y) {
    int match = 0;
    for (int plate_y = 0; plate_y < plate->height; plate_y++) {
        for (int plate_x = 0; plate_x < plate->width; plate_x++ ) {
            unsigned char img_pixel = GetPixel(image, x + plate_x, y + plate_y);
            unsigned char plate_pixel = GetPixel(plate, plate_x, plate_y);
            match += 255 - abs(img_pixel - plate_pixel);
        }
    }
    return match;
}

Vector2 ImageFindTemplate(Image* image, Image* plate) {
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