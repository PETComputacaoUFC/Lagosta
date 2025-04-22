#include "reader.hpp"
#include <array>

#include "imgtools/filters.hpp"
#include "imgtools/imgtools.hpp"
#include "raylib.h"
#include "raymath.h"

// base.png COORDS:
// rectangle:  63, 63  -->  1260, 869  |  (1197x806)
// q01a:  293, 520  |  q11a:  631, 520
// modalidade:  948, 520  |  fase:  977, 588
// horizontal spacing: 57  |  vertical spacing: 34
//
// relative values:
// q01a: 230, 457  |  q11a: 568, 457
// modalidade: 885, 457  |  fase:  914, 525
//
// as percentages of the rectangle (USE THESE VALUES IN Vector2Lerp):
// q01a:  0.192147034, 0.566997519  |  q11a:  0.474519632, 0.566997519
// modalidade:  0.7393448371, 0.566997519  |  fase:  0.763575606, 0.651364764
// horizontal spacing: 0.04735  |  vertical spacing: 0.042
const float Q01A_X = 0.192147034f, Q01A_Y = 0.566997519f;
const float Q11A_X = 0.474519632f, Q11A_Y = 0.566997519f;
// const float MODALIDADE_X = 0.7393448371f, MODALIDADE_Y = 0.566997519f;
// const float FASE_X = 0.763575606f, FASE_Y = 0.651364764f;
const float X_ITEM_SPACING = 0.04735f;
const float Y_ITEM_SPACING = 0.042f;

const char ITEMS_STR[6] = "abcde";

const int KERNEL_SIZE = 4;
const int READ_RADIUS = 7;
const float CHOICE_LERP_T = 0.625f;
const float DOUBLE_MARK_THRESHOLD = 0.1f;
const float PIXEL_THRESHOLD = 0.4f;  // threshold that defines if a pixel is read as marked
const float AREA_THRESHOLD = 0.6f;  // threshold that defines if a choice is considered as marked

void Reader::image_filter1(Image *image) {
    ImagePow(image, 1.5);
    ImageInvert(image);
}

void Reader::image_filter2(Image *image) {
    ImagePow(image, 1.5);
    ImageInvert(image);

    ImageThreshold(image, 60);
    ImageErode(image, KERNEL_SIZE);
    ImageDilate(image, KERNEL_SIZE);
}

Reading Reader::read(Image image) {
    Image grayscale = ImageCopy(image);
    ImageFormat(&grayscale, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    Image image_filtered1 = ImageCopy(grayscale);
    Image image_filtered2 = ImageCopy(grayscale);
    image_filter1(&image_filtered1);
    image_filter2(&image_filtered2);

    Reading reading {};
    reading.answer_string.clear();
    reading.items.clear();
    
    reading.rectangle = get_reading_rectangle(image);
    std::array<Vector2, 4> rectangle = reading.rectangle;

    for (int i = 0; i < 10; i++) {
        Item item = {-1, std::vector<float>(5)};
        float y_lerp_amount = Q01A_Y + Y_ITEM_SPACING * (float)i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q01A_X + X_ITEM_SPACING * (float)c;
            Vector2 v1 = Vector2Lerp(rectangle[0], rectangle[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(rectangle[2], rectangle[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            float reading1 = read_area(image_filtered1, center.x, center.y);
            float reading2 = read_area(image_filtered2, center.x, center.y);
            item.choice_readings[c] = lerp(reading1, reading2, CHOICE_LERP_T);
        }
        reading.items.push_back(item);
    }

    for (int i = 0; i < 10; i++) {
        Item item = {-1, std::vector<float>(5)};
        float y_lerp_amount = Q11A_Y + Y_ITEM_SPACING * (float)i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q11A_X + X_ITEM_SPACING * (float)c;
            Vector2 v1 = Vector2Lerp(rectangle[0], rectangle[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(rectangle[2], rectangle[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            float reading1 = read_area(image_filtered1, center.x, center.y);
            float reading2 = read_area(image_filtered2, center.x, center.y);
            item.choice_readings[c] = lerp(reading1, reading2, CHOICE_LERP_T);
        }
        reading.items.push_back(item);
    }

    for (Item& item : reading.items) {
        char choice_id = '0';
        size_t choice_index = 0;
        float choice_value = -1.0f;

        for (size_t choice = 0; choice < item.choice_readings.size(); choice++) {
            float reading = item.choice_readings[choice];
            if (reading > AREA_THRESHOLD && reading > choice_value) {
                choice_id = ITEMS_STR[choice];
                choice_index = choice;
                choice_value = reading;
            }
        }

        // anula a questão caso um item tenha mais de uma marcação
        for (size_t choice = 0; choice < item.choice_readings.size(); choice++) {
            if (choice != choice_index
                && abs(choice_value - item.choice_readings[choice]) <= DOUBLE_MARK_THRESHOLD) {
                choice_id = 'X';
                break;
            }
        }
        item.choice = choice_id;
        reading.answer_string.append(&item.choice);
    }
    return reading;
}

// Retorna o valor do pixel entre 0 e 1
float Reader::read_pixel(Image image, int x, int y) {
    int offset = x + image.width * y;
    return ((float)((uint8_t*)image.data)[offset]) / 255.0f;
}

float Reader::read_area(Image image, int x, int y) {
    Vector2 center = {(float)x, (float)y};
    float reading = 0.0f;
    float read_count = 0.0f;

    for (int r_x = -READ_RADIUS; r_x <= READ_RADIUS; r_x++) {
        for (int r_y = -READ_RADIUS; r_y <= READ_RADIUS; r_y++) {
            Vector2 read_coords = {(float)(x + r_x), (float)(y + r_y)};
            if (read_mode == SAMPLE_CIRCLE && Vector2Distance(center, read_coords) > READ_RADIUS) {
                continue;
            }

            read_count += 1.0f;
            float pixel = read_pixel(image, read_coords.x, read_coords.y);
            if (pixel >= PIXEL_THRESHOLD) {
                reading += pixel;
            }
        }
    }

    return reading / read_count;
}

void Reader::draw_reading(Reading reading) {
    std::array<Vector2, 4> rectangle = reading.rectangle;
    for (Vector2 corner : rectangle) {
        DrawCircleV(corner, 5.0f, RED);
    }
    
    for (int i = 0; i < 10; i++) {
        float y_lerp_amount = Q01A_Y + Y_ITEM_SPACING * (float) i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q01A_X + X_ITEM_SPACING * (float) c;
            Vector2 v1 = Vector2Lerp(rectangle[0], rectangle[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(rectangle[2], rectangle[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            char text[6];
            sprintf(text, "%.2f", reading.items[i].choice_readings[c]);
            DrawText(text, center.x, center.y, 20, YELLOW);
            DrawCircleV(center, 5.0f, reading.items[i].choice == ITEMS_STR[c] ? ORANGE : PURPLE);
        }
    }

    for (int i = 0; i < 10; i++) {
        float y_lerp_amount = Q11A_Y + Y_ITEM_SPACING * (float) i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q11A_X + X_ITEM_SPACING * (float) c;
            Vector2 v1 = Vector2Lerp(rectangle[0], rectangle[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(rectangle[2], rectangle[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            char text[6];
            sprintf(text, "%.2f", reading.items[i+10].choice_readings[c]);
            DrawText(text, center.x, center.y, 20, YELLOW);
            DrawCircleV(center, 5.0f, reading.items[i+10].choice == ITEMS_STR[c] ? ORANGE : PURPLE);
        }
    }
}

const Range THETA_RANGE_H = {-20.0f, 20.0f, 1.0f};
const Range THETA_RANGE_V1 = {-90.0f, -70.0f, 1.0f};
const Range THETA_RANGE_V2 = {70.0f, 90.0f, 1.0f};
const float RHO_STEP = 1.0f;

const int BLOCK_WIDTH = 116;
const int BLOCK_HEIGHT = 116;
const int BLOCK_X1 = 0, BLOCK_X2 = 1148;
const int BLOCK_Y1 = 0, BLOCK_Y2 = 749;

const Rectangle BLOCKS[4] = {
    {BLOCK_X1, BLOCK_Y1, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X2, BLOCK_Y1, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X1, BLOCK_Y2, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X2, BLOCK_Y2, BLOCK_WIDTH, BLOCK_HEIGHT},
};

std::array<Vector2, 4> Reader::get_reading_rectangle(Image image) {
    std::array<Vector2, 4> rectangle{};
    int block_counter = 0;
    for (Rectangle block_rect : BLOCKS) {
        Image block_img = ImageCopy(image);
        ImageCrop(&block_img, block_rect);

        ImageThreshold(&block_img, 90);
        ImageNormalizedGradient(&block_img);
        ImageThreshold(&block_img, 1);

        int diagonal = GetDiagonalLength(&block_img);
        PixelVector white_pixels = FilterImageThreshold(&block_img, 255);

        HoughParameterSpace pspace_h(&white_pixels, diagonal,
                                     THETA_RANGE_H, RHO_STEP, 0.5);
        HoughParameterSpace pspace_v1(&white_pixels, diagonal,
                                      THETA_RANGE_V1, RHO_STEP, 0.5);
        HoughParameterSpace pspace_v2(&white_pixels, diagonal,
                                      THETA_RANGE_V2, RHO_STEP, 0.5);
        
        Line max_h = *pspace_h.max;
        Line max_v1 = *pspace_v1.max;
        Line max_v2 = *pspace_v2.max;

        Line line1 = max_h;
        Line line2 = max_v1.count > max_v2.count ? max_v1 : max_v2;

        Vector2 intersection = IntersectionPoint(line1, line2);
        intersection.x += block_rect.x;
        intersection.y += block_rect.y;

        rectangle[block_counter] = intersection;
        
        block_counter++;
    }

    return rectangle;
}