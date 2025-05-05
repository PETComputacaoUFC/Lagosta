#include "raylib.h"
#include "rlImGui.h"
// #include "imgui.h"

#include "gui/gui.hpp"
#include "rapidcsv.h"

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

    Reader r{.data_table = rapidcsv::Document("resources/data/participantes_teste.csv",
                                              rapidcsv::LabelParams(0, 0))};
    //

    rlImGuiSetup(true);
    UIReader reader = UIReader();
    reader.reader = r;
    UIWindow ui = UIWindow();
    ui.add_tab(&reader);

    while (!WindowShouldClose()) {
        BeginDrawing();
        rlImGuiBegin();

        ui.draw();
        // ImGui::ShowDemoWindow();

        rlImGuiEnd();
        EndDrawing();
    }

    // delete reader;
    rlImGuiShutdown();
    CloseWindow();
    return 0;
}