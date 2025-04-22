#pragma once

#include "imgtools.hpp"
#include "raylib.h"

const Range DEFAULT_THETA_RANGE = {-90.0, 90.0, 1.0};
const float DEFAULT_RHO_STEP = 0.5;

inline float GetDiagonal(Image *image) {
    return sqrtf(image->width * image->width + image->height * image->height);
}

struct Line {
    float theta;
    float rho;
    int count;
};

// Espaço parametral da transformada de Hough
//  - https://en.wikipedia.org/wiki/Hough_transform
//  - Digital Image Processing, 4th Edition (Rafael C.) - pág. 920
struct HoughParameterSpace {
private:
    void build_space(const PixelVector *pixel_vector, const float threshold);

public:
    // JAMAIS esquecer de converter o theta pra radianos.
    Range range_theta = DEFAULT_THETA_RANGE;
    Range range_rho;

    // Reta (theta,rho) no espaço com maior número de pontos (x,y) associados
    Line *max;
    Line *data;

    int width;
    int height;

    HoughParameterSpace(const PixelVector *pixel_vector, int diagonal, float threshold = 1);
    HoughParameterSpace(const PixelVector *pixel_vector, int diagonal, Range theta_range,
                        const float rho_step, const float threshold = 1);

    Image image();  // cria uma imagem com base no espaço
};

Vector2 IntersectionPoint(Line l1, Line l2);
