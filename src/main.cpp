#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylib.c"
#include "raylib.h"

void clay_error_handler(Clay_ErrorData errorData) { printf("%s", errorData.errorText.chars); }

// Your project's main entry
int main(void) {
    SetTraceLogLevel(LOG_WARNING);

    Font fonts[1];
    // ClayMan clayMan(1024, 786, Raylib_MeasureText, fonts);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory =
        Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Initialize(clayMemory, {1024, 786}, (Clay_ErrorHandler)clay_error_handler);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Clay_Raylib_Initialize(
        1024, 786, "Lagosta",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    // Load fonts after initializing raylib
    fonts[0] = LoadFontEx("resources/fonts/NunitoSans-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);




    // Raylib render loop
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_D)) { Clay_SetDebugModeEnabled(!Clay_IsDebugModeEnabled()); }

        // Raylib mouse position and scroll vectors
        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetLayoutDimensions({(float)GetScreenWidth(), (float)GetScreenHeight()});
        Clay_SetPointerState({mousePosition.x, mousePosition.y}, IsMouseButtonDown(0));
        Clay_UpdateScrollContainers(true, {scrollDelta.x, scrollDelta.y}, GetFrameTime());

        // Prep for layout
        Clay_BeginLayout();

        // Example full-window parent container
        CLAY({// Configure element
              .id = CLAY_ID("Root"),
              .layout =
                  {
                      .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                      .padding = CLAY_PADDING_ALL(16),
                      .childGap = 16,
                      .layoutDirection = CLAY_TOP_TO_BOTTOM,
                  },
              .backgroundColor = {50, 50, 50, 255},
              .border = {
                  .color = {50, 50, 180, 255},
                  .width = {1, 1, 1, 1},
              }}) {  // Child elements in here
            CLAY_TEXT(CLAY_STRING("Hello World"),
                      CLAY_TEXT_CONFIG({.textColor = {255, 255, 255, 255}, .fontSize = 24}));
            CLAY_TEXT(CLAY_STRING("Hello World 2"),
                      CLAY_TEXT_CONFIG({.textColor = {255, 255, 255, 255}, .fontSize = 24}));
        }
        // );

        // Pass your layout to the manager to get the render commands
        Clay_RenderCommandArray renderCommands = Clay_EndLayout();

        BeginDrawing();                             // Start Raylib's draw block
        ClearBackground(BLACK);                     // Raylib's clear function
        Clay_Raylib_Render(renderCommands, fonts);  // Render Clay Layout
        EndDrawing();                               // End Raylib's draw block
    }
    
    UnloadFont(fonts[0]);
    return 0;
}
