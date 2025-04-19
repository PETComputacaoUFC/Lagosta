#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "raylib.h"

void ImageDilate(Image* image, int kernelSize);

void ImageErode(Image* image, int kernelSize);

void ImageThreshold(Image* image, unsigned char threshold);

void ImageInvert(Image* image);