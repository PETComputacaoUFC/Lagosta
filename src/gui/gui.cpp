#include "gui.hpp"

#include <algorithm>
#include <cmath>

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"


#define CATPPUCCIN_PEACH (ImVec4){0.99f, 0.39f, 0.04f, 1.00f}
#define CATPPUCCIN_YELLOW (ImVec4){0.87f, 0.56f, 0.11f, 1.00f}


void UpdateStyle() {
    /* ===== SETTING UP FONTS ===== */
    ImGuiIO &imGuiIO = ImGui::GetIO();
    imGuiIO.IniFilename = nullptr;
    ImFont *nunito =
        imGuiIO.Fonts->AddFontFromFileTTF("resources/fonts/NunitoSans-Regular.ttf", 20.0f, nullptr,
                                          imGuiIO.Fonts->GetGlyphRangesDefault());
    imGuiIO.FontDefault = nunito;
    // font-awesome
    static const ImWchar icons_ranges[] = {0xe005, 0xf8ff, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.FontDataOwnedByAtlas = false;
    icons_config.GlyphMaxAdvanceX = std::numeric_limits<float>::max();
    icons_config.RasterizerMultiply = GetWindowScaleDPI().y;
    icons_config.OversampleH = 2;
    icons_config.OversampleV = 1;
    icons_config.GlyphRanges = icons_ranges;
    ImGuiIO &io = ImGui::GetIO();
    float size = 16;
    if (!IsWindowState(FLAG_WINDOW_HIGHDPI)) { size *= GetWindowScaleDPI().y; }
    io.Fonts->AddFontFromFileTTF("resources/fonts/fa-solid-900.ttf", size, &icons_config,
                                 icons_ranges);

    /* ===== SETTING UP STYLE ===== */
    ImGuiStyle &style = ImGui::GetStyle();
    // padding & spacing
    style.WindowPadding = {8, 8};
    style.FramePadding = {8, 5};
    style.ItemSpacing = {8, 4};
    // scrollbar
    style.ScrollbarSize = {10};
    style.GrabMinSize = {10};
    // borders
    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    // rounding
    style.WindowRounding = 0;
    style.ChildRounding = 0;
    style.FrameRounding = 32;
    style.PopupRounding = 0;
    style.ScrollbarRounding = 32;
    style.GrabRounding = 32;
    // tab configs
    style.TabBorderSize = 1;
    style.TabBarBorderSize = 0;
    style.TabRounding = 0;
    // using textures for anti-aliased lines makes them pretty rough
    style.AntiAliasedLinesUseTex = false;

    /* ===== SETTING UP COLORS ===== */
    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.79f, 0.83f, 0.96f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.43f, 0.45f, 0.55f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.15f, 0.23f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.13f, 0.19f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.29f, 0.30f, 0.39f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.13f, 0.19f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.40f, 0.96f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.40f, 0.96f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.15f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.13f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.19f, 0.20f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.29f, 0.30f, 0.39f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.58f, 0.60f, 0.72f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.43f, 0.45f, 0.55f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.38f, 0.47f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.12f, 0.40f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.12f, 0.40f, 0.96f, 0.78f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.12f, 0.40f, 0.96f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.12f, 0.40f, 0.96f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.12f, 0.40f, 0.96f, 0.67f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.40f, 0.96f, 0.80f);
    colors[ImGuiCol_Header] = ImVec4(0.12f, 0.40f, 0.96f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.12f, 0.40f, 0.96f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.40f, 0.96f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.12f, 0.40f, 0.96f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.12f, 0.40f, 0.96f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.35f, 0.35f, 0.35f, 0.17f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.12f, 0.40f, 0.96f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.12f, 0.40f, 0.96f, 0.95f);
    colors[ImGuiCol_InputTextCursor] = ImVec4(0.79f, 0.83f, 0.96f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.09f, 0.10f, 0.15f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.29f, 0.30f, 0.39f, 1.00f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.14f, 0.15f, 0.23f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.12f, 0.40f, 0.96f, 1.00f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.26f, 0.59f, 1.00f, 0.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
    colors[ImGuiCol_TextLink] = ImVec4(0.12f, 0.40f, 0.96f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.12f, 0.40f, 0.96f, 0.35f);
    colors[ImGuiCol_TreeLines] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.12f, 0.40f, 0.96f, 0.95f);
    colors[ImGuiCol_NavCursor] = ImVec4(0.12f, 0.40f, 0.96f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}


UserInterface::UserInterface() {
    BeginTextureMode(rl_viewport);
    ClearBackground(BLACK);
    EndTextureMode();
}

// UnloadDirectoryFiles(pathlist);

bool update_reading = true;


void UserInterface::draw() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    rlImGuiBegin();

    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("root", nullptr,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove);

    // Create a tab bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginTabBar("MainTabBar")) {
            if (ImGui::BeginTabItem("Leitor")) { ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Scanner")) { ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("Gerador")) { ImGui::EndTabItem(); }
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
    ImGui::Text("scans/base.png");
    ImGui::EndChild();

    ImGui::NextColumn();

    draw_reader();

    ImGui::End();
    rlImGuiEnd();
}


void UserInterface::draw_reader() {
    ImGui::BeginChild("Leitor");
    ImGui::Columns(2, "LeitorCols");

    /* ===== PAINEL ESQUERDO ===== */
    ImGui::BeginChild("ReaderLeftPanel");
    float windowWidth = ImGui::GetContentRegionMax().x;
    if (selected_entry > -1) {
        ImGui::PushItemWidth(windowWidth);
        Reading &reading = fs_entries[selected_entry].reading;
        for (Header &header : reading.headers) {
            ImGui::Text("%s", header.field_name.c_str());
            ImGui::InputText("##6", header.content.data(), HEADER_CONTENT_MAX_CHARS);
            ImGui::Spacing();
        }
        ImGui::Text("\uf05a  Código de barras: %s", reading.barcode_string.c_str());
        ImGui::PopItemWidth();

        ImGui::Separator();

        // clang-format off
        ImGui::Text("Gabarito: %s", reading.get_answer_string().c_str());
        for (ReadWarning rw : reading.warnings) {
            switch (rw) {
            case TOO_MANY_NULL_CHOICES: {
                ImGui::TextColored(CATPPUCCIN_YELLOW,
                    "\uf071  Muitos itens nulos detectados.");
                break;
            }
            case IMPRECISE_READING_RECTANGLE: {
                ImGui::TextColored(CATPPUCCIN_YELLOW,
                    "\uf071  A detecção de alinhamento pode ter sido imprecisa.");
                break;
            }
            case BARCODE_NOT_FOUND: {
                ImGui::TextColored(CATPPUCCIN_YELLOW,
                    "\uf071  Não foi possível ler o código de barras.");
                break;
            }
            }
        }
        // clang-format on
        ImGui::Button("Salvar .csv");
    }

    ImGui::EndChild();


    /* ===== PAINEL DIREITO ===== */
    ImGui::NextColumn();
    // Aloca espaço pro botão de "repetir leitura"
    ImVec2 ws = ImGui::GetContentRegionAvail();
    ws.y -= 35;
    ImGui::SetNextWindowSize(ws);

    ImGui::BeginChild("ReaderRightPanel", {0, 0}, ImGuiChildFlags_Border);
    ImVec2 container_pos = ImGui::GetCursorScreenPos();
    ImVec2 container_size = ImGui::GetContentRegionAvail();
    ImVec2 image_size = {(float)rl_viewport.texture.width, (float)rl_viewport.texture.height};

    // Calcula coordenadas pra centralizar a imagem na janela
    float scale = container_size.x / image_size.x;
    ImVec2 scaled_image_size = {image_size.x * scale, image_size.y * scale};
    ImVec2 image_draw_pos = container_pos;
    image_draw_pos.x += (container_size.x - scaled_image_size.x) / 2.0f;
    // bottom-right
    ImVec2 image_draw_br = image_draw_pos;
    image_draw_br.x += scaled_image_size.x;
    image_draw_br.y += scaled_image_size.y;

    // Cálculo coordenadas globais -> locais
    Vector2 mouse_global_pos = GetMousePosition();
    Vector2 texture_global_pos = {container_pos.x, container_pos.y};
    Vector2 texture_local_size = {(float)rl_viewport.texture.width,
                                  (float)rl_viewport.texture.height};
    Vector2 texture_global_size = {scaled_image_size.x, scaled_image_size.y};
    Vector2 texture_scale = Vector2Divide(texture_local_size, texture_global_size);
    Vector2 mouse_local_00 = Vector2Subtract(mouse_global_pos, texture_global_pos);
    Vector2 mouse_local_pos = Vector2Multiply(mouse_local_00, texture_scale);

    BeginTextureMode(rl_viewport);
    ClearBackground(BLACK);
    Rectangle container_rect = {texture_global_pos.x, texture_global_pos.y, container_size.x,
                                std::min(container_size.y + 7, texture_global_size.y)};
    if (CheckCollisionPointRec(mouse_global_pos, container_rect)) {
        DrawCircleV(mouse_local_pos, 10.0f, RED);
    }
    EndTextureMode();

    // Desenha um viewport com um preview do gabarito na tela
    ImGui::GetWindowDrawList()->AddImage(ImTextureID(rl_viewport.texture.id), image_draw_pos,
                                         image_draw_br, ImVec2(0, 1), ImVec2(1, 0));
    // ImGui::Image(ImGuiID(rl_viewport.texture.id), scaled_image_size, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
    ImGui::Button("Repetir Leitura");
    ImGui::EndChild();
}