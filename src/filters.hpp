#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "raylib.h"

void ImageDilate(Image* image, int kernel_size);

void ImageErode(Image* image, int kernel_size);

void ImageThreshold(Image* image, unsigned char threshold);

void ImageInvert(Image* image);

void ImagePow(Image* image, float expo);