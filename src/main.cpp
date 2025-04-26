#include <algorithm>
#include <cmath>
#include <cstdio>

#include "imgui.h"
#include "raylib.h"
#include "reader.hpp"
#include "rlImGui.h"
#include "ui/ui.hpp"

bool ImGuiDemoOpen = true;

void SetupFontAwesome() {
    static const ImWchar icons_ranges[] = { 0xe005, 0xf8ff, 0 };
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

    io.Fonts->AddFontFromFileTTF("resources/fonts/fa-solid-900.ttf", size, &icons_config, icons_ranges);
}

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
    ImFont *nunito = imGuiIO.Fonts->AddFontFromFileTTF("resources/fonts/NunitoSans-Regular.ttf", 20.0f, nullptr, imGuiIO.Fonts->GetGlyphRangesDefault());
    imGuiIO.FontDefault = nunito;
    SetupFontAwesome();

    SetExitKey(KEY_NULL);
    UpdateStyle();

    FilePathList pathlist = LoadDirectoryFiles("resources/scans_teste_oci");
    std::vector<std::string> image_paths;
    size_t path_index = 0;
    for (size_t p = 0; p < pathlist.count; p++) { image_paths.push_back(pathlist.paths[p]); }
    std::sort(image_paths.begin(), image_paths.end());
    Image img_gabarito;
    Texture texture_gabarito;
    Reading reading;
    Reader reader{};
    RenderTexture2D rl_viewport = LoadRenderTexture(1264, 1760);
    UnloadDirectoryFiles(pathlist);

    char nome[101] = "Participante sem nome";
    char escola[101] = "Escola sem nome";
    char modalidade[21] = "Iniciação A";
    char fase[2] = "1";
    char data[11] = "01/01/1970";
    char inscricao[10] = "000000000";

    bool layout_init = false;
    bool update_reading = true;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_LEFT)) {
            if (path_index > 0) {
                path_index -= 1;
                update_reading = true;
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            if (path_index < image_paths.size() - 1) {
                path_index += 1;
                update_reading = true;
            }
        }
        
        if (update_reading) {
            update_reading = false;

            std::string img_path = image_paths[path_index];
            img_gabarito = LoadImage(img_path.c_str());
            printf("\nFILE: %s\n", img_path.c_str());

            /* ==== LEITURA GABARITO ==== */
            reading = reader.read(img_gabarito);
            printf("  > Gabarito: %s\n", reading.get_answer_string().c_str());
            printf("  > Aztec: %s\n", reading.barcode_string.c_str());
            printf("  > Warnings: ");
            for (ReadWarning w : reading.warnings) { printf("%d, ", w); }
            printf("\n");

            reader.image_filter1(&img_gabarito);
            texture_gabarito = LoadTextureFromImage(img_gabarito);

            BeginTextureMode(rl_viewport);
            ClearBackground(BLACK);
            DrawTexture(texture_gabarito, 0, 0, WHITE);
            reader.draw_reading(reading);
            EndTextureMode();
        }

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

        ImGui::BeginChild("Leitor");
            ImGui::Columns(2, "LeitorCols");
            ImGui::BeginChild("ReaderLeftPanel");
                float windowWidth = ImGui::GetContentRegionMax().x;
                ImGui::PushItemWidth(windowWidth);

                ImGui::Text("Nome");
                ImGui::InputText("##1", nome, 101);
                ImGui::Spacing();

                ImGui::Text("Escola");
                ImGui::InputText("##2", escola, 101);
                ImGui::Spacing();

                ImGui::Text("Modalidade");
                ImGui::InputText("##3", modalidade, 21);
                ImGui::Spacing();

                ImGui::Text("Fase");
                ImGui::InputText("##4", fase, 2, ImGuiInputTextFlags_CharsDecimal);
                ImGui::Spacing();

                ImGui::Text("Data");
                ImGui::InputText("##5", data, 11);
                ImGui::Spacing();
                
                ImGui::Text("Inscrição");
                ImGui::InputText("##6", inscricao, 10);
                ImGui::Spacing();

                ImGui::PopItemWidth();

                ImGui::Separator();

                // const ImVec4 CATPPUCCIN_PEACH = {0.99f, 0.39f, 0.04f, 1.00f};
                const ImVec4 CATPPUCCIN_YELLOW = {0.87f, 0.56f, 0.11f, 1.00f};
                ImGui::Text("Gabarito: %s", reading.get_answer_string().c_str());
                ImGui::Text("Código de barras: %s", reading.barcode_string.c_str());
                for (ReadWarning rw : reading.warnings) {
                    switch (rw) {
                        case TOO_MANY_NULL_CHOICES: {
                            ImGui::TextColored(CATPPUCCIN_YELLOW, "\xef\x81\xb1  Muitos itens nulos detectados.");
                            break;
                        }
                        case IMPRECISE_READING_RECTANGLE: {
                            ImGui::TextColored(CATPPUCCIN_YELLOW, "\xef\x81\xb1  A detecção de alinhamento pode ter sido imprecisa.");
                            break;
                        }
                        case BARCODE_NOT_FOUND: {
                            ImGui::TextColored(CATPPUCCIN_YELLOW, "\xef\x81\xb1  Não foi possível ler o código de barras.");
                            break;
                        }
                    }
                }
                ImGui::Button("Salvar .csv");
            ImGui::EndChild();

            ImGui::NextColumn();

            ImVec2 ws = ImGui::GetContentRegionAvail();
            ws.y -= 35;
            ImGui::SetNextWindowSize(ws);
            ImGui::BeginChild("ReaderRightPanel", {0,0}, ImGuiChildFlags_Border);                
                ImVec2 container_pos = ImGui::GetCursorScreenPos();
                ImVec2 container_size = ImGui::GetContentRegionAvail();
                ImVec2 image_size = { (float)rl_viewport.texture.width, (float)rl_viewport.texture.height };

                // Calculate the scaling factor to fit the image within the container
                float scale = container_size.x / image_size.x;

                // Calculate the size of the scaled image
                ImVec2 scaled_image_size = {image_size.x * scale, image_size.y * scale};

                // Calculate the position to draw the image
                ImVec2 image_draw_pos = container_pos;
                image_draw_pos.x += (container_size.x - scaled_image_size.x) / 2.0f;
                // image_draw_pos.y += (container_size.y - scaled_image_size.y) / 2.0f;

                // Calculate the bottom-right of the image
                ImVec2 image_draw_br = image_draw_pos;
                image_draw_br.x += scaled_image_size.x;
                image_draw_br.y += scaled_image_size.y;

                // Draws the image
                ImGui::GetWindowDrawList()->AddImage(
                    ImTextureID(rl_viewport.texture.id),
                    image_draw_pos, image_draw_br,
                    ImVec2(0, 1), ImVec2(1, 0)
                );
                // ImGui::Image(ImGuiID(rl_viewport.texture.id), scaled_image_size, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
            ImGui::Button("Repetir Leitura");
        ImGui::EndChild();

        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}