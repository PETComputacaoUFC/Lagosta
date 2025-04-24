#include "reader.hpp"

#include <array>

#include "ZXing/ReadBarcode.h"
#include "ZXing/ReaderOptions.h"
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
const float Q01A_X = 0.193147034f, Q01A_Y = 0.563997519f;
const float Q11A_X = 0.475519632f, Q11A_Y = 0.563997519f;
// const float MODALIDADE_X = 0.7393448371f, MODALIDADE_Y = 0.566997519f;
const float X_ITEM_SPACING = 0.04735f;
const float Y_ITEM_SPACING = 0.042f;

const char ITEMS_STR[6] = "abcde";

void Reader::image_filter1(Image *image) {
    ImageFormat(image, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    ImagePow(image, 1.5);
    ImageColorInvertFast(image);
}

void Reader::image_filter2(Image *image) {
    ImageThreshold(image, 60);
    ImageErode(image, filter2_kernel_radius);
    ImageDilate(image, filter2_kernel_radius);
}

Reading Reader::read(Image image) {
    Image image_filtered1 = ImageCopy(image);
    image_filter1(&image_filtered1);
    Image image_filtered2 = ImageCopy(image_filtered1);
    image_filter2(&image_filtered2);

    Reading reading{};
    reading.items.reserve(20);
    reading.answer_string.clear();
    reading.items.clear();

    std::array<Vector2, 4> rectangle = get_reading_rectangle(image);
    reading.rectangle = rectangle;

    /* ==== READING BARCODE ==== */
    unsigned char *img_data = (unsigned char *)image.data;
    ZXing::ImageView barcode_image_view(img_data, image.width, image.height,
                                        ZXing::ImageFormat::Lum);
    ZXing::ReaderOptions options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::Aztec);
    ZXing::Barcode barcode = ZXing::ReadBarcode(barcode_image_view, options);
    reading.barcode_string = barcode.text();
    if (reading.barcode_string.empty()) { reading.warnings.push_back(BARCODE_NOT_FOUND); }

    /* ==== READING ITEMS ==== */
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
            item.choice_readings[c] = Lerp(reading1, reading2, choice_lerp_t);
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
            item.choice_readings[c] = Lerp(reading1, reading2, choice_lerp_t);
        }
        reading.items.push_back(item);
    }

    for (Item &item : reading.items) {
        char choice_id = '0';
        size_t choice_index = 0;
        float choice_value = -1.0f;

        for (size_t choice = 0; choice < item.choice_readings.size(); choice++) {
            float reading = item.choice_readings[choice];
            if (reading > area_threshold && reading > choice_value) {
                choice_id = ITEMS_STR[choice];
                choice_index = choice;
                choice_value = reading;
            }
        }

        // anula a questão caso um item tenha mais de uma marcação
        for (size_t choice = 0; choice < item.choice_readings.size(); choice++) {
            if (choice != choice_index
                && abs(choice_value - item.choice_readings[choice]) <= double_mark_threshold) {
                choice_id = 'X';
                break;
            }
        }
        item.choice = choice_id;
        reading.answer_string.append(&item.choice);
    }

    UnloadImage(image_filtered1);
    UnloadImage(image_filtered2);
    return reading;
}

// Retorna o valor do pixel entre 0 e 1
float Reader::read_pixel(Image image, int x, int y) {
    int offset = x + image.width * y;
    return ((float)((uint8_t *)image.data)[offset]) / 255.0f;
}

// Retorna porcentagem dos pixels de uma área que estão acima do threshold
float Reader::read_area(Image image, int x, int y) {
    Vector2 center = {(float)x, (float)y};
    float reading = 0.0f;
    float read_count = 0.0f;

    for (int r_x = -read_radius; r_x <= read_radius; r_x++) {
        for (int r_y = -read_radius; r_y <= read_radius; r_y++) {
            Vector2 read_coords = {(float)(x + r_x), (float)(y + r_y)};
            if (read_mode == SAMPLE_CIRCLE && Vector2Distance(center, read_coords) > read_radius) {
                continue;
            }

            read_count += 1.0f;
            float pixel = read_pixel(image, read_coords.x, read_coords.y);
            if (pixel >= pixel_threshold) { reading += pixel; }
        }
    }

    return reading / read_count;
}

#define ORANGE_T CLITERAL(Color){255, 161, 0, 128}    // Orange
#define PURPLE_T CLITERAL(Color){200, 122, 255, 128}  // Purple
// Desenha o output de uma leitura na teal
void Reader::draw_reading(Reading reading) {
    std::array<Vector2, 4> rectangle = reading.rectangle;
    for (Vector2 corner : rectangle) { DrawCircleV(corner, 5.0f, RED); }

    for (int i = 0; i < 10; i++) {
        float y_lerp_amount = Q01A_Y + Y_ITEM_SPACING * (float)i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q01A_X + X_ITEM_SPACING * (float)c;
            Vector2 v1 = Vector2Lerp(rectangle[0], rectangle[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(rectangle[2], rectangle[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            char text[6];
            sprintf(text, "%.2f", reading.items[i].choice_readings[c]);
            DrawText(text, center.x, center.y, 20, YELLOW);
            DrawCircleV(center, read_radius,
                        reading.items[i].choice == ITEMS_STR[c] ? ORANGE_T : PURPLE_T);
        }
    }

    for (int i = 0; i < 10; i++) {
        float y_lerp_amount = Q11A_Y + Y_ITEM_SPACING * (float)i;
        for (int c = 0; c < 5; c++) {
            float x_lerp_amount = Q11A_X + X_ITEM_SPACING * (float)c;
            Vector2 v1 = Vector2Lerp(rectangle[0], rectangle[1], x_lerp_amount);
            Vector2 v2 = Vector2Lerp(rectangle[2], rectangle[3], x_lerp_amount);

            Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
            char text[6];
            sprintf(text, "%.2f", reading.items[i + 10].choice_readings[c]);
            DrawText(text, center.x, center.y, 20, YELLOW);
            DrawCircleV(center, read_radius,
                        reading.items[i + 10].choice == ITEMS_STR[c] ? ORANGE_T : PURPLE_T);
        }
    }
}

const Range THETA_RANGE_H = {-10.0f, 10.0f, 1.0f};
const Range THETA_RANGE_V1 = {-90.0f, -80.0f, 1.0f};
const Range THETA_RANGE_V2 = {80.0f, 90.0f, 1.0f};
const float RHO_STEP = 1.0f;
const float HOUGH_THRESHOLD = 0.5f;

const int BLOCK_WIDTH = 120;
const int BLOCK_HEIGHT = 90;
const int BLOCK_X1 = 0, BLOCK_X2 = 1144;
const int BLOCK_Y1 = 0, BLOCK_Y2 = 774;

const Rectangle BLOCKS[4] = {
    {BLOCK_X1, BLOCK_Y1, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X2, BLOCK_Y1, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X1, BLOCK_Y2, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X2, BLOCK_Y2, BLOCK_WIDTH, BLOCK_HEIGHT},
};

void Reader::image_filter_hough(Image *image) {
    ImageKernelConvolutionFast(image, KERNEL_BOX_BLUR);  // Blur,
    ImageKernelConvolutionFast(image, KERNEL_LAPLACE);   // then sharpen!
    ImageThreshold(image, 90);                           // Filter for the darkest pixels
    ImageNormalizedGradient(image);                      // Edge detection
    ImageThreshold(image, 1);                            // Turns into a pure BW image
}

// TODO: There might be a better way of finding the reading rectangle;
// grouping pixels and finding the one group closest to the corner, etc.
// Also, maybe detecting 3 corners and finding the last one based on angles...
std::array<Vector2, 4> Reader::get_reading_rectangle(Image image) {
    std::array<Vector2, 4> rectangle{};

    int block_counter = 0;
    for (Rectangle block_rect : BLOCKS) {
        Image block_img = ImageCopy(image);
        ImageCrop(&block_img, block_rect);

        image_filter_hough(&block_img);

        int diagonal = GetDiagonalLength(block_img);
        PixelVector white_pixels = FilterImageThreshold(block_img, 255);

        HoughParameterSpace pspace_h(white_pixels, diagonal, THETA_RANGE_H, RHO_STEP,
                                     HOUGH_THRESHOLD);
        HoughParameterSpace pspace_v1(white_pixels, diagonal, THETA_RANGE_V1, RHO_STEP,
                                      HOUGH_THRESHOLD);
        HoughParameterSpace pspace_v2(white_pixels, diagonal, THETA_RANGE_V2, RHO_STEP,
                                      HOUGH_THRESHOLD);

        Line max_h = *pspace_h.max;
        Line max_v1 = *pspace_v1.max;
        Line max_v2 = *pspace_v2.max;

        Line line1 = max_h;
        Line line2 = max_v1.count > max_v2.count ? max_v1 : max_v2;

        printf("%d > line1: %d | line2: %d\n", block_counter, line1.count, line2.count);

        Vector2 intersection = IntersectionPoint(line1, line2);
        intersection.x += block_rect.x;
        intersection.y += block_rect.y;

        rectangle[block_counter] = intersection;

        block_counter++;
    }

    return rectangle;
}