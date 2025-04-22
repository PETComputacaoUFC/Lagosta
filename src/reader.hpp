#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "raylib.h"

enum ReadMode : uint8_t {
    SAMPLE_SQUARE,  // Averages a square with size 2*read_radius around the item's center.
    SAMPLE_CIRCLE,  // Averages a circle of radius read_radius around the item's center.
};

struct Item {
    char choice = '0';
    std::vector<float> choice_readings;
};

// TODO: ReadingBox with coordinates instead of hard-coded values (see reader.cpp)
struct ReadingBox {};

struct Reader {
public:
    std::string answer_string = "";

    Vector2 square[4];

    Image image;            // Grayscale image
    Image image_filtered1;  // Inversion + contrast
    Image image_filtered2;  // Threshold filter on top of filter 1

    std::vector<Item> items = std::vector<Item>(20);  // questões
    std::vector<Item> head = std::vector<Item>(2);    // cabeçalho: modalidade e fase

    ReadMode read_mode;

    Reader();
    Reader(Image* image, Vector2 square[4], ReadMode read_mode);

    std::string read();
    void draw_reading();

private:
    float read_pixel(Image* image, int x, int y);
    float read_area(Image* image, int x, int y);
};