#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.hpp"

#include <cmath>

#include "imgui.h"
#include "imgui_internal.h"
#include "raylib.h"
#include "raymath.h"


#define CATPPUCCIN_PEACH  (ImVec4){0.99f, 0.39f, 0.04f, 1.00f}
#define CATPPUCCIN_YELLOW (ImVec4){0.87f, 0.56f, 0.11f, 1.00f}


void UserInterface::update_style() {
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
    style.ItemSpacing = {0, 4};
    // scrollbar
    style.ScrollbarSize = {10};
    style.GrabMinSize = {10};
    // borders
    style.WindowBorderSize = 1;
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

    // 0.09f, 0.10f, 0.15f, 1.00f
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

// default checkbox but with white square instead of check. ugh.
bool CustomCheckbox(const char *label, bool *v) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    const float square_sz = ImGui::GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(
        pos,
        pos
            + ImVec2(
                square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
                label_size.y + style.FramePadding.y * 2.0f));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    const bool is_visible = ImGui::ItemAdd(total_bb, id);
    const bool is_multi_select = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
    if (!is_visible)
        if (!is_multi_select || !g.BoxSelectState.UnclipMode
            || !g.BoxSelectState.UnclipRect.Overlaps(
                total_bb))  // Extra layer of "no logic clip" for box-select support
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label,
                                        g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable
                                            | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

    // Range-Selection/Multi-selection support (header)
    bool checked = *v;
    if (is_multi_select) ImGui::MultiSelectItemHeader(id, &checked, NULL);

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

    // Range-Selection/Multi-selection support (footer)
    if (is_multi_select)
        ImGui::MultiSelectItemFooter(id, &checked, &pressed);
    else if (pressed)
        checked = !checked;

    if (*v != checked) {
        *v = checked;
        pressed = true;  // return value
        ImGui::MarkItemEdited(id);
    }

    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
    const bool mixed_value = (g.LastItemData.ItemFlags & ImGuiItemFlags_MixedValue) != 0;
    if (is_visible) {
        ImGui::RenderNavCursor(total_bb, id);
        ImGui::RenderFrame(check_bb.Min, check_bb.Max,
                           ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive
                                              : hovered         ? ImGuiCol_FrameBgHovered
                                                                : ImGuiCol_FrameBg),
                           true, style.FrameRounding);
        ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
        if (mixed_value) {
            // Undocumented tristate/mixed/indeterminate checkbox (#2644)
            // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue
            // supported by all widgets (not just checkbox)
            ImVec2 pad(ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)),
                       ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col,
                                            style.FrameRounding);
        } else if (*v) {
            const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 6.0f)) * 1.5f;
            window->DrawList->AddRectFilled(check_bb.Min + ImVec2(pad, pad),
                                            check_bb.Max - ImVec2(pad, pad),
                                            IM_COL32(255, 255, 255, 255), style.FrameRounding);
        }
    }
    const ImVec2 label_pos =
        ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
    if (g.LogEnabled) ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
    if (is_visible && label_size.x > 0.0f) ImGui::RenderText(label_pos, label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label,
                                g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable
                                    | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

std::string get_path_file_name(std::string file_path) {
    size_t last_slash = file_path.rfind('/');
    std::string file_name = file_path.substr(last_slash + 1);
    return file_name;
}


UserInterface::UserInterface() : fs_entries({}), reader(Reader{}) {
    update_style();

    FilePathList files = LoadDirectoryFiles("resources/scans_teste_oci");
    for (size_t f = 0; f < files.count; f++) {
        std::string file_path = files.paths[f];
        Image file_img = LoadImage(file_path.c_str());
        Reading file_reading = reader.read(file_img);
        fs_entries.push_back({.name = file_path, .image = file_img, .reading = file_reading});
    }

    selected_entry = 1;
    draw_viewport();
}

// TODO: there must be a better way!
void UserInterface::draw_viewport() {
    Image i = ImageCopy(fs_entries[selected_entry].image);
    reader.image_filter1(&i);
    Texture t = LoadTextureFromImage(i);
    BeginTextureMode(rl_viewport);
    ClearBackground(BLACK);
    DrawTexture(t, 0, 0, WHITE);
    reader.draw_reading(fs_entries[selected_entry].reading);
    EndTextureMode();
    UnloadTexture(t);
    UnloadImage(i);
}

// UnloadDirectoryFiles(pathlist);

bool update_reading = true;


void UserInterface::draw() {
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
            if (ImGui::BeginTabItem("Gerador")) { ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Columns(2, "MainCols");
    if (!layout_initialized) { ImGui::SetColumnWidth(0, 200.0f); }

    draw_sidebar();

    ImGui::NextColumn();

    draw_reader();

    ImGui::End();
    layout_initialized = true;
}


// clang-format off
void UserInterface::draw_sidebar() {
    ImVec2 sidebar_ws = ImGui::GetWindowSize();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::BeginChild("SideBarFSHead", {0,0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);
    ImGui::PopStyleVar();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.09f, 0.10f, 0.15f, 1.00f});
    sidebar_ws.x = ImGui::GetWindowSize().x;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushID(1230921);
        CustomCheckbox("", &all_entries_selected);
        ImGui::PopID();
        ImGui::SameLine();
        // Change order button
        ImGui::SetNextWindowSizeConstraints({0, 0}, {sidebar_ws.x - 100, 32});
        ImGui::BeginChild("SidebarOrder", {0, 0}, ImGuiChildFlags_AutoResizeY );
        std::string order_str = entry_order_ascending ? " \uf062   Ordem" : " \uf063   Ordem";
        if (ImGui::Selectable(order_str.c_str(), false)) {
            entry_order_ascending = !entry_order_ascending;
        }
        ImGui::EndChild();
        ImGui::SameLine(); ImGui::Button("\uf1f8", {20, 20});
        ImGui::PopStyleVar(); // frame rounding
        ImGui::SameLine(); ImGui::Checkbox("##12389213921", &all_entries_checked);
        ImGui::PopStyleVar(); // frame padding
        ImGui::PopStyleVar(); // item spacing
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
    ImGui::EndChild();  // SideBarFSHead
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(); // item spacing

    
    // Draws the filesystem entries
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::SetNextWindowSizeConstraints({0, 0}, {sidebar_ws.x, sidebar_ws.y});
    ImGui::BeginChild("SidebarFSEntries", {0, 0}, ImGuiChildFlags_FrameStyle);
    ImGui::PopStyleVar();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.09f, 0.10f, 0.15f, 1.00f});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

        int element_counter = 0;
        for (int e = 0; e < (int)fs_entries.size(); e++) {
            FSEntry &entry = fs_entries[e];
            std::string file_name = get_path_file_name(entry.name);
            
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
            ImGui::PushID(element_counter);
            CustomCheckbox("", &entry.selected);
            element_counter++;
            ImGui::PopID();
            ImGui::PopStyleVar();  // frame rounding

            ImGui::SameLine();
            ImGui::SetNextWindowSizeConstraints({0,0}, {sidebar_ws.x - 72, sidebar_ws.y});
            ImGui::PushID(element_counter);
            ImGui::BeginChild("", {0,0}, ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollWithMouse);
            element_counter++;
                ImGui::PushID(element_counter);
                if (ImGui::Selectable(file_name.c_str(), selected_entry == e)) {
                    selected_entry = e;
                    draw_viewport();
                }
                element_counter++;
                ImGui::PopID();
            ImGui::EndChild();
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushID(element_counter);
            ImGui::Checkbox("", &entry.checked);
            element_counter++;
            ImGui::PopID();
        }
        ImGui::PopStyleVar();  // window padding
        ImGui::PopStyleVar();  // item spacing
        ImGui::PopStyleVar();  // frame padding
    ImGui::PopStyleColor();
    ImGui::EndChild();
}
// clang-format on


