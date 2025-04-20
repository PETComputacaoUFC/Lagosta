#include <stdbool.h>

#define CLAY_IMPLEMENTATION
// #include "clay.h"
#include "clay_renderer_raylib.c"
#include "raylib.h"
#include "raymath.h"

#include "reader.hpp"


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

    // out0002: checar se a direita foi lida corretamente, simulação de "erro" na marcação do gabarito
    // out0003: analisar marcações com baixo contraste
    // out0004: analisar itens 04, 07, 12, 19, 20
    // out0005: analisar itens 05, 10, 11, 12, 14, 15
    // out0006: analisar marcações de baixíssimo contraste
    // out0008: analisar marcações de baixo contraste
    // out0009: analisar itens 07, 13, 14
    // 0010, 0011, 0012, 0013 são versões de menor contraste de 0002, 0003, 0002, 0009
    Image img_gabarito = LoadImage("resources/scans_teste_oci/out0004.png");
    Reader reader = Reader(&img_gabarito, circles, SAMPLE_CIRCLE);

    Camera2D camera = {};
    camera.zoom = 0.75f;

    for (int i = 0; i < 20; i++) {
        reader.items[i] = Item();
        reader.items[i].choice_readings = { 0.0, 0.0, 0.0, 0.0, 0.0 };
    }

    size_t dragging = -1;
    int filter = 1;

    Texture2D texturas[3] = {
        LoadTextureFromImage(img_gabarito),
        LoadTextureFromImage(reader.image_filtered1),
        LoadTextureFromImage(reader.image_filtered2),
    };

    while (!WindowShouldClose()) {
        // ==== UPDATE ====
        if (IsKeyPressed(KEY_SPACE)) {
            filter = (filter + 1) % 3;
        }

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

        // Prints box coords
        if (IsKeyPressed(KEY_P)) {
            printf("v0  x%.2f y%.2f | v1  x%.2f y%.2f\n", circles[0].x, circles[0].y, circles[1].x, circles[1].y);
            printf("v2  x%.2f y%.2f | v3  x%.2f y%.2f\n", circles[2].x, circles[2].y, circles[3].x, circles[3].y);
        }

        // Updates reader and... well... reads, ig.
        if (IsKeyPressed(KEY_R)) {
            for (int i = 0; i < 4; i++) { reader.square[i] = circles[i]; }
            std::string answers = reader.read();
            printf("%s\n", answers.c_str());
        }

        // ==== DRAW ====
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                DrawTexture(texturas[filter], 0, 0, WHITE);
                for (Vector2 circle_center: circles) {
                    DrawCircleV(circle_center, circle_radius, RED);
                }

                for (int i = 0; i < 10; i++) {
                    float y_lerp_amount = Q01A_Y + Y_ITEM_SPACING * (float) i;
                    for (int c = 0; c < 5; c++) {
                        float x_lerp_amount = Q01A_X + X_ITEM_SPACING * (float) c;
                        Vector2 v1 = Vector2Lerp(circles[0], circles[1], x_lerp_amount);
                        Vector2 v2 = Vector2Lerp(circles[2], circles[3], x_lerp_amount);
            
                        Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
                        char text[6];
                        sprintf(text, "%.2f", reader.items[i].choice_readings[c]);
                        DrawText(text, center.x, center.y, 20, YELLOW);
                        DrawCircleV(center, 5.0f, reader.items[i].choice == c ? ORANGE : PURPLE);
                    }
                }

                for (int i = 0; i < 10; i++) {
                    float y_lerp_amount = Q11A_Y + Y_ITEM_SPACING * (float) i;
                    for (int c = 0; c < 5; c++) {
                        float x_lerp_amount = Q11A_X + X_ITEM_SPACING * (float) c;
                        Vector2 v1 = Vector2Lerp(circles[0], circles[1], x_lerp_amount);
                        Vector2 v2 = Vector2Lerp(circles[2], circles[3], x_lerp_amount);
            
                        Vector2 center = Vector2Lerp(v1, v2, y_lerp_amount);
                        char text[6];
                        sprintf(text, "%.2f", reader.items[i+10].choice_readings[c]);
                        DrawText(text, center.x, center.y, 20, YELLOW);
                        DrawCircleV(center, 5.0f, reader.items[i+10].choice == c ? ORANGE : PURPLE);
                    }
                }

            EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
