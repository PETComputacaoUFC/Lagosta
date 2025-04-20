#pragma once
#include "raylib.h"

Vector2 ImageFindTemplate(Image* image, Image* plate);

int ImageTestTemplate(Image* image, Image* plate, int x, int y);