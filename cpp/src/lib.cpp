#include "lib.hpp"
#include "raylib.h"
#include "reader.hpp"

Reading read_image_path(const char* path) {
    SetTraceLogLevel(LOG_NONE);
    Reader reader{};
    Image img_gabarito = LoadImage(path);
    ReaderOutput reading;

    try {
        reading = reader.read(img_gabarito);
    } catch (...) {
        char d[21] = "--------------------";
        reading = {.erro = 3, .id_prova = -1, .id_participante = -1, .leitura = d};
    }
    
    return {reading.erro, reading.id_prova, reading.id_participante, reading.leitura};
}

Reading read_image_data(const char* file_type, const unsigned char* file_data, int file_data_size) {
    SetTraceLogLevel(LOG_NONE);
    Reader reader{};
    Image img_gabarito = LoadImageFromMemory(file_type, file_data, file_data_size);
    ReaderOutput reading = reader.read(img_gabarito);
    return {reading.erro, reading.id_prova, reading.id_participante, reading.leitura};
}