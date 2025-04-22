#include "raylib.h"
#include "reader.hpp"

#define CLAY_IMPLEMENTATION
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

    // ==== Resource importing ====
    Font fonts[1];
    fonts[0] = LoadFontEx("resources/fonts/NunitoSans-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Image img1 = LoadImage("resources/scans_teste_oci/out0009.png");
    ImageFormat(&img1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    
    Reader reader(&img1, SAMPLE_CIRCLE);
    std::string answer = reader.read();
    printf("%s\n", answer.c_str());
    
    Texture texture1 = LoadTextureFromImage(reader.image_filtered1);

    Camera2D camera = {};
    camera.zoom = 0.8f;

    while (!WindowShouldClose()) {
        // ==== DRAW ====
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(texture1, 0, 0, WHITE);
        reader.draw_reading();

        EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
