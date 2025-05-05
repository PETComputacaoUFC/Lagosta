#include <cctype>
#include <cstdio>
#include <string>
#include "gui.hpp"
#include "imgui.h"
#include "raymath.h"

const char ITEMS_STR[8] = "abcde0X";

void UIReader::read_current() {
    std::vector<FSEntry>& fs_entries = parent->sidebar.entries;
    int selected_entry = parent->sidebar.selected_entry;
    if (selected_entry < 0) { return; }
    FSEntry& entry = fs_entries[selected_entry];
    Reading r = reader.read(entry.image);
    entry.reading = r;
    update_viewport();
}

void UIReader::read_selection() {
    std::vector<FSEntry>& fs_entries = parent->sidebar.entries;
    for (FSEntry& entry : fs_entries) {
        if (!entry.selected) { continue; }
        Reading r = reader.read(entry.image);
        entry.reading = r;
    }
    update_viewport();
}

void UIReader::read_all() {
    std::vector<FSEntry>& fs_entries = parent->sidebar.entries;
    for (FSEntry& entry : fs_entries) {
        Reading r = reader.read(entry.image);
        entry.reading = r;
    }
    update_viewport();
}


// TODO: there must be a better way!
void UIReader::update_viewport() {
    std::vector<FSEntry> fs_entries = parent->sidebar.entries;
    int selected_entry = parent->sidebar.selected_entry;
    if (selected_entry < 0) {
        BeginTextureMode(viewport);
        ClearBackground(BLACK);
        EndTextureMode();
        return;
    }

    BeginTextureMode(viewport);
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


void UIReader::fs_updated() { update_viewport(); }


void UIReader::draw() {
    std::vector<FSEntry>& fs_entries = parent->sidebar.entries;
    int selected_entry = parent->sidebar.selected_entry;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 4});
    ImGui::BeginChild("Leitor");
    ImGui::Columns(2, "LeitorCols");

    /* ===== PAINEL ESQUERDO ===== */
    ImGui::BeginChild("ReaderLeftPanel");
    ImGui::SeparatorText("Cabeçalho");

    float windowWidth = ImGui::GetContentRegionMax().x;
    if (selected_entry > -1) {
        ImGui::PushItemWidth(windowWidth);
        Reading& reading = fs_entries[selected_entry].reading;

        // Input de headers
        for (Header& head : reading.headers) {
            std::string upper = head.name;
            if (head.name.length() > 0) {
                head.name[0] = toupper(upper[0]);
            }
            InputTextTitle(head.name.c_str(), head.content.data(), HEADER_CONTENT_MAX_CHARS);
        }
        ImGui::Spacing();
        // Código de barras lido
        ImGui::Text("\uf05a  Código de barras: %s", reading.barcode_string.c_str());
        ImGui::PopItemWidth();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {0, 0});
        ImGui::SeparatorText("Gabarito");
        ImGui::PushFont(parent->monospace_font);
        ImGui::Text("Gabarito: %s", reading.get_answer_string().c_str());
        // Permite mudar cada item do gabarito com uma combo box
        for (size_t i = 0; i < reading.items.size(); i++) {
            Item& item = reading.items[i];
            char label[21];
            char preview[2];
            sprintf(label, "%02zu", i); // número do item
            sprintf(preview, "%c", item.choice); // item selecionado

            ImGui::SetNextItemWidth(24);
            if (ImGui::BeginCombo(label, preview, ImGuiComboFlags_NoArrowButton)) {
                // um selectable pra cada opção
                for (int n = 0; n < IM_ARRAYSIZE(ITEMS_STR) - 1; n++) {
                    bool is_selected = (ITEMS_STR[n] == item.choice);
                    char s_label[2];
                    sprintf(s_label, "%c", ITEMS_STR[n]);

                    if (ImGui::Selectable(s_label, is_selected)) { item.choice = ITEMS_STR[n]; }
                    if (is_selected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            ImGui::Spacing();
            if (i % 10 != 9 && i < reading.items.size()) { ImGui::SameLine(); }
        }
        ImGui::PopFont();
        ImGui::PopStyleVar(2);

        ImGui::SeparatorText("Diagnóstico");
        // Mostra todos os avisos da leitura
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
    ImVec2 image_size = {(float)viewport.texture.width, (float)viewport.texture.height};

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
    Vector2 texture_local_size = {(float)viewport.texture.width, (float)viewport.texture.height};
    Vector2 texture_global_size = {scaled_image_size.x, scaled_image_size.y};
    Vector2 texture_scale = Vector2Divide(texture_local_size, texture_global_size);
    Vector2 mouse_local_00 = Vector2Subtract(mouse_global_pos, texture_global_pos);
    Vector2 mouse_local_pos = Vector2Multiply(mouse_local_00, texture_scale);

    // Desenha um viewport com um preview do gabarito na tela
    ImGui::GetWindowDrawList()->AddImage(ImTextureID(viewport.texture.id), image_draw_pos,
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
    FileDialogBar(&participants_file_dialog, participant_table_path.data(),
                  participant_table_path.capacity(), true, "ChooseFileDlgKey",
                  "Escola um arquivo (.json, .csv)", "Arquivos de dados (.json, .csv){.json,.csv}");
    ImGui::Spacing();

    ImGui::Text("Tabela de respostas");
    ImGui::PushID("tabela de respostas");
    FileDialogBar(&answers_file_dialog, answers_table_path.data(), answers_table_path.capacity(),
                  true, "ChooseFileDlgKey", "Escola um arquivo (.json, .csv)",
                  "Arquivos de dados (.json, .csv){.json,.csv}");
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