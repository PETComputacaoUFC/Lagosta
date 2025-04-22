#include "raylib.h"
#include "reader.hpp"

#include <cstdio>
#include <iostream>
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay_renderer_raylib.c"
#include "ZXing/ReadBarcode.h"


void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    Clay_Raylib_Initialize(992, 699, "Lagosta",
                           FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT
                               | FLAG_VSYNC_HINT);  // Extra parameters to this function are new
                                                    // since the video was published
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemoryTop =
        Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));
    Clay_Initialize(
        clayMemoryTop,
        (Clay_Dimensions){.width = (float)GetScreenWidth(), .height = (float)GetScreenHeight()},
        (Clay_ErrorHandler){
            HandleClayErrors});  // This final argument is new since the video was published

    // ==== Resource importing ====
    Font fonts[1];
    fonts[0] = LoadFontEx("resources/fonts/NunitoSans-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    // Carrega a imagem pelo raylib
    Image datamatrix = LoadImage("resources/datamatrix.png");
    // A leitura do código espera que a imagem input seja grayscale
    ImageFormat(&datamatrix, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

    // Ponteiro pros dados da imagem
    unsigned char* img_data = (unsigned char*) datamatrix.data;
    
    // Objeto imagem do leitor
    auto image = ZXing::ImageView(
        img_data,
        datamatrix.width,
        datamatrix.height, 
        ZXing::ImageFormat::Lum
    );
    // Opções de leitura
    auto options = ZXing::ReaderOptions()
        .setFormats(ZXing::BarcodeFormat::DataMatrix); // Especifica o formato de barcode pra ele tentar ler
    
    // Tenta ler um código de barras na imagem dadas as opções (se ele não conseguir, a string é vazia.)
    auto barcode = ZXing::ReadBarcode(image, options);
    printf("Leitura: %s\n", barcode.text().c_str());
    
    Texture2D texture = LoadTextureFromImage(datamatrix);
    Camera2D camera = {};
    camera.zoom = 0.75f;

    while (!WindowShouldClose()) {
        // ==== DRAW ====
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                DrawTexture(texture, 0, 0, WHITE);
            EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
