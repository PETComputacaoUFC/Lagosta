#include "align.hpp"
#include "raylib.h"
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <thread>
#include <future>
#include <vector>

const int MAX_THREADS = std::thread::hardware_concurrency();

unsigned char GetPixel(Image* image, int x, int y) {
    int offset = x + y * image->width;
    if (offset >= image->width * image->height) { return 0; } // pixel fora da imagem
    return ( (unsigned char*) image->data)[offset];
}

int ImageTestTemplateV(Image* image, Image* plate, Vector2 coords) {
    int match = 0;
    for (int plate_y = 0; plate_y < plate->height; plate_y++) {
        for (int plate_x = 0; plate_x < plate->width; plate_x++ ) {
            unsigned char img_pixel = GetPixel(image, coords.x + plate_x, coords.y + plate_y);
            unsigned char plate_pixel = GetPixel(plate, plate_x, plate_y);
            match += 255 - abs(img_pixel - plate_pixel);
        }
    }
    return match;
}

Vector2 ImageFindTemplate(Image* image, Image* plate) {
    std::function<std::vector<int> (int, int)> lambda = [image, plate](int start_row, int end_row) {
        Vector2 match_coords = {0,  0};
        int best_match = 0;

        for (int img_y = start_row; img_y < end_row; img_y++) {
            for (int img_x = 0; img_x < image->width; img_x++) {
                Vector2 coords = Vector2(img_x, img_y);
                int match = ImageTestTemplateV(image, plate, coords);
                if (match > best_match) {
                    best_match = match;
                    match_coords = coords;
                }
            }
        }

        std::vector<int> answer;
        answer.push_back(best_match);
        answer.push_back(match_coords.x);
        answer.push_back(match_coords.y);

        return answer;
    };

    int elements = image->height;
    int thread_elements = elements / MAX_THREADS;
    int start_element = 0;
    int end_element = thread_elements;
    std::vector<std::future<std::vector<int>>> threads;
    for (int i = 0; i < MAX_THREADS; i++) {
        if (i == MAX_THREADS - 1) { end_element = elements; }
        threads.push_back(std::async(lambda, start_element, end_element));
        start_element = end_element;
        end_element += thread_elements;
    }

    Vector2 match_coords = {0, 0};
    int best_match = 0;
    for (size_t i = 0; i < threads.size(); i++) {
        std::vector<int> answer = threads[i].get();
        if (answer[0] > best_match) {
            best_match = answer[0];
            match_coords = Vector2(answer[1], answer[2]);
        }
    }

    printf("%d\n", best_match);
    return match_coords;
}