#pragma once
#include "raylib.h"

Vector2 ImageFindTemplate(Image* image, Image* plate);

int ImageTestTemplateV(Image* image, Image* plate, Vector2 coords);