#include "gui.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>

#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "raylib.h"

void UserInterface::update_style() {
    /* ===== SETTING UP FONTS ===== */
    ImGuiIO& imGuiIO = ImGui::GetIO();
    imGuiIO.IniFilename = nullptr;
    ImFont* nunito =
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
    ImGuiIO& io = ImGui::GetIO();
    float size = 16;
    if (!IsWindowState(FLAG_WINDOW_HIGHDPI)) { size *= GetWindowScaleDPI().y; }
    // font-awesome
    io.Fonts->AddFontFromFileTTF("resources/fonts/fa-solid-900.ttf", size, &icons_config,
                                 icons_ranges);
    // monospace font
    monospace_font = io.Fonts->AddFontFromFileTTF("resources/fonts/Inconsolata-Regular.ttf", 16.0f);

    /* ===== SETTING UP STYLE ===== */
    ImGuiStyle& style = ImGui::GetStyle();
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

    /* ===== FILE DIALOG ICONS ===== */
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", {1, 1, 1, 1},
                                              "\uf07b");  // folder
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", {1, 1, 1, 1},
                                              "\uf15c");  // file
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpg", {1, 1, 1, 1},
                                              "\uf03e");  // image
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".jpeg", {1, 1, 1, 1},
                                              "\uf03e");  // image
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", {1, 1, 1, 1},
                                              "\uf03e");  // image
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, ".csv", {1, 1, 1, 1},
                                              "\uf00b");  // table
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, ".json", {1, 1, 1, 1},
                                              "\uf00b");  // table

    // 0.09f, 0.10f, 0.15f, 1.00f
    /* ===== SETTING UP COLORS ===== */
    ImVec4* colors = style.Colors;
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
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.12f, 0.13f, 0.19f, 1.00f);
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

void UserInterface::update_fs() {
    fs_entries.clear();

    FilePathList files = LoadDirectoryFiles(_scans_directory.c_str());
    for (size_t f = 0; f < files.count; f++) {
        std::string file_path = files.paths[f];
        if (!(file_path.ends_with(".png") || file_path.ends_with(".jpg")
              || file_path.ends_with(".jpeg"))) {
            continue;
        }
        Image file_img = LoadImage(file_path.c_str());
        ImageFormat(&file_img, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
        fs_entries.push_back({.path = file_path, .image = file_img, .reading = {}});
    }

    UnloadDirectoryFiles(files);
    sort_fs();
    selected_entry = -1;
}


bool UserInterface::entry_has_warnings(const FSEntry& entry) {
    if (!entry.reading.warnings.empty()) { return true; }
    // TODO: empty headers
    // for (Header h : entry.reading.headers) {
    //     if ((std::string)(h.content.c_str()) == "") { return true; }
    // }
    return false;
}


bool UserInterface::entry_has_empty_headers(const FSEntry& entry) {
    for (Header h : entry.reading.headers) {
        if ((std::string)(h.content.c_str()) == "") { return true; }
    }
    return false;
}


void UserInterface::read_current() {
    if (selected_entry < 0) { return; }
    FSEntry& entry = fs_entries[selected_entry];
    Reading r = reader.read(entry.image);
    entry.reading = r;
    update_viewport();
}

void UserInterface::read_selection() {
    for (FSEntry& entry : fs_entries) {
        if (!entry.selected) { continue; }
        Reading r = reader.read(entry.image);
        entry.reading = r;
    }
    update_viewport();
}

void UserInterface::read_all() {
    for (FSEntry& entry : fs_entries) {
        Reading r = reader.read(entry.image);
        entry.reading = r;
    }
    update_viewport();
}


void UserInterface::sort_fs() {
    std::string current_path;
    if (selected_entry != -1) { current_path = fs_entries[selected_entry].path; }
    if (_entry_order_ascending) {
        std::sort(fs_entries.begin(), fs_entries.end(), std::less<FSEntry>());
    } else {
        std::sort(fs_entries.begin(), fs_entries.end(), std::greater<FSEntry>());
    }
    if (selected_entry != -1) {
        for (size_t e = 0; e < fs_entries.size(); e++) {
            if (current_path == fs_entries[e].path) {
                selected_entry = e;
                break;
            }
        }
    }
}


void UserInterface::delete_selected_files() {
    for (int e = fs_entries.size() - 1; e > -1; e--) {
        FSEntry& file = fs_entries[e];
        if (file.selected) {
            if (remove(file.path.c_str()) != 0) {
                printf("Error deleting file.\n");
            } else {
                if (e == selected_entry) {
                    selected_entry--;
                    update_viewport();
                }
                fs_entries.erase(fs_entries.begin() + e);
                printf("File successfully deleted.\n");
            }
        }
    }
}


void UserInterface::check_all_entries() {
    bool check = _all_entries_checked;
    for (FSEntry& entry : fs_entries) {
        entry.checked = check;
        entry.reading.warnings.clear();
    }
};

void UserInterface::select_all_entries() {
    bool check = _all_entries_selected;
    for (FSEntry& entry : fs_entries) { entry.selected = check; }
}

void UserInterface::update_all_checked() {
    bool check = true;
    for (FSEntry& entry : fs_entries) {
        if (!entry.checked) {
            check = false;
            break;
        }
    }
    _all_entries_checked = check;
}

void UserInterface::update_all_selected() {
    bool check = true;
    for (FSEntry& entry : fs_entries) {
        if (!entry.selected) {
            check = false;
            break;
        }
    }
    _all_entries_selected = check;
}


UserInterface::UserInterface() : fs_entries({}), reader(Reader{}) {
    _scans_directory.resize(UI_PATH_MAX_SIZE);
    _participant_table_path.resize(UI_PATH_MAX_SIZE);
    _answers_table_path.resize(UI_PATH_MAX_SIZE);
    update_style();
    update_fs();
    update_viewport();
}