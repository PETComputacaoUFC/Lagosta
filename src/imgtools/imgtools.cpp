#include "imgtools.hpp"

#include <mutex>
#include <thread>

#include "raylib.h"

const int MAX_THREADS = std::thread::hardware_concurrency();

float XDerivative(Image image, int x, int y) {
    bool forwards = (x == 0);
    bool backwards = (x == image.width - 1);
    bool off1 = (forwards | !backwards);
    bool off2 = (backwards | !forwards);

    return (GetPixelF(image, x + off1, y) - GetPixelF(image, x - off2, y)) / (float)(off1 + off2);
}

float YDerivative(Image image, int x, int y) {
    bool forwards = (y == 0);
    bool backwards = (y == image.height - 1);
    bool off1 = (forwards | !backwards);
    bool off2 = (backwards | !forwards);

    return (GetPixelF(image, x, y + off1) - GetPixelF(image, x, y - off2)) / (float)(off1 + off2);
}

// Wrapper de processamento multithreading
void DoThreaded(std::function<void(int start, int end)> lambda, int elements) {
    int thread_elements = elements / MAX_THREADS;
    int start_element = 0;
    int end_element = thread_elements;
    std::vector<std::thread> threads;
    for (int i = 0; i < MAX_THREADS; i++) {
        if (i == MAX_THREADS - 1) { end_element = elements; }
        threads.push_back(std::thread(lambda, start_element, end_element));
        start_element = end_element;
        end_element += thread_elements;
    }

    for (size_t i = 0; i < threads.size(); i++) { threads[i].join(); }
}

PixelVector FilterImage(Image* image, std::function<bool(Pixel)> filter) {
    uint8_t* img_data = (uint8_t*)image->data;
    PixelVector v = {};

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Pixel pixel{x, y, img_data[x + y * image->width]};
            if (filter(pixel)) { v.push_back(pixel); }
        }
    }

    return v;
}

PixelVector FilterImageThreshold(Image image, uint8_t threshold) {
    uint8_t* img_data = (uint8_t*)image.data;
    PixelVector v = {};

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            uint8_t value = img_data[x + y * image.width];
            if (value >= threshold) { v.push_back({x, y, value}); }
        }
    }

    return v;
}

// Conta quantos pixels de uma imagem fazem parte de uma reta, dado um certo threshold
int CountPixelsInLine(PixelVector pixel_vector, float const theta, float const rho,
                      float const threshold) {
    float t = DEG2RAD * theta;
    float st = sinf(t), ct = cosf(t);

    int count = 0;
    for (Pixel pixel : pixel_vector) {
        if (pixel.value == 255 && std::abs(pixel.x * ct + pixel.y * st - rho) < threshold) {
            count++;
        }
    }

    return count;
}

// Constrói o espaço parâmetral de Hough
void HoughParameterSpace::build_space(PixelVector pixel_vector, float threshold) {
    if (data != nullptr) { free(data); }
    data = (Line*)malloc(sizeof(Line) * width * height);

    // O cálculo de máximo global não é trivial de ser feito em threads.
    // É melhor realizar cálculos de máximo local e depois juntar tudo.
    std::mutex list_lock;
    std::vector<Line*> max_list{};

    // Função lambda de processamento do espaço parametral
    auto lambda = [this, pixel_vector, threshold, &max_list, &list_lock](int start_row,
                                                                         int end_row) {
        float rho = range_rho.start + range_rho.step * start_row;
        // Começa assumindo que o máximo local é o primeiro elemento do array local.
        Line* local_max = data + start_row * width;

        for (int i = start_row; i < end_row; i++) {
            float theta = range_theta.start;
            for (int j = 0; j < width; j++) {
                int parameter = j + i * width;
                int count = CountPixelsInLine(pixel_vector, theta, rho, threshold);
                data[parameter] = {theta, rho, count};
                if (count > local_max->count) { local_max = &(data[parameter]); }

                theta += range_theta.step;
            }

            rho += range_rho.step;
        }

        list_lock.lock();
        max_list.push_back(local_max);
        list_lock.unlock();
    };

    // Realiza o processamento
    DoThreaded(lambda, height);

    // Calcula o máximo global após o processamento
    max = data;  // Começa assumindo que o máximo global é o primeiro elemento do array.
    for (Line* local_max : max_list) {
        if (local_max->count > max->count) { max = local_max; }
    }
}

// Cria uma imagem grayscale com base no espaço parametral.
// branco = muitos pixels na linha; preto = poucos pixels
Image HoughParameterSpace::image() {
    uint8_t* img_data = (uint8_t*)malloc(width * height);
    float max_count = (float)max->count;

    for (int line = 0; line < width * height; line++) {
        float pixel = ((float)data[line].count) / max_count;
        img_data[line] = (uint8_t)(pixel * 255.0f);
    }
    return {img_data, width, height, 1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};
}

// Construtores
HoughParameterSpace::HoughParameterSpace(PixelVector pixel_vector, int diagonal, float threshold)
    : range_rho{0.0f, (float)diagonal, DEFAULT_RHO_STEP},
      width(range_theta.size()),
      height(range_rho.size()) {
    build_space(pixel_vector, threshold);
}
HoughParameterSpace::HoughParameterSpace(PixelVector pixel_vector, int diagonal, Range theta_range,
                                         float rho_step, float threshold)
    : range_theta(theta_range),
      range_rho{0.0f, (float)diagonal, rho_step},
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
