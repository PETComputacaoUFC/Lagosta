#ifndef LIB_H
#define LIB_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int erro;
    int id_prova;
    int id_participante;
    char* leitura;
} Reading;

Reading read_image_path(const char* path);
Reading read_image_data(const char* file_type, const unsigned char* file_data, int file_data_size);

#ifdef __cplusplus
}
#endif

#endif // LIB_H
