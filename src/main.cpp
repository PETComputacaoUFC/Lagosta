#include <cstdio>

#include "raylib.h"
#include "reader.hpp"
#define CLAY_IMPLEMENTATION
#include "ZXing/ReadBarcode.h"
#include "clay.h"
#include "clay_renderer_raylib.c"


void HandleClayErrors(Clay_ErrorData errorData) { printf("%s", errorData.errorText.chars); }

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
    Image img1 = LoadImage("resources/qr_test/qr_test02.png");
    // O leitor da biblioteca espera que a imagem input seja grayscale
    ImageFormat(&img1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
    unsigned char* img_data = (unsigned char*)img1.data;

    auto image = ZXing::ImageView(img_data, img1.width, img1.height, ZXing::ImageFormat::Lum);
    auto options = ZXing::ReaderOptions().setFormats(ZXing::BarcodeFormat::Aztec);
    auto barcode = ZXing::ReadBarcode(image, options);
    printf("Leitura Aztec: %s\n", barcode.text().c_str());

    Reader reader{};
    Reading reading = reader.read(img1);
    printf("Leitura gabarito: %s\n", reading.answer_string.c_str());
    
    reader.image_filter1(&img1);
    Texture2D texture = LoadTextureFromImage(img1);
    
    Camera2D camera = {};
    camera.zoom = 0.8f;

    while (!WindowShouldClose()) {
        // ==== DRAW ====
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        DrawTexture(texture, 0, 0, WHITE);
        reader.draw_reading(reading);

        EndMode2D();
        EndDrawing();
    }

    Clay_Raylib_Close();
}
