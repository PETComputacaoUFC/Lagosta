#include <cmath>
#include <string>

#include "gui.hpp"
#include "imgui.h"
#include "raylib.h"
#include "raymath.h"



#define CATPPUCCIN_PEACH  (ImVec4){0.99f, 0.39f, 0.04f, 1.00f}
#define CATPPUCCIN_YELLOW (ImVec4){0.87f, 0.56f, 0.11f, 1.00f}
#define CATPPUCCIN_GREEN  (ImVec4){0.250f, 0.627f, 0.169f, 1.000f}
#define CATPPUCCIN_RED    (ImVec4){0.8235f, 0.0588f, 0.2235f, 1.0000f}



// TODO: there must be a better way!
void UserInterface::update_viewport() {
    if (selected_entry < 0) {
        BeginTextureMode(_viewport);
        ClearBackground(BLACK);
        EndTextureMode();
        return;
    }

    BeginTextureMode(_viewport);
    Image i = ImageCopy(fs_entries[selected_entry].image);
    reader.image_filter1(&i);
    Texture t = LoadTextureFromImage(i);
    ClearBackground(BLACK);
    DrawTexture(t, 0, 0, WHITE);
    reader.draw_reading(fs_entries[selected_entry].reading);
    EndTextureMode();
    UnloadTexture(t);
    UnloadImage(i);
}


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
            if (ImGui::BeginTabItem("Editor")) { ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Columns(2, "MainCols");
    if (!_layout_initialized) { ImGui::SetColumnWidth(0, 200.0f); }

    draw_sidebar();

    ImGui::NextColumn();

    draw_reader();

    ImGui::End();
    _layout_initialized = true;
}


// clang-format off
void UserInterface::draw_sidebar() {
    ImVec2 sidebar_ws = ImGui::GetWindowSize();
    if (FileDialogBar(&_scans_directory_file_dialog_instance, _scans_directory.data(), _scans_directory.capacity())) {
        update_fs();
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
        if (CustomCheckbox("##1", &_all_entries_selected)) { select_all_entries(); }

        // Change order button
        ImGui::SameLine();
        ImGui::SetNextWindowSizeConstraints({0, 0}, {sidebar_ws.x - 100, 32});
        ImGui::BeginChild("SidebarOrder", {0, 0}, ImGuiChildFlags_AutoResizeY );
        std::string order_str = _entry_order_ascending ? " \uf062   Ordem" : " \uf063   Ordem";
        if (ImGui::Selectable(order_str.c_str(), false)) {
            _entry_order_ascending = !_entry_order_ascending;
            sort_fs();
        }
        ImGui::EndChild();

        // Delete selection button
        ImGui::SameLine();
        if (ImGui::Button("\uf1f8", {20, 20})) {
            delete_selected_files();
        }
        ImGui::PopStyleVar(); // frame rounding

        // Check all button
        ImGui::SameLine(); 
        if (ImGui::Checkbox("##2", &_all_entries_checked)) { check_all_entries(); }
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
                update_viewport();
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
        for (Header &head : reading.headers) {
            InputTextTitle(head.field_name.c_str(), head.content.data(), HEADER_CONTENT_MAX_CHARS);
        }
        ImGui::Spacing();
        ImGui::Text("\uf05a  Código de barras: %s", reading.barcode_string.c_str());
        ImGui::PopItemWidth();

        ImGui::SeparatorText("Gabarito");
        ImGui::PushFont(monospace_font);
        ImGui::Text("Gabarito: %s", reading.get_answer_string().c_str());
        for (size_t i = 0; i < reading.items.size(); i++) {
            Item &item = reading.items[i];
            int c = 0;
            if (item.choice == 'a') { c = 0; }
            if (item.choice == 'b') { c = 1; }
            if (item.choice == 'c') { c = 2; }
            if (item.choice == 'd') { c = 3; }
            if (item.choice == 'e') { c = 4; }
            if (item.choice == '0') { c = 5; }
            if (item.choice == 'X') { c = 6; }
            ImGui::PushID(i);
            // Combo box with a label and our list of items.
            static const char* items[] = { "a", "b", "c", "d", "e", "0", "X" };
            char preview[2] = "";
            sprintf(preview, "%c", item.choice);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {8, 2});
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
            ImGui::Text("%02zu",i);
            ImGui::SameLine();
            if (ImGui::BeginCombo("", preview, ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_NoArrowButton)) {
                // Display selectable items
                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                    bool isSelected = (c == n);
                    if (ImGui::Selectable(items[n], isSelected)) { c = n; }
                    if (isSelected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndCombo();
            }
            ImGui::PopStyleVar(4);
            item.choice = ("abcde0X")[c];
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::Spacing();
            if (!(i % 10 == 9 || i == reading.items.size())) {
                ImGui::SameLine();
            }
        }
        ImGui::PopFont();

        ImGui::SeparatorText("Diagnóstico");
        for (size_t r = 0; r < reading.warnings.size(); r++) {
            ReadWarning warning = reading.warnings[r];
            ImGui::PushID(r);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {4, 0});
            if (ImGui::SmallButton("\uf00d")) {
                reading.warnings.erase(reading.warnings.begin() + r);
            }
            ImGui::PopStyleVar();
            ImGui::PopID();
            ImGui::SameLine();
            switch (warning) {
            case TOO_MANY_NULL_CHOICES: {
                ImGui::TextColored(CATPPUCCIN_YELLOW, "\uf071  Muitos itens nulos detectados.");
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
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();


    /* ===== PAINEL DIREITO ===== */
    ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::BeginChild("ReaderRightPanel", {0, 0}, ImGuiChildFlags_Borders);
    ImVec2 container_pos = ImGui::GetCursorScreenPos();
    ImVec2 container_size = ImGui::GetContentRegionAvail();
    ImVec2 image_size = {(float)_viewport.texture.width, (float)_viewport.texture.height};

    // Aloca espaço pro botão de "repetir leitura"
    ImVec2 ws = ImGui::GetContentRegionAvail();
    ws.y -= 240;
    ImGui::SetNextWindowSize(ws);
    ImGui::BeginChild("ViewportPreview", {0, 0}, ImGuiChildFlags_Border);
    ImGui::PopStyleVar();  // Window Padding

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
    Vector2 texture_local_size = {(float)_viewport.texture.width, (float)_viewport.texture.height};
    Vector2 texture_global_size = {scaled_image_size.x, scaled_image_size.y};
    Vector2 texture_scale = Vector2Divide(texture_local_size, texture_global_size);
    Vector2 mouse_local_00 = Vector2Subtract(mouse_global_pos, texture_global_pos);
    Vector2 mouse_local_pos = Vector2Multiply(mouse_local_00, texture_scale);

    // Desenha um viewport com um preview do gabarito na tela
    ImGui::GetWindowDrawList()->AddImage(ImTextureID(_viewport.texture.id), image_draw_pos,
                                         image_draw_br, ImVec2(0, 1), ImVec2(1, 0));
    // ImGui::Image(ImGuiID(rl_viewport.texture.id), scaled_image_size, ImVec2(0, 1), ImVec2(1,
    // 0));
    ImGui::EndChild();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {4, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 4});
    ImGui::BeginChild("BottomRightPanel", {}, ImGuiChildFlags_Border);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 0});
    ImGui::SeparatorText("Arquivo");
    ImGui::Text("Tabela de participantes");
    FileDialogBar(&_participant_file_dialog_instance, _participant_table_path.data(),
                  _participant_table_path.capacity(), true, "ChooseFileDlgKey",
                  "Escola um arquivo (.json, .csv)", "Arquivos de dados (.json, .csv){.json,.csv}");
    ImGui::Spacing();

    ImGui::Text("Tabela de respostas");
    ImGui::PushID("tabela de respostas");
    FileDialogBar(&_answers_file_dialog_instance, _answers_table_path.data(),
                  _answers_table_path.capacity(), true, "ChooseFileDlgKey",
                  "Escola um arquivo (.json, .csv)", "Arquivos de dados (.json, .csv){.json,.csv}");
    ImGui::PopID();
    ImGui::PopStyleVar();
    ImGui::Spacing();

    ImGui::SeparatorText("Leitor");
    if (selected_entry < 0) { ImGui::BeginDisabled(); }
    if (ImGui::Button("Reler atual")) { read_current(); }
    if (selected_entry < 0) { ImGui::EndDisabled(); }
    ImGui::SameLine();
    if (ImGui::Button("Reler selecionados")) { read_selection(); }
    ImGui::SameLine();
    if (ImGui::Button("Reler todos")) { read_all(); };

    if (selected_entry < 0) { ImGui::BeginDisabled(); }
    ImGui::Button("Salvar atual");
    if (selected_entry < 0) { ImGui::EndDisabled(); }
    ImGui::SameLine();
    ImGui::Button("Salvar selecionados");
    ImGui::SameLine();
    ImGui::Button("Salvar tudo");

    ImGui::EndChild();
    ImGui::PopStyleVar(2);

    ImGui::EndChild();  // Right panel
    ImGui::PopStyleVar();

    ImGui::EndChild();  // Leitor
}
