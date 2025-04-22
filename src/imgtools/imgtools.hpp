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

// Reta representada pela forma normal: x*cos(t) + y*sin(t) = p
struct Line {
    float theta;
    float rho;
    int count;
};

struct Pixel {
    int x;
    int y;
    uint8_t value;
};

typedef std::vector<Pixel> PixelVector;

// Espaço parametral da transformada de Hough
//  - https://en.wikipedia.org/wiki/Hough_transform
//  - Digital Image Processing, 4th Edition (Rafael C.) - pág. 920
const Range DEFAULT_THETA_RANGE = {-90.0, 90.0, 1.0};
const float DEFAULT_RHO_STEP = 0.5;
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

// Retorna um vetor com os pixels da imagem que forem aprovados por uma função filtro
PixelVector FilterImage(Image *image, std::function<bool(Pixel)> filter);
// Retorna um vetor com os pixels da imagem que tiverem valor >= threshold
PixelVector FilterImageThreshold(Image *image, uint8_t threshold);

// Derivada parcial em x de um ponto de uma imagem
float XDerivative(Image *image, int x, int y);
// Derivada parcial em y de um ponto de uma imagem
float YDerivative(Image *image, int x, int y);

// Retorna o ponto de interseção entre duas retas
Vector2 IntersectionPoint(Line l1, Line l2);

inline float VectorMagnitude(float x, float y) { return sqrtf(x * x + y * y); }

inline float GetDiagonalLength(Image *image) {
    return sqrtf(image->width * image->width + image->height * image->height);
}

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