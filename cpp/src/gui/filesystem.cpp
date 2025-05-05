#include "gui.hpp"

UIFilesystem::UIFilesystem() {
    directory = "resources/scans_teste_oci";
    directory.resize(UI_PATH_MAX_SIZE);
    reload_fs();
    selected_entry = 1;
}

bool UIFilesystem::check_all() {
    bool check = all_checked;
    for (FSEntry& entry : entries) {
        entry.checked = check;
        entry.reading.warnings.clear();
    }
    return all_checked;
}

bool UIFilesystem::select_all() {
    bool check = all_selected;
    for (FSEntry& entry : entries) { entry.selected = check; }
    return all_selected;
}

bool UIFilesystem::update_all_checked() {
    bool check = true;
    for (FSEntry& entry : entries) {
        if (!entry.checked) {
            check = false;
            break;
        }
    }
    all_checked = check;
    return all_checked;
}

bool UIFilesystem::update_all_selected() {
    bool check = true;
    for (FSEntry& entry : entries) {
        if (!entry.selected) {
            check = false;
            break;
        }
    }
    all_selected = check;
    return all_selected;
}


void UIFilesystem::reload_fs() {
    entries.clear();

    FilePathList files = LoadDirectoryFiles(directory.c_str());
    for (size_t f = 0; f < files.count; f++) {
        std::string file_path = files.paths[f];
        if (!(file_path.ends_with(".png") || file_path.ends_with(".jpg")
              || file_path.ends_with(".jpeg"))) {
            continue;
        }
        Image file_img = LoadImage(file_path.c_str());
        ImageFormat(&file_img, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
        entries.push_back({.path = file_path, .image = file_img, .reading = {}});
    }

    UnloadDirectoryFiles(files);
    sort_fs();
    selected_entry = -1;
}


bool UIFilesystem::entry_has_warnings(const FSEntry& entry) {
    if (!entry.reading.warnings.empty()) { return true; }
    return false;
}


bool UIFilesystem::entry_has_empty_headers(const FSEntry& entry) {
    for (Header h : entry.reading.headers) {
        if ((std::string)(h.content.c_str()) == "") { return true; }
    }
    return false;
}


void UIFilesystem::sort_fs() {
    std::string current_path;
    if (selected_entry != -1) { current_path = entries[selected_entry].path; }
    if (order_ascending) {
        std::sort(entries.begin(), entries.end(), std::less<FSEntry>());
    } else {
        std::sort(entries.begin(), entries.end(), std::greater<FSEntry>());
    }
    if (selected_entry != -1) {
        for (size_t e = 0; e < entries.size(); e++) {
            if (current_path == entries[e].path) {
                selected_entry = e;
                break;
            }
        }
    }
}


void UIFilesystem::delete_selected() {
    for (int e = entries.size() - 1; e > -1; e--) {
        FSEntry& file = entries[e];
        if (file.selected) {
            if (remove(file.path.c_str()) != 0) {
                printf("Error deleting file.\n");
            } else {
                if (e == selected_entry) {
                    selected_entry--;
                    parent->fs_updated();
                }
                entries.erase(entries.begin() + e);
                printf("File successfully deleted.\n");
            }
        }
    }
}


// clang-format off
void UIFilesystem::draw() {
    ImVec2 sidebar_ws = ImGui::GetWindowSize();
    if (FileDialogBar(&file_dialog, directory.data(), directory.capacity())) {
        reload_fs();
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::BeginChild("SideBarFSHead", {0,0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);
    ImGui::PopStyleVar();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.09f, 0.10f, 0.15f, 1.00f});
    sidebar_ws.x = ImGui::GetWindowSize().x;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

        // Select all button
        if (CustomCheckbox("##1select_all", &all_selected)) { select_all(); }

        // Change order button
        ImGui::SameLine();
        ImGui::SetNextWindowSizeConstraints({0, 0}, {sidebar_ws.x - 100, 32});
        ImGui::BeginChild("SidebarOrder", {0, 0}, ImGuiChildFlags_AutoResizeY );
        std::string order_str = order_ascending ? " \uf062   Ordem" : " \uf063   Ordem";
        if (ImGui::Selectable(order_str.c_str(), false)) {
            order_ascending = !order_ascending;
            sort_fs();
        }
        ImGui::EndChild();

        // Delete selection button
        ImGui::SameLine();
        if (ImGui::Button("\uf1f8", {20, 20})) {
            delete_selected();
        }
        ImGui::PopStyleVar(); // frame rounding

        // Check all button
        ImGui::SameLine(); 
        if (ImGui::Checkbox("##2", &all_checked)) { check_all(); }
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
        for (int e = 0; e < (int)entries.size(); e++) {
            FSEntry &entry = entries[e];
            std::string file_name = GetFileName(entry.path.c_str());
            
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
            ImGui::PushID(element_counter);
            if (CustomCheckbox("", &entry.selected)) { update_all_selected(); }
            element_counter++;
            ImGui::PopID();
            ImGui::PopStyleVar();  // frame rounding

            ImGui::SameLine();
            ImGui::SetNextWindowSizeConstraints({0,0}, {sidebar_ws.x - 72, sidebar_ws.y});
            ImGui::PushID(element_counter);
            ImGui::BeginChild("", {0,0}, ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollWithMouse);
            element_counter++;

            ImGui::PushID(element_counter);
            bool colored = false;
            std::string selectable_str = "";
            if (entry_has_empty_headers(entry)) {
                colored = true;
                ImGui::PushStyleColor(ImGuiCol_Text, CATPPUCCIN_RED);
                selectable_str.append("\uf071  ");
            } else if (entry_has_warnings(entry)) {
                colored = true;
                ImGui::PushStyleColor(ImGuiCol_Text, CATPPUCCIN_YELLOW);
                selectable_str.append("\uf071  ");
            } else if (entry.checked) {
                colored = true;
                ImGui::PushStyleColor(ImGuiCol_Text, CATPPUCCIN_GREEN);
            }
            selectable_str.append(file_name.c_str());
            if (ImGui::Selectable(selectable_str.c_str(), selected_entry == e)) {
                selected_entry = e;
                parent->fs_updated();
            }
            if (colored) { ImGui::PopStyleColor(); }
            element_counter++;

            ImGui::PopID();
            ImGui::EndChild();
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushID(element_counter);
            if (ImGui::Checkbox("", &entry.checked)) { update_all_checked(); entry.reading.warnings.clear(); }
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
