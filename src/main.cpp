#include <cstdio>

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "ui/ui.hpp"

bool ImGuiDemoOpen = true;

int main() {
    SetTraceLogLevel(LOG_WARNING);
    int screenWidth = 1280;
    int screenHeight = 720;
    // do not set the FLAG_WINDOW_HIGHDPI flag, that scales a low res framebuffer up to the native
    // resolution. use the native resolution and scale your geometry.
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE );
    InitWindow(screenWidth, screenHeight, "Lagosta");
    SetTargetFPS(144);

    rlImGuiSetup(true);
    ImGuiIO &imGuiIO = ImGui::GetIO();
    imGuiIO.IniFilename = nullptr;
    // imGuiIO.LogFilename = nullptr;
    SetExitKey(KEY_NULL);
    UpdateStyle();

    bool layout_init = false;
    while (!WindowShouldClose()) {
        IsKeyPressed(KEY_ESCAPE);

        BeginDrawing();
        ClearBackground(DARKGRAY);
        rlImGuiBegin();

        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("root", nullptr,
                     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar
                         | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                         | ImGuiWindowFlags_NoMove);

        // Create a tab bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginTabBar("MainTabBar")) {
                if (ImGui::BeginTabItem("Leitor")) { ImGui::EndTabItem(); }
                if (ImGui::BeginTabItem("Scanner")) { ImGui::EndTabItem(); }
                ImGui::EndTabBar();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Columns(2, "MainCols");
        if (!layout_init) {
            ImGui::SetColumnWidth(0, 200.0f);
            layout_init = true;
        }

        ImGui::BeginChild("SidePanel");
        ImGui::Text("This is the side panel!");
        ImGui::EndChild();

        ImGui::NextColumn();

        ImGui::BeginChild("Leitor");
            ImGui::Columns(2, "LeitorCols");
            ImGui::BeginChild("ReaderLeftPanel");
            ImGui::Text("This is the reader left panel!");
            ImGui::Button("Button1");
            ImGui::EndChild();

            ImGui::NextColumn();

            ImGui::BeginChild("ReaderRightPanel");
                ImGui::BeginChild("ReaderTopRightPanel", {0,320}, ImGuiChildFlags_ResizeY);
                ImGui::Text("This is the reader top-right panel!");
                ImGui::Button("Button2");
                ImGui::EndChild();

                ImGui::Separator();

                ImGui::BeginChild("ReaderBottomRightPanel", {0,0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize);
                ImGui::Text("This is the reader bottom-right panel!");
                ImGui::Button("Button2");
                ImGui::EndChild();
            ImGui::EndChild();
        ImGui::EndChild();

        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}