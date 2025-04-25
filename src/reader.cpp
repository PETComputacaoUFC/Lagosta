#include "reader.hpp"

#include <array>

#include "ZXing/ReadBarcode.h"
#include "ZXing/ReaderOptions.h"
#include "imgtools/filters.hpp"
#include "imgtools/imgtools.hpp"
#include "raylib.h"
#include "raymath.h"

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

// Pre-processing filter for applying the hough transform
void Reader::image_filter_hough(Image *image) {
    ImageKernelConvolutionFast(image, KERNEL_BOX_BLUR);  // Blur,
    ImageKernelConvolutionFast(image, KERNEL_LAPLACE);   // then sharpen!
    ImageThreshold(image, 90);                           // Filter for the darkest pixels
    ImageNormalizedGradient(image);                      // Edge detection
    ImageThreshold(image, 1);                            // Turns into a pure BW image
}

Reading Reader::read(Image image) {
    // we'll store warnings inside the reader's vector, then copie the
    // to the reading in the end.
    warnings.clear();
    image_filtered1 = ImageCopy(image);
    image_filter1(&image_filtered1);
    image_filtered2 = ImageCopy(image_filtered1);
    image_filter2(&image_filtered2);

    Reading reading{};
    reading.items.reserve(20);
    reading.answer_string.clear();
    reading.items.clear();

    reading_rectangle = get_reading_rectangle(image);
    reading.reading_rectangle = reading_rectangle;

    /* ==== READING BARCODE ==== */
    unsigned char *img_data = (unsigned char *)image.data;
    ZXing::ImageView barcode_image_view(img_data, image.width, image.height,
                                        ZXing::ImageFormat::Lum);
    ZXing::ReaderOptions options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::Aztec);
    ZXing::Barcode barcode = ZXing::ReadBarcode(barcode_image_view, options);
    reading.barcode_string = barcode.text();
    if (reading.barcode_string.empty()) { warnings.push_back(BARCODE_NOT_FOUND); }

    /* ==== READING ITEMS ==== */
    for (ItemGroup ig : reading_box.item_groups) {
        for (int i = 0; i < ig.num_items; i++) {
            Item item = {-1, std::vector<float>(ig.num_choices)};

            float y_lerp_amount = ig.item01a_y + ig.item_spacing_y * (float)i;
            for (int c = 0; c < ig.num_choices; c++) {
                float x_lerp_amount = ig.item01a_x + ig.item_spacing_x * (float)c;
                Vector2 v1 = Vector2Lerp(reading_rectangle[0], reading_rectangle[1], x_lerp_amount);
                Vector2 v2 = Vector2Lerp(reading_rectangle[2], reading_rectangle[3], x_lerp_amount);

                Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
                float reading1 = read_area(image_filtered1, center.x, center.y);
                float reading2 = read_area(image_filtered2, center.x, center.y);
                item.choice_readings[c] = Lerp(reading1, reading2, choice_lerp_t);
            }

            char choice_id = '0';
            float choice_value = -1.0f;
            float second_highest = -1.0f;

            for (int choice = 0; choice < ig.num_choices; choice++) {
                float reading = item.choice_readings[choice];
                if (reading > area_threshold && reading > choice_value) {
                    choice_id = ITEMS_STR[choice];
                    second_highest = choice_value;
                    choice_value = reading;
                } else if (reading > second_highest) {
                    second_highest = reading;
                }
            }

            // Invalida a questão se os dois itens mais altos tem valores muito próximos.
            if (abs(choice_value - second_highest) <= double_mark_threshold) { choice_id = 'X'; }

            item.choice = choice_id;
            reading.answer_string.append(&item.choice);
            reading.items.push_back(item);
        }
    }

    // this actually copies the vector, which is what we want.
    reading.warnings = warnings;
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
    std::array<Vector2, 4> reading_rectangle = reading.reading_rectangle;
    for (Vector2 corner : reading_rectangle) { DrawCircleV(corner, 5.0f, RED); }

    int item_counter = 0;
    for (ItemGroup ig : reading_box.item_groups) {
        for (int i = 0; i < ig.num_items; i++) {
            Item item = {-1, std::vector<float>(ig.num_choices)};

            float y_lerp_amount = ig.item01a_y + ig.item_spacing_y * (float)i;
            for (int c = 0; c < ig.num_choices; c++) {
                float x_lerp_amount = ig.item01a_x + ig.item_spacing_x * (float)c;
                Vector2 v1 = Vector2Lerp(reading_rectangle[0], reading_rectangle[1], x_lerp_amount);
                Vector2 v2 = Vector2Lerp(reading_rectangle[2], reading_rectangle[3], x_lerp_amount);

                Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
                char text[6];
                sprintf(text, "%.2f", reading.items[item_counter].choice_readings[c]);
                DrawText(text, center.x, center.y, 20, YELLOW);
                DrawCircleV(
                    center, read_radius,
                    reading.items[item_counter].choice == ITEMS_STR[c] ? ORANGE_T : PURPLE_T);
            }
            item_counter++;
        }
    }
}

const Range THETA_RANGE_H = {-10.0f, 10.0f, 1.0f};
const Range THETA_RANGE_V1 = {-90.0f, -80.0f, 1.0f};
const Range THETA_RANGE_V2 = {80.0f, 90.0f, 1.0f};
const float RHO_STEP = 1.0f;
const float HOUGH_THRESHOLD = 0.5f;

// TODO: There might be a better way of finding the reading rectangle;
// grouping pixels and finding the one group closest to the corner, etc.
// Also, maybe detecting 3 corners and finding the last one based on angles...
std::array<Vector2, 4> Reader::get_reading_rectangle(Image image) {
    std::array<Vector2, 4> rectangle{};

    int block_counter = 0;
    bool imprecise = false;  // mark if there was an imprecision finding the alignment corner
    for (Rectangle block_rect : reading_box.get_block_rectangles()) {
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

        Line line_h = max_h;
        Line line_v = max_v1.count > max_v2.count ? max_v1 : max_v2;

        // our threshold for imprecision is 5 countings or less.
        if (line_h.count <= 5 || line_v.count <= 5) { imprecise = true; }
        printf("%d > line_h: %d | line_v: %d\n", block_counter, line_h.count, line_v.count);

        Vector2 intersection = IntersectionPoint(line_h, line_v);
        intersection.x += block_rect.x;
        intersection.y += block_rect.y;

        rectangle[block_counter] = intersection;

        block_counter++;
    }

    if (imprecise) { warnings.push_back(IMPRECISE_READING_RECTANGLE); }

    return rectangle;
}