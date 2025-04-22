#pragma once
#include <cstdint>

#include "raylib.h"

void ImageDilate(Image *image, int kernel_size);

void ImageErode(Image *image, int kernel_size);

void ImageThreshold(Image *image, uint8_t threshold);

void ImageInvert(Image *image);

void ImagePow(Image *image, float expo);