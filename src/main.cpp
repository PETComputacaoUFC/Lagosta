#include <algorithm>
#include <cstdio>
#include <string>

#include "raylib.h"
#include "reader.hpp"
#include "scanner.hpp"  // TODO: if in Windows, don't use SANE
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylib.c"


void HandleClayErrors(Clay_ErrorData errorData) { printf("%s", errorData.errorText.chars); }


int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    Clay_Raylib_Initialize(
        1012, 720, "Lagosta",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemoryTop =
        Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Initialize(
        clayMemoryTop,
        (Clay_Dimensions){.width = (float)GetScreenWidth(), .height = (float)GetScreenHeight()},
        (Clay_ErrorHandler){HandleClayErrors});

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
            std::string img_path = image_paths[path_index];
            printf("\nFILE: %s\n", img_path.c_str());
            Image img_gabarito = LoadImage(img_path.c_str());

            /* ==== LEITURA GABARITO ==== */
            reading = reader.read(img_gabarito);
            printf("  > Gabarito: %s\n", reading.answer_string.c_str());
            printf("  > Aztec: %s\n", reading.barcode_string.c_str());

            reader.image_filter1(&img_gabarito);
            texture = LoadTextureFromImage(img_gabarito);
        }

        /* ==== DRAWING ==== */
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
