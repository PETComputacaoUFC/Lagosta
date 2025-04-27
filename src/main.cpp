#include "gui/gui.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    int screenWidth = 1280;
    int screenHeight = 720;
    // do not set the FLAG_WINDOW_HIGHDPI flag, that scales a low res framebuffer up to the native
    // resolution. use the native resolution and scale your geometry.
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Lagosta");
    SetTargetFPS(144);
    SetExitKey(KEY_NULL);

    rlImGuiSetup(true);

    UserInterface ui{};

    while (!WindowShouldClose()) {
        BeginDrawing();
        rlImGuiBegin();

        ui.draw();
        // ImGui::ShowDemoWindow();
        // ImGui::SetWindowFocus("Dear ImGui Demo");

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}