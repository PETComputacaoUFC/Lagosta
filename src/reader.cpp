#include "reader.hpp"
#include <cmath>
#include <cstdio>
#include <raylib.h>
#include <raymath.h>

// base.png COORDS:
// square:  63, 63  -->  1260, 869  |  (1197x806)
// q01a:  293, 520  |  q11a:  631, 520
// modalidade:  948, 520  |  fase:  977, 588
// horizontal spacing: 57  |  vertical spacing: 34
//
// relative values:
// q01a: 230, 457  |  q11a: 568, 457
// modalidade: 885, 457  |  fase:  914, 525
//
// as percentages of the square (USE THESE VALUES IN Vector2Lerp):
// q01a:  0.192147034, 0.566997519  |  q11a:  0.474519632, 0.566997519
// modalidade:  0.7393448371, 0.566997519  |  fase:  0.763575606, 0.651364764
// horizontal spacing: 0.04735  |  vertical spacing: 0.042
const float Q01A_X = 0.192147034f, Q01A_Y = 0.566997519f;
const float Q11A_X = 0.474519632f, Q11A_Y = 0.566997519f;
const float MODALIDADE_X = 0.7393448371f, MODALIDADE_Y = 0.566997519f;
const float FASE_X = 0.763575606f, FASE_Y = 0.651364764f;
const float X_ITEM_SPACING = 0.04735f;
const float Y_ITEM_SPACING = 0.042f;

Reader::Reader() : image(nullptr), read_mode(SAMPLE_CIRCLE), read_radius(4) {
    square[0] = { 63, 63 };
    square[1] = { 1260, 63 };
    square[2] = { 63, 869 };
    square[3] = { 1260, 869 };
}

Reader::Reader(Image* image, Vector2 square[4], ReadMode read_mode, int read_radius, float read_threshold, float avg_threshold) {
    this->image = image;
    this->read_mode = read_mode;
    this->read_radius = read_radius;
    this->read_threshold = read_threshold;
    this->avg_threshold = avg_threshold;
    for (int i = 0; i < 4; i++) { this->square[i] = square[i]; }
}

void Reader::read() {
    items.clear();

    for (int i = 0; i < 10; i++) {
        Item item = {-1, std::vector<float>(5)};
        float y_lerp_amount = Q01A_Y + Y_ITEM_SPACING * (float) i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q01A_X + X_ITEM_SPACING * (float) c;
            Vector2 v1 = Vector2Lerp(square[0], square[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(square[2], square[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            item.choice_readings[c] = read_area(center.x, center.y);
        }
        items.push_back(item);
    }

    for (int i = 0; i < 10; i++) {
        Item item = {-1, std::vector<float>(5)};
        float y_lerp_amount = Q11A_Y + Y_ITEM_SPACING * (float) i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q11A_X + X_ITEM_SPACING * (float) c;
            Vector2 v1 = Vector2Lerp(square[0], square[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(square[2], square[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            item.choice_readings[c] = read_area(center.x, center.y);
        }
        items.push_back(item);
    }

    // TODO: SET THRESHOLD
    // TODO: what if there are multiple selections in a question? just get the max reading? should we report that to the user?
    for (Item& item : items) {
        size_t choice_index = -1;
        float choice_value = -1.0f;

        for (size_t choice = 0; choice < item.choice_readings.size(); choice++) {
            float reading = item.choice_readings[choice];
            if (reading > 0.33f && reading > choice_value) {
                choice_index = choice;
                choice_value = reading;
            }
        }

        item.choice = choice_index;
    }
}

// Retorna o valor do pixel entre 0 e 1
float Reader::read_pixel(int x, int y) {
    int offset = x + image->width * y;
    return ( (float) ((unsigned char*) image->data)[offset] ) / 255.0f;
}

// TODO: should we use a thresholded image or is it better to use a grayscale one?
// I mean, mathematically if we're just using the threshold anyways it should be the same. But constructing an average
// could be better to avoid errors where the student accidentaly marked the wrong item and tried to fix it.
// It could also prevent error readings from smudges etc.
// In short, things that are more boldly marked could have an advantage on the reading.
float Reader::read_area(int x, int y) {
    const int NUM_SAMPLES = 100;

    Vector2 center = { (float) x, (float) y };
    float reading = 0.0f;
    float read_count = 0.0f;

    for (int r_x = -read_radius; r_x <= read_radius; r_x++) {
        for (int r_y = -read_radius; r_y <= read_radius; r_y++) {
            Vector2 read_coords = {(float) (x + r_x), (float) (y + r_y) };
            if (read_mode == SAMPLE_CIRCLE && Vector2Distance(center, read_coords) > read_radius ) {
                continue;
            }

            read_count += 1.0f;
            float pixel = read_pixel(read_coords.x, read_coords.y);
            if (pixel >= read_threshold) { reading += pixel; }
        }
    }

    return reading / read_count;
}