#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <vector>

#define CLAY_IMPLEMENTATION
#include "../include/clay.h"
#include "../include/clay_renderer_raylib.c"
#include "filters.hpp"

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

bool PointInCircle(Vector2 p, Vector2 circle_center, float circle_radius) {
    float distance = Vector2Distance(p, circle_center);
    return distance <= circle_radius;
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

    Image img_gabarito = LoadImage("resources/scans_teste_oci/out0002.png");
    // ImageColorContrast(&img_gabarito, 100);
    ImageThreshold(&img_gabarito, 210);
    ImageDilate(&img_gabarito, 5);
    ImageErode(&img_gabarito, 5);
    // ImageInvert(&img_gabarito);
    Texture2D text_gabarito = LoadTextureFromImage(img_gabarito);

    Camera2D camera = { };
    camera.zoom = 0.75f;

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
    std::vector<Vector2> circles = {
        { 63, 63 },
        { 1260, 63 },
        { 63, 869 },
        { 1260, 869 },
    };
   
    const float Q01A_X = 0.192147034f, Q01A_Y = 0.566997519f;
    const float Q11A_X = 0.474519632f, Q11A_Y = 0.566997519f;
    const float MODALIDADE_X = 0.7393448371f, MODALIDADE_Y = 0.566997519f;
    const float FASE_X = 0.763575606f, FASE_Y = 0.651364764f;
    const float X_ITEM_SPACING = 0.04735f;
    const float Y_ITEM_SPACING = 0.042f;

    size_t dragging = -1;
    while (!WindowShouldClose()) {
        // ==== UPDATE ====

        // Drags the circle closest to the mouse
        bool mouse_down = IsMouseButtonDown(1);
        if (mouse_down) {
            Vector2 click_pos = GetScreenToWorld2D(GetMousePosition(), camera);
            if (dragging == (size_t) -1) {
                size_t closest_circle = 0;
                for (size_t i = 0; i < circles.size(); i++) {
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

        
        if (IsKeyPressed(KEY_P)) {
            printf("v0  x%.2f y%.2f | v1  x%.2f y%.2f\n", circles[0].x, circles[0].y, circles[1].x, circles[1].y);
            printf("v2  x%.2f y%.2f | v3  x%.2f y%.2f\n", circles[2].x, circles[2].y, circles[3].x, circles[3].y);
        }

        // ==== DRAW ====
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                DrawTexture(text_gabarito, 0, 0, WHITE);
                for (Vector2 circle_center: circles) {
                    DrawCircleV(circle_center, circle_radius, RED);
                }

                for (int i = 0; i < 5; i++) {
                    float l_x_lerp_amount = Q01A_X + X_ITEM_SPACING * (float) i;
                    Vector2 l_v1 = Vector2Lerp(circles[0], circles[1], l_x_lerp_amount);
                    Vector2 l_v2 = Vector2Lerp(circles[2], circles[3], l_x_lerp_amount);

                    float r_x_lerp_amount = Q11A_X + X_ITEM_SPACING * (float) i;
                    Vector2 r_v1 = Vector2Lerp(circles[0], circles[1], r_x_lerp_amount);
                    Vector2 r_v2 = Vector2Lerp(circles[2], circles[3], r_x_lerp_amount);

                    float m_x_lerp_amount = MODALIDADE_X + X_ITEM_SPACING * (float) i;
                    Vector2 m_v1 = Vector2Lerp(circles[0], circles[1], m_x_lerp_amount);
                    Vector2 m_v2 = Vector2Lerp(circles[2], circles[3], m_x_lerp_amount);

                    float f_x_lerp_amount = FASE_X + X_ITEM_SPACING * (float) i;
                    Vector2 f_v1 = Vector2Lerp(circles[0], circles[1], f_x_lerp_amount);
                    Vector2 f_v2 = Vector2Lerp(circles[2], circles[3], f_x_lerp_amount);

                    if (i < 3) {
                        float y_lerp_amount = MODALIDADE_Y;
                        Vector2 m_center = Vector2Lerp(m_v1, m_v2, y_lerp_amount);
                        DrawCircleV(m_center, 5.0f, PURPLE);
                    }

                    if (i < 2) {
                        float y_lerp_amount = FASE_Y;
                        Vector2 f_center = Vector2Lerp(f_v1, f_v2, y_lerp_amount);
                        DrawCircleV(f_center, 5.0f, PURPLE);
                    }

                    for (int i = 0; i < 10; i++) {
                        float y_lerp_amount = Q01A_Y + Y_ITEM_SPACING * (float) i;
                        Vector2 l_center = Vector2Lerp(l_v1, l_v2, y_lerp_amount);
                        Vector2 r_center = Vector2Lerp(r_v1, r_v2, y_lerp_amount);
                        DrawCircleV(l_center, 5.0f, PURPLE);
                        DrawCircleV(r_center, 5.0f, PURPLE);
                    }
                }
            EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
