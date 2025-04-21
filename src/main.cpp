#include "imgtools/align.hpp"
#include "imgtools/imgtools.hpp"
#include "raylib.h"
#include "raymath.h"

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylib.c"

// base.png COORDS:
// square:  63, 63  -->  1260, 869  |  (1197x806)
// q01a:  293, 520  |  q11a:  631, 520
// modalidade:  948, 520  |  fase:  977, 588
// horizontal spacing: 57  |  vertical spacing: 34
//
// relative values:
// q01a: 230, 457  |  q11a: 568, 457
// modalidade: 885, 457  |  fase:  914, 525
//
// as percentages of the square (USE THESE VALUES IN Vector2Lerp):
// q01a:  0.192147034, 0.566997519  |  q11a:  0.474519632, 0.566997519
// modalidade:  0.7393448371, 0.566997519  |  fase:  0.763575606, 0.651364764
// horizontal spacing: 0.04735  |  vertical spacing: 0.042
float circle_radius = 9.0f;
Vector2 circles[4] = {
    {100, 100},
    {400, 100},
    {100, 400},
    {400, 400},
};

const float Q01A_X = 0.192147034f, Q01A_Y = 0.566997519f;
const float Q11A_X = 0.474519632f, Q11A_Y = 0.566997519f;
const float MODALIDADE_X = 0.7393448371f, MODALIDADE_Y = 0.566997519f;
const float FASE_X = 0.763575606f, FASE_Y = 0.651364764f;
const float X_ITEM_SPACING = 0.04735f;
const float Y_ITEM_SPACING = 0.042f;

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    Clay_Raylib_Initialize(992, 699, "Lagosta", FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT); // Extra parameters to this function are new since the video was published
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemoryTop = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Context *clayContextTop = Clay_Initialize(clayMemoryTop, (Clay_Dimensions) {
       .width = (float) GetScreenWidth(),
       .height = (float) GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors }); // This final argument is new since the video was published

    // ==== Resource importing ====
    Font fonts[1];
    fonts[0] = LoadFontEx("resources/fonts/NunitoSans-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Image img_gabarito = LoadImage("resources/align_test/align_test02.png");
    ImageFormat(&img_gabarito, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    Image img_template = LoadImage("resources/align_test/template.png");
    ImageFormat(&img_template, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    Gradient g = Gradient(&img_gabarito);
    Image gradient_img = g.image();
    
    // Vector2 match_coords = ImageFindTemplate(&img_gabarito, &img_template);
    // printf("x%.2f y%.2f\n", match_coords.x, match_coords.y);
    // circles[0] = match_coords;

    Camera2D camera = {};
    camera.zoom = 0.75f;

    size_t dragging = -1;

    Texture2D texture = LoadTextureFromImage(gradient_img);
    while (!WindowShouldClose()) {
        // Drags the circle closest to the mouse
        bool mouse_down = IsMouseButtonDown(1);
        if (mouse_down) {
            Vector2 click_pos = GetScreenToWorld2D(GetMousePosition(), camera);
            if (dragging == (size_t) -1) {
                size_t closest_circle = 0;
                for (size_t i = 0; i < 4; i++) {
                    Vector2 circle_center = circles[i];
                    if (Vector2Distance(circle_center, click_pos) <= Vector2Distance(circles[closest_circle], click_pos)) {
                        closest_circle = i;
                    }
                }
                dragging = closest_circle;
            }
            circles[dragging] = click_pos;
        }
        else {
             dragging = -1;
        }

        if (IsMouseButtonPressed(0)) {
            Vector2 click_pos = GetScreenToWorld2D(GetMousePosition(), camera);
            printf("%d\n", ImageTestTemplate(&img_gabarito, &img_template, click_pos.x, click_pos.y));
        }

        // Prints box coords
        if (IsKeyPressed(KEY_P)) {
            printf("v0  x%.2f y%.2f | v1  x%.2f y%.2f\n", circles[0].x, circles[0].y, circles[1].x, circles[1].y);
            printf("v2  x%.2f y%.2f | v3  x%.2f y%.2f\n", circles[2].x, circles[2].y, circles[3].x, circles[3].y);
        }

        // ==== DRAW ====
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                DrawTexture(texture, 0, 0, WHITE);
                for (Vector2 circle_center: circles) {
                    DrawCircleV(circle_center, circle_radius, RED);
                }
            EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
