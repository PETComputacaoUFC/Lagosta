struct Reading {
    int erro;
    int id_prova;
    int id_participante;
    char* leitura;
};

Reading read_image_path(const char* path);
Reading read_image_data(const char* file_type, const unsigned char* file_data, int file_data_size);