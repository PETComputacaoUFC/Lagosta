#include <algorithm>
#include <cstdio>
#include <string>

#include "imgtools/filters.hpp"
#include "raylib.h"
#include "reader.hpp"
#define CLAY_IMPLEMENTATION
#include "ZXing/ReadBarcode.h"
#include "clay.h"
#include "clay_renderer_raylib.c"


void HandleClayErrors(Clay_ErrorData errorData) { printf("%s", errorData.errorText.chars); }

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    Clay_Raylib_Initialize(992, 699, "Lagosta",
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
    for (size_t p = 0; p < pathlist.count; p++) { image_paths.push_back(pathlist.paths[p]); }
    std::sort(image_paths.begin(), image_paths.end());
    size_t path_index = 10;
    Texture texture;
    Reading reading;

    Camera2D camera = {};
    camera.zoom = 0.8f;

    bool update_reading = true;
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

            reader.image_filter1(&img_gabarito);
            // ImageThreshold(&img_gabarito, 90);
            // ImageNormalizedGradient(&img_gabarito);
            // ImageThreshold(&img_gabarito, 1);
            // ImageDilate(&img_gabarito, 1);

            texture = LoadTextureFromImage(img_gabarito);
        }

        /* ==== DRAW ==== */
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
