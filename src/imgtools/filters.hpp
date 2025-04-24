#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "raylib.h"

struct Kernel {
    float *data;
    size_t size;

    inline Kernel(float *kernel_data, size_t kernel_size) : data(kernel_data), size(kernel_size) {}

    inline Kernel(float all, size_t kernel_size) : size(kernel_size) {
        data = (float *)malloc(kernel_size * sizeof(float));
        for (size_t i = 0; i < kernel_size; i++) { data[i] = all; }
    };

    inline void normalize() {
        float sum = 0;
        for (size_t i = 0; i < size; i++) { sum += data[i]; }
        for (size_t i = 0; i < size; i++) { data[i] /= sum; }
    };

    inline Kernel normalized() {
        Kernel k(*this);
        float sum = 0;
        for (size_t i = 0; i < size; i++) { sum += k.data[i]; }
        for (size_t i = 0; i < size; i++) { k.data[i] /= sum; }
        return k;
    };

    inline Kernel operator*(float x) {
        Kernel k(*this);
        for (size_t i = 0; i < size; i++) { k.data[i] *= x; }
        return k;
    };
};

// clang-format off
const Kernel KERNEL_BOX_BLUR = Kernel{1, 9}.normalized();
const Kernel KERNEL_LAPLACE = Kernel{
    (float[9]){
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0,
    },
    9
}.normalized();
// clang-format on

// O valor de cada pixel se torna o MAIOR valor dos pixels em sua vizinhança
void ImageDilate(Image *image, int kernel_radius);

// O valor de cada pixel se torna o MENOR valor dos pixels em sua vizinhança
void ImageErode(Image *image, int kernel_radius);

// Colore preto cada pixel abaixo do threshold, e branco cada pixel igual ou acima.
void ImageThreshold(Image *image, uint8_t threshold);

// Trata cada pixel de uma imagem como um número entre 0 e 1, e eleva ele a um exponente
void ImagePow(Image *image, float expo);

// Gera o gradiente de uma imagem normalizado entre 0 e 1 (0 e 255)
void ImageNormalizedGradient(Image *image);

// Realiza uma inversão simples na cor da imagem (pixel = 255 - pixel)
// Essa função é mais rápida que a função do raylib pq é multithreaded
// e não faz certos checks (incluindo o de uma imagem ser colorida)
void ImageColorInvertFast(Image *image);

// Aplica um kernel convolution numa imagem
// Essa função é mais rápida que a função do raylib pq é multithreaded
// e não faz certos checks (incluindo o de uma imagem ser colorida)
void ImageKernelConvolutionFast(Image *image, Kernel kernel);