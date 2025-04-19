#pragma once
#include <raylib.h>
#include <vector>
#include <raymath.h>

enum ReadMode: unsigned char {
    SAMPLE_SQUARE, // Averages a square with size 2*read_radius around the item's center.
    SAMPLE_CIRCLE, // Averages a circle of radius read_radius around the item's center.
};

struct Item {
    int choice = -1;
    std::vector<float> choice_readings;
};

class Reader {
public:
    // TODO: ReadingBox with coordinates instead of hard-coded values (see reader.cpp)
    Image* image;
    Vector2 square[4];
    ReadMode read_mode;
    int read_radius;
    float read_threshold;
    float avg_threshold;
    std::vector<Item> items = std::vector<Item>(20); // questões
    std::vector<Item> head = std::vector<Item>(2);   // cabeçalho: modalidade e fase

    Reader();
    Reader(Image* image, Vector2 square[4], ReadMode read_mode, int read_radius, float read_threshold, float avg_threshold);

    void read();
private:
    float read_pixel(int x, int y);
    float read_area(int x, int y);
};