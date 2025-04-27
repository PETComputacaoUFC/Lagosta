#include <cmath>

#include "ImGuiFileDialog.h"
#include "gui.hpp"
#include "imgui.h"
#include "raylib.h"
#include "raymath.h"



#define CATPPUCCIN_PEACH  (ImVec4){0.99f, 0.39f, 0.04f, 1.00f}
#define CATPPUCCIN_YELLOW (ImVec4){0.87f, 0.56f, 0.11f, 1.00f}


std::string get_path_file_name(std::string file_path) {
    size_t last_slash = file_path.rfind('/');
    std::string file_name = file_path.substr(last_slash + 1);
    return file_name;
}


// TODO: there must be a better way!
void UserInterface::draw_viewport() {
    if (selected_entry < 0) { return; }
    Image i = ImageCopy(fs_entries[selected_entry].image);
    reader.image_filter1(&i);
    Texture t = LoadTextureFromImage(i);
    BeginTextureMode(_viewport);
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
            if (ImGui::BeginTabItem("Gerador")) { ImGui::EndTabItem(); }
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
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::BeginChild("SideBarFSHead", {0,0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);
    ImGui::PopStyleVar();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, {0.09f, 0.10f, 0.15f, 1.00f});
    sidebar_ws.x = ImGui::GetWindowSize().x;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushID(1230921);
        CustomCheckbox("", &_all_entries_selected);
        ImGui::PopID();
        ImGui::SameLine();
        // Change order button
        ImGui::SetNextWindowSizeConstraints({0, 0}, {sidebar_ws.x - 100, 32});
        ImGui::BeginChild("SidebarOrder", {0, 0}, ImGuiChildFlags_AutoResizeY );
        std::string order_str = _entry_order_ascending ? " \uf062   Ordem" : " \uf063   Ordem";
        if (ImGui::Selectable(order_str.c_str(), false)) {
            _entry_order_ascending = !_entry_order_ascending;
        }
        ImGui::EndChild();
        ImGui::SameLine(); ImGui::Button("\uf1f8", {20, 20});
        ImGui::PopStyleVar(); // frame rounding
        ImGui::SameLine(); ImGui::Checkbox("##12389213921", &_all_entries_checked);
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
            for (Header &head : reading.headers) {
                InputTextTitle(head.field_name.c_str(), head.content.data(),
                               HEADER_CONTENT_MAX_CHARS);
            }
            ImGui::Spacing();
            ImGui::Text("\uf05a  Código de barras: %s", reading.barcode_string.c_str());
            ImGui::PopItemWidth();

            ImGui::SeparatorText("Gabarito");
            ImGui::Text("Gabarito: %s", reading.get_answer_string().c_str());
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
                    ImGui::TextColored(
                        CATPPUCCIN_YELLOW,
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
        ws.y -= 200;
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
        Vector2 texture_local_size = {(float)_viewport.texture.width,
                                      (float)_viewport.texture.height};
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
        auto [update, path] = DirectoryChooser();
        if (update) {
            _scans_directory = path;
            update_fs();
        }
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
        ImGui::PopStyleVar(2);

        ImGui::EndChild();  // Right panel
        ImGui::PopStyleVar();

        ImGui::EndChild();  // Leitor
    }
