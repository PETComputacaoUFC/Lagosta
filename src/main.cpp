#include "imgtools/align.hpp"
#include "imgtools/filters.hpp"
#include "imgtools/imgtools.hpp"
#include "raylib.h"
#include "raymath.h"

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
    Clay_Context *clayContextTop = Clay_Initialize(
        clayMemoryTop,
        (Clay_Dimensions){.width = (float)GetScreenWidth(), .height = (float)GetScreenHeight()},
        (Clay_ErrorHandler){
            HandleClayErrors});  // This final argument is new since the video was published

    // ==== Resource importing ====
    Font fonts[1];
    fonts[0] = LoadFontEx("resources/fonts/NunitoSans-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Image img1 = LoadImage("resources/align_test/lines2.png");
    ImageFormat(&img1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    // ImageResize(&img1, 331, 233);
    ImageNormalizedGradient(&img1);
    ImageThreshold(&img1, 20);
    
    Range angle_range = {-90.0f, 90.0f, 1.0f};

    int diagonal = GetDiagonal(&img1);
    PixelVector pv = FilterImageThreshold(&img1, 255);

    HoughParameterSpace pspace(&pv, diagonal, angle_range, 1.0, 1.0);
    printf("width: %d, height: %d\n", pspace.width, pspace.height);
    printf("max: theta: %.2f, rho: %.2f, %d\n", pspace.max->theta, pspace.max->rho,
        pspace.max->count);
    Image pspace_img = pspace.image();
        
    ImageResize(&pspace_img, pspace_img.width*2, pspace_img.height);
    Texture texture = LoadTextureFromImage(img1);
    Texture texture2 = LoadTextureFromImage(pspace_img);

    Camera2D camera = {};
    camera.zoom = 1.25f;

    while (!WindowShouldClose()) {
        // ==== DRAW ====
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        
            DrawTexture(texture, 0, 0, WHITE);
            DrawTexture(texture2, texture.width, 0, WHITE);

        EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
