#pragma once

#include <array>
#include <string>
#include <vector>

#include "raylib.h"

enum ReadMode {
    SAMPLE_SQUARE,  // Averages a square with size 2*read_radius around the item's center.
    SAMPLE_CIRCLE,  // Averages a circle of radius read_radius around the item's center.
};

enum ReadWarning {
    BARCODE_NOT_FOUND,
    IMPRECISE_READING_RECTANGLE,
    TOO_MANY_NULL_CHOICES,
};




struct Item {
    char choice = '-';
    std::vector<float> choice_readings;
};

struct Reading {
    std::string barcode_string = "";
    std::array<Vector2, 4> reading_rectangle;
    std::vector<ReadWarning> warnings;
    std::vector<Item> items;    // questões
    std::vector<Item> headers;  // cabeçalho
    inline std::string get_answer_string() {
        std::string answer_string;
        for (Item item : items) { answer_string.push_back(item.choice); }
        answer_string.push_back('|');
        for (Item header : headers) { answer_string.push_back(header.choice); }
        return answer_string;
    };
};




struct ItemGroup {
    float item01a_x, item01a_y;
    float item_spacing_x, item_spacing_y;
    int num_items, num_choices;
};

struct ReadingBox {
    std::vector<ItemGroup> item_groups;
    std::vector<ItemGroup> header_groups;
    float barcode_x, barcode_y;
    float barcode_width, barcode_height;
    int align_block_x1, align_block_x2;
    int align_block_y1, align_block_y2;
    int align_block_width, align_block_height;

    // Retorna os retângulos dos blocos de alinhamento.
    // ORDEM: top-left, top-right, bottom-left, bottom-right
    inline constexpr std::array<Rectangle, 4> get_block_rectangles() {
        float x1 = (float)align_block_x1, x2 = (float)align_block_x2;
        float y1 = (float)align_block_y1, y2 = (float)align_block_y2;
        float width = (float)align_block_width, height = (float)align_block_height;

        return {Rectangle{x1, y1, width, height}, Rectangle{x2, y1, width, height},
                Rectangle{x1, y2, width, height}, Rectangle{x2, y2, width, height}};
    };
};


// clang-format off
// Valores calculados de forma relativa usando uma imagem 1323x932 do gabarito oficial
// como base, levando em conta que a área lida pelo leitor é a área interna demarcada
// pelos marcadores de alinhamento.
const ReadingBox OCI_READING_BOX = {
    .item_groups={
        // Itens 01 a 10
        {.item01a_x = 0.193147034f, .item01a_y = 0.563997519f,
            .item_spacing_x = 0.04735f, .item_spacing_y = 0.042f,
            .num_items = 10, .num_choices = 5},
        // Itens 11 a 20
        {.item01a_x = 0.475519632f, .item01a_y = 0.563997519f,
            .item_spacing_x = 0.04735f, .item_spacing_y = 0.042f,
            .num_items = 10, .num_choices = 5}
    },
    .header_groups={
        // Gabarito "Modalidade"
        {.item01a_x = 0.7393448371f, .item01a_y = 0.563997519f,
            .item_spacing_x = 0.04735f, .item_spacing_y = 0.042f,
            .num_items = 1, .num_choices = 3}
    },
    .barcode_x = 0.082706767f, .barcode_y = 0.248138958f,
    .barcode_width = 0.171261487f, .barcode_height = 0.254342432f,
    // Valores ótimos calculados usando scans teste de uma folha A4 a 150DPI
    // com o gabarito, que tem proporção A5, na metade de cima da folha.
    .align_block_x1 = 0, .align_block_x2 = 1144,
    .align_block_y1 = 0, .align_block_y2 = 774,
    .align_block_width = 120, .align_block_height = 90
};
// clang-format on


struct Reader {
    ReadMode read_mode = SAMPLE_CIRCLE;
    ReadingBox reading_box = OCI_READING_BOX;

    int read_radius = 7;           // radius around the center of the item the reader will scan
    float area_threshold = 0.5f;   // threshold that defines if a choice is considered as marked
    float pixel_threshold = 0.4f;  // threshold that defines if a pixel is read as marked
    float choice_lerp_t = 0.625f;  // weight of filters in reading (filter1=0.0, filter2=1.0)
    float double_mark_threshold = 0.1f;  // difference between mark readings to count double mark

    int filter2_kernel_radius = 2;       // size of erode/dilate kernel used in filter 2

    Reading read(Image image);
    void draw_reading(Reading reading);
    void image_filter1(Image *image);
    void image_filter2(Image *image);
    void image_filter_hough(Image *image);
    float read_area(Image image, int x, int y);
    std::vector<Item> read_item_group(ItemGroup item_group,
                                      std::array<Vector2, 4> reading_rectangle,
                                      Image image_filtered1, Image image_filtered2);
    std::array<Vector2, 4> get_reading_rectangle(Image image, std::vector<ReadWarning> *warnings);
};