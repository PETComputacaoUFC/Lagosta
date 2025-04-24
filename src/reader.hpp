#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "raylib.h"

enum ReadMode : uint8_t {
    SAMPLE_SQUARE,  // Averages a square with size 2*read_radius around the item's center.
    SAMPLE_CIRCLE,  // Averages a circle of radius read_radius around the item's center.
};

// TODO: Implement reading errors
enum ReadError {
    AZTEC_NOT_FOUND,
};
enum ReadWarning {
    IMPRECISE_READING_RECTANGLE,
    TOO_MANY_EMPTY_CHOICES,
};

struct Item {
    // TODO: Change choice from char to enum
    char choice = '0';
    std::vector<float> choice_readings;
};

struct Reading {
    std::string answer_string = "";
    std::array<Vector2, 4> rectangle;
    std::vector<ReadError> reading_errors;
    std::vector<ReadWarning> reading_warnings;
    std::vector<Item> items = std::vector<Item>(20);  // questões
    // TODO: Headers
};

// TODO: ReadingBox with coordinates instead of hard-coded values (see reader.cpp)
struct ReadingBox {};

struct Reader {
public:
    ReadMode read_mode;

    int read_radius = 7;           // radius around the center of the item the reader will scan
    float area_threshold = 0.6f;   // threshold that defines if a choice is considered as marked
    float pixel_threshold = 0.4f;  // threshold that defines if a pixel is read as marked
    float choice_lerp_t = 0.625f;  // weight of filters in reading (filter1=0.0, filter2=1.0)
    float double_mark_threshold = 0.1f;  // difference between mark readings to count double mark

    int filter2_kernel_radius = 2;       // size of erode/dilate kernel used in filter 2

    Reading read(Image image);
    void draw_reading(Reading reading);
    void image_filter1(Image *image);
    void image_filter2(Image *image);
    void image_filter_hough(Image *image);
    float read_pixel(Image image, int x, int y);
    float read_area(Image image, int x, int y);
    std::array<Vector2, 4> get_reading_rectangle(Image image);
};