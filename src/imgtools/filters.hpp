#pragma once
#include <cstdint>

#include "raylib.h"

// O valor de cada pixel se torna o MAIOR valor dos pixels em sua vizinhança
void ImageDilate(Image *image, int kernel_radius);

// O valor de cada pixel se torna o MENOR valor dos pixels em sua vizinhança
void ImageErode(Image *image, int kernel_radius);

// Colore preto cada pixel abaixo do threshold, e branco cada pixel igual ou acima.
void ImageThreshold(Image *image, uint8_t threshold);

// Trata cada pixel de uma imagem como um número entre 0 e 1, e eleva ele a um exponente
void ImagePow(Image *image, float expo);

// Realiza uma inversão simples na cor da imagem (pixel = 255 - pixel)
// Essa função é mais rápida que a função do raylib pq é multithreaded
// e não faz certos checks (incluindo o de uma imagem ser colorida)
void ImageColorInvertFast(Image *image);

// Gera o gradiente de uma imagem normalizado entre 0 e 1 (0 e 255)
void ImageNormalizedGradient(Image *image);