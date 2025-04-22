#include "align.hpp"

#include <cstdint>

#include "imgtools.hpp"
#include "raylib.h"

// Conta quantos pixels de uma imagem fazem parte de uma reta, dado um certo threshold
int CountPixelsInLine(PixelVector const *pixel_vector, float const theta, float const rho,
                      float const threshold) {
    float t = DEG2RAD * theta;
    float st = sinf(t), ct = cosf(t);

    int count = 0;
    for (Pixel pixel : *pixel_vector) {
        if (pixel.value == 255 && std::abs(pixel.x * ct + pixel.y * st - rho) < threshold) {
            count++;
        }
    }

    return count;
}

// Constrói o espaço parâmetral de Hough
void HoughParameterSpace::build_space(PixelVector const *pixel_vector, float threshold) {
    data = (Line *)malloc(sizeof(Line) * width * height);
    max = data;

    float rho = range_rho.start;
    for (int i = 0; i < height; i++) {
        float theta = range_theta.start;

        for (int j = 0; j < width; j++) {
            int parameter = j + i * width;

            int count = CountPixelsInLine(pixel_vector, theta, rho, threshold);
            data[parameter] = {theta, rho, count};
            if (count > max->count) {
                max = &(data[parameter]);
            }

            theta += range_theta.step;
        }

        rho += range_rho.step;
    }
}

// Cria uma imagem grayscale com base no espaço parametral.
// branco = muitos pixels na linha; preto = poucos pixels
Image HoughParameterSpace::image() {
    uint8_t *img_data = (uint8_t *)malloc(width * height);
    float max_count = (float)max->count;

    for (int line = 0; line < width * height; line++) {
        float pixel = ((float)data[line].count) / max_count;
        img_data[line] = (uint8_t)(pixel * 255.0f);
    }
    return {img_data, width, height, 1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};
}

// Construtores
HoughParameterSpace::HoughParameterSpace(const PixelVector *pixel_vector, int diagonal,
                                         float threshold)
    : range_rho(0.0f, diagonal, DEFAULT_RHO_STEP),
      width(range_theta.size()),
      height(range_rho.size()) {
    build_space(pixel_vector, threshold);
}
HoughParameterSpace::HoughParameterSpace(const PixelVector *pixel_vector, int diagonal,
                                         Range theta_range, float rho_step, float threshold)
    : range_theta(theta_range),
      range_rho(0.0f, diagonal, rho_step),
      width(range_theta.size()),
      height(range_rho.size()) {
    build_space(pixel_vector, threshold);
}

// Retorna o ponto de interseção entre duas retas.
// Fórmula desenvolvida a partir da representação normal da reta:
// x*cos(t) + y*sin(t) = p
Vector2 IntersectionPoint(Line l1, Line l2) {
    float p1 = l1.rho;
    float p2 = l2.rho;
    float t1 = DEG2RAD * l1.theta;
    float t2 = DEG2RAD * l2.theta;

    float x = (p2 * sinf(t1) - p1 * sinf(t2)) / sin(t1 - t2);
    float y = (p2 * cosf(t1) - p1 * cosf(t2)) / sin(t2 - t1);

    return {x, y};
}