void UserInterface::draw_reader() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 4});
    ImGui::BeginChild("Leitor");
    ImGui::Columns(2, "LeitorCols");

    /* ===== PAINEL ESQUERDO ===== */
    ImGui::BeginChild("ReaderLeftPanel");
    ImGui::SeparatorText("Cabeçalho");

    float windowWidth = ImGui::GetContentRegionMax().x;
    if (selected_entry > -1) {
        ImGui::PushItemWidth(windowWidth);
        Reading &reading = fs_entries[selected_entry].reading;
        int header_counter = 0;
        for (Header &header : reading.headers) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 0});
            ImGui::Text("%s", header.field_name.c_str());
            ImGui::PushID(header_counter);
            ImGui::InputText("", header.content.data(), HEADER_CONTENT_MAX_CHARS);
            ImGui::PopID();
            ImGui::PopStyleVar();
            ImGui::Spacing();
            header_counter++;
        }
        ImGui::Text("\uf05a  Código de barras: %s", reading.barcode_string.c_str());
        ImGui::PopItemWidth();

        ImGui::SeparatorText("Gabarito");

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
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();


    /* ===== PAINEL DIREITO ===== */
    ImGui::NextColumn();
    // Aloca espaço pro botão de "repetir leitura"
    ImVec2 ws = ImGui::GetContentRegionAvail();
    ws.y -= 136;
    ImGui::SetNextWindowSize(ws);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::BeginChild("ReaderRightPanel", {0, 0}, ImGuiChildFlags_Border);
    ImGui::PopStyleVar();
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

    // BeginTextureMode(rl_viewport);
    // ClearBackground(BLACK);
    // Rectangle container_rect = {texture_global_pos.x, texture_global_pos.y, container_size.x,
    //                             std::min(container_size.y + 7, texture_global_size.y)};
    // if (ImGui::IsItemActive()) { DrawCircleV(mouse_local_pos, 10.0f, RED); }
    // EndTextureMode();

    // Desenha um viewport com um preview do gabarito na tela
    ImGui::GetWindowDrawList()->AddImage(ImTextureID(rl_viewport.texture.id), image_draw_pos,
                                         image_draw_br, ImVec2(0, 1), ImVec2(1, 0));
    // ImGui::Image(ImGuiID(rl_viewport.texture.id), scaled_image_size, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();

    ImGui::SeparatorText("Leitor");
    ImGui::Button("Reler atual");
    ImGui::SameLine();
    ImGui::Button("Reler selecionados");
    ImGui::SameLine();
    ImGui::Button("Reler todos");
    ImGui::SeparatorText("Arquivo");
    ImGui::Button("Salvar atual");
    ImGui::SameLine();
    ImGui::Button("Salvar selecionados");
    ImGui::SameLine();
    ImGui::Button("Salvar tudo");
    ImGui::EndChild();
}
