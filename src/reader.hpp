#pragma once
#include <raylib.h>
#include <vector>
#include <raymath.h>

enum ReadMode: unsigned char {
    RANDOM_SAMPLE = 0, // Averages many random samples in a square of size 2*read_radius around the item's center.
    SAMPLE_CIRCLE = 1, // Averages a perfect circle of radius read_radius around the item's center.
    SAMPLE_SQUARE = 2,    // Averages a square with size 2*read_radius around the item's center.
};

struct Item {
    int choice = -1;
    std::vector<float> choice_reading;
};

class Reader {
public:
    // TODO: ReadingBox with coordinates instead of hard-coded values (see reader.cpp)
    Image* image;
    Vector2 square[4];
    ReadMode read_mode;
    int read_radius;
    std::vector<Item> items = std::vector<Item>(20); // questões
    std::vector<Item> head = std::vector<Item>(2);   // cabeçalho: modalidade e fase

    Reader();
    Reader(Image* image, Vector2 square[4], ReadMode read_mode, int read_radius);

    void read();
private:
    unsigned char read_pixel(int x, int y);
    float read_area(int x, int y);
};