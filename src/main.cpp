#ifndef WINDOWS
#include <sane/sane.h>
#endif

#include <algorithm>
#include <cstdio>
#include <string>

#include "raylib.h"
#include "reader.hpp"
#include "scanner.hpp"
#define CLAY_IMPLEMENTATION
#include "ZXing/ReadBarcode.h"
#include "clay.h"
#include "clay_renderer_raylib.c"


void HandleClayErrors(Clay_ErrorData errorData) { printf("%s", errorData.errorText.chars); }

// 1144,774
// 120x90
int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    Clay_Raylib_Initialize(1012, 720, "Lagosta",
                           FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT
                               | FLAG_VSYNC_HINT);  // Extra parameters to this function are new
                                                    // since the video was published
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemoryTop =
        Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Initialize(
        clayMemoryTop,
        (Clay_Dimensions){.width = (float)GetScreenWidth(), .height = (float)GetScreenHeight()},
        (Clay_ErrorHandler){
            HandleClayErrors});  // This final argument is new since the video was published

    // Setting default clay font
    Font fonts[1];
    fonts[0] = LoadFontEx("resources/fonts/NunitoSans-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    // Inicializando leitura interativa do gabarito
    Reader reader{};

    FilePathList pathlist = LoadDirectoryFiles("resources/scans_teste_oci");
    std::vector<std::string> image_paths;
    size_t path_index = 0;
    for (size_t p = 0; p < pathlist.count; p++) { image_paths.push_back(pathlist.paths[p]); }
    std::sort(image_paths.begin(), image_paths.end());
    Texture texture;
    Reading reading;

    Camera2D camera = {};
    camera.zoom = 0.8f;


    /* ==== LEITURA GABARITO ==== */
    Image img = LoadImage("resources/scans_teste_oci/out0007.png");

    reading = reader.read(img);
    printf("  > Gabarito: %s\n", reading.answer_string.c_str());


    /* ==== LEITURA AZTEC CODE ==== */
    unsigned char* img_data = (unsigned char*)img.data;
    auto image = ZXing::ImageView(img_data, img.width, img.height, ZXing::ImageFormat::Lum);
    auto options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::Aztec);
    auto barcode = ZXing::ReadBarcode(image, options);
    printf("  > Aztec: %s\n", barcode.text().c_str());

    reader.image_filter_hough(&img);
    texture = LoadTextureFromImage(img);

    bool update_reading = false;
    while (!WindowShouldClose()) {
        /* ==== UPDATE ==== */
        if (IsKeyPressed(KEY_LEFT)) {
            if (path_index > 0) {
                path_index -= 1;
                update_reading = true;
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            if (path_index < image_paths.size() - 1) {
                path_index += 1;
                update_reading = true;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            /* ==== SCAN DE IMAGEM ==== */
            SANE_Int sane_version;
            sane_init(&sane_version, nullptr);
            SANE_Handle handler = GetScanner("000000000YP76T4DPR1a");
            Image img_gabarito = ImageFromScanner(handler);
            sane_exit();

            // ExportImage(img, "scan.png");

            /* ==== LEITURA GABARITO ==== */
            reading = reader.read(img_gabarito);
            printf("  > Gabarito: %s\n", reading.answer_string.c_str());

            /* ==== LEITURA AZTEC CODE ==== */
            ImageFormat(&img_gabarito, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
            unsigned char* img_data = (unsigned char*)img_gabarito.data;
            auto image = ZXing::ImageView(img_data, img_gabarito.width, img_gabarito.height,
                                            ZXing::ImageFormat::Lum);
            auto options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::Aztec);
            auto barcode = ZXing::ReadBarcode(image, options);
            printf("  > Aztec: %s\n", barcode.text().c_str());

            reader.image_filter_hough(&img_gabarito);
            texture = LoadTextureFromImage(img_gabarito);
        }

        if (update_reading) {
            update_reading = false;

            /* ==== LEITURA GABARITO ==== */
            std::string img_path = image_paths[path_index];
            printf("\nFILE: %s\n", img_path.c_str());
            Image img_gabarito = LoadImage(img_path.c_str());
            reading = reader.read(img_gabarito);
            printf("  > Gabarito: %s\n", reading.answer_string.c_str());

            /* ==== LEITURA AZTEC CODE ==== */
            ImageFormat(&img_gabarito, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
            unsigned char* img_data = (unsigned char*)img_gabarito.data;
            auto image = ZXing::ImageView(img_data, img_gabarito.width, img_gabarito.height,
                                          ZXing::ImageFormat::Lum);
            auto options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::Aztec);
            auto barcode = ZXing::ReadBarcode(image, options);
            printf("  > Aztec: %s\n", barcode.text().c_str());

            reader.image_filter_hough(&img_gabarito);
            texture = LoadTextureFromImage(img_gabarito);
        }
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(texture, 0, 0, WHITE);
        reader.draw_reading(reading);

        EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
