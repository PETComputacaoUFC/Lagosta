#include "reader.hpp"
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

Reader::Reader() : image(nullptr), read_mode(RANDOM_SAMPLE), read_radius(4) {
    square[0] = { 63, 63 };
    square[1] = { 1260, 63 };
    square[2] = { 63, 869 };
    square[3] = { 1260, 869 };
}

Reader::Reader(Image* image, Vector2 square[4], ReadMode read_mode, int read_radius) {
    this->image = image;
    this->read_mode = read_mode;
    this->read_radius = read_radius;
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
            item.choice_reading[c] = read_area(center.x, center.y);
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
            item.choice_reading[c] = read_area(center.x, center.y);
        }
        items.push_back(item);
    }

    // TODO: SET THRESHOLD
    // TODO: what if there are multiple selections in a question? just get the max reading? should we report that to the user?
    for (Item& item : items) {
        size_t max_index = -1;
        float max_value = -1.0f;

        for (size_t choice = 0; choice < item.choice_reading.size(); choice++) {
            float reading = item.choice_reading[choice];
            if (reading > 0.4f && reading > max_value) {
                max_index = choice;
                max_value = reading;
            }
        }

        item.choice = max_index;
    }
}

unsigned char Reader::read_pixel(int x, int y) {
    int offset = x + image->width * y;
    return ((unsigned char*) image->data)[offset];
}

// TODO: should we use a thresholded image or is it better to use a grayscale one?
// I mean, mathematically if we're just using the threshold anyways it should be the same. But constructing an average
// could be better to avoid errors where the student accidentaly marked the wrong item but tried to fix it.
// i.e. things that are more boldly marked could have an advantage on the reading.
float Reader::read_area(int x, int y) {
    switch (read_mode) {
        case RANDOM_SAMPLE: {
            float read_count = 0.0f;
            for (int i = 0; i < 100; i++) {
                int r_x = GetRandomValue(-read_radius, read_radius);
                int r_y = GetRandomValue(-read_radius, read_radius);
                if (read_pixel(x + r_x, y + r_y) == 0) {
                    read_count += 1.0f;
                }
            }
            return read_count / 20.0f;
        }
        case SAMPLE_CIRCLE: {
            Vector2 v1 = { (float) x, (float) y };
            float read_count = 0.0f;
            float pixel_count = 0.0f;
            for (int o_x = -read_radius; o_x <= read_radius; o_x++) {
                for (int o_y = -read_radius; o_y <= read_radius; o_y++) {
                    Vector2 v2 = { (float) x + o_x, (float) y + o_y };
                    if (Vector2Distance( v1, v2 ) <= (float) read_radius) {
                        pixel_count += 1.0f;
                        if (read_pixel(x + o_x, y + o_y) == 0) {
                            read_count += 1.0f;
                        }
                    }
                }
            }
            return read_count / pixel_count;
        }
        case SAMPLE_SQUARE: {
            float read_count = 0.0f;
            float pixel_count = 0.0f;
            for (int o_x = -read_radius; o_x <= read_radius; o_x++) {
                for (int o_y = -read_radius; o_y <= read_radius; o_y++) {
                    pixel_count += 1.0f;
                    if (read_pixel(x + o_x, y + o_y) == 0) {
                        read_count += 1.0f;
                    }
                }
            }
            return read_count / pixel_count;
        }
    }
    return 0.0f;
}