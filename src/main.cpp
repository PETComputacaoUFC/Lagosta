#include "imgtools/filters.hpp"
#include "imgtools/imgtools.hpp"
#include "raylib.h"

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

    Image img1 = LoadImage("resources/align_test/align_test06.png");
    ImageFormat(&img1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    ImageThreshold(&img1, 90);
    ImageNormalizedGradient(&img1);
    ImageThreshold(&img1, 1);

    Image tl_block = ImageCopy(img1);
    Image tr_block = ImageCopy(img1);
    // Image bl_block = ImageCopy(img1);
    // Image br_block = ImageCopy(img1);

    Range angle_range_h = {-20.0f, 20.0f, 1.0f};
    Range angle_range_v1 = {-90.0f, -70.0f, 1.0f};
    Range angle_range_v2 = {70.0f, 90.0f, 1.0f};

    // TOP LEFT
    ImageCrop(&tl_block, Rectangle{0.0f, 0.0f, 116.0f, 116.0f,});
    int tl_diagonal = GetDiagonalLength(&tl_block);
    PixelVector tl_pv = FilterImageThreshold(&tl_block, 255);

    HoughParameterSpace tl_pspace_h(&tl_pv, tl_diagonal, angle_range_h, 1.0, 0.5);
    HoughParameterSpace tl_pspace_v1(&tl_pv, tl_diagonal, angle_range_v1, 1.0, 0.5);
    HoughParameterSpace tl_pspace_v2(&tl_pv, tl_diagonal, angle_range_v2, 1.0, 0.5);

    Line tl_max_h = *tl_pspace_h.max;
    Line tl_max_v1 = *tl_pspace_v1.max;
    Line tl_max_v2 = *tl_pspace_v2.max;

    Line tl_line1 = tl_max_h;
    Line tl_line2 = tl_max_v1.count > tl_max_v2.count ? tl_max_v1 : tl_max_v2;
    
    Vector2 tl_intersection = IntersectionPoint(tl_line1, tl_line2);

    // TOP RIGHT
    ImageCrop(&tr_block, Rectangle{1148.0f, 0.0f, 116.0f, 116.0f,});
    int tr_diagonal = GetDiagonalLength(&tr_block);
    PixelVector tr_pv = FilterImageThreshold(&tr_block, 255);

    HoughParameterSpace tr_pspace_h(&tr_pv, tr_diagonal, angle_range_h, 1.0, 0.5);
    HoughParameterSpace tr_pspace_v1(&tr_pv, tr_diagonal, angle_range_v1, 1.0, 0.5);
    HoughParameterSpace tr_pspace_v2(&tr_pv, tr_diagonal, angle_range_v2, 1.0, 0.5);

    Line tr_max_h = *tr_pspace_h.max;
    Line tr_max_v1 = *tr_pspace_v1.max;
    Line tr_max_v2 = *tr_pspace_v2.max;

    Line tr_line1 = tr_max_h;
    Line tr_line2 = tr_max_v1.count > tr_max_v2.count ? tr_max_v1 : tr_max_v2;
    
    Vector2 tr_intersection = IntersectionPoint(tr_line1, tr_line2) + Vector2(1148,0);

    // printf(" H | max: theta: %.2f, rho: %.2f, count: %d\n", max_h.theta, max_h.rho, max_h.count);
    // printf("V1 | max: theta: %.2f, rho: %.2f, count: %d\n", max_v1.theta, max_v1.rho, max_v1.count);
    // printf("V2 | max: theta: %.2f, rho: %.2f, count: %d\n", max_v2.theta, max_v2.rho, max_v2.count);
    // Image pspace_h_img = pspace_h.image();
    // Image pspace_v1_img = pspace_v1.image();
    // Image pspace_v2_img = pspace_v2.image();
    // ImageResize(&pspace_h_img, pspace_h_img.width * 2, pspace_h_img.height);
    // ImageResize(&pspace_v1_img, pspace_v1_img.width * 2, pspace_v1_img.height);
    // ImageResize(&pspace_v2_img, pspace_v2_img.width * 2, pspace_v2_img.height);
    // Texture texture2 = LoadTextureFromImage(pspace_h_img);
    // Texture texture3 = LoadTextureFromImage(pspace_v1_img);
    // Texture texture4 = LoadTextureFromImage(pspace_v2_img);
    
    Texture texture1 = LoadTextureFromImage(img1);

    Camera2D camera = {};
    camera.zoom = 0.8f;

    while (!WindowShouldClose()) {
        // ==== DRAW ====
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(texture1, 0, 0, WHITE);
        // DrawTexture(texture2, 256, 0, WHITE);
        // DrawTexture(texture3, 256 + texture2.width, 0, WHITE);
        // DrawTexture(texture4, 256 + texture2.width + texture3.width, 0, WHITE);

        DrawCircleV(tl_intersection, 5.0f, RED);
        DrawCircleV(tr_intersection, 5.0f, RED);

        EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
