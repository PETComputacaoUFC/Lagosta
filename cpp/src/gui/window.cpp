#include "gui.hpp"

UIWindow::UIWindow() { sidebar.parent = this; update_style(); }


void UIWindow::fs_updated() {
    size_t tab_num = tabs.size();
    for (size_t tab = 0; tab < tab_num; tab++) {
        tabs[tab]->fs_updated();
    }
}


void UIWindow::draw() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin("root", nullptr,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar
                     | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
                     | ImGuiWindowFlags_NoMove);
    ImGui::PopStyleVar();

    // Create a tab bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginTabBar("MainTabBar")) {
            if (ImGui::BeginTabItem("Leitor")) { ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Scanner")) { ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Editor")) { ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Columns(2, "MainCols");
    if (!layout_initialized) { ImGui::SetColumnWidth(0, 200.0f); }

    sidebar.draw();

    ImGui::NextColumn();

    if (selected_tab < tabs.size()) { tabs[selected_tab]->draw(); }

    ImGui::End();
    layout_initialized = true;
}