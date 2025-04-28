#include <string>

#include "../reader.hpp"
#include "ImGuiFileDialog.h"
#include "raylib.h"

#define UI_PATH_MAX_SIZE 501

struct FSEntry {
    std::string path;
    // std::string name;
    Image image;
    Reading reading;
    bool selected;
    bool checked;

    bool operator<(const FSEntry& other) const { return this->path < other.path; }
    bool operator>(const FSEntry& other) const { return this->path > other.path; }
};

bool CustomCheckbox(const char* label, bool* v);
bool InputTextTitle(const char* label, char* buf, size_t buf_size);
bool FileDialogBar(ImGuiFileDialog* instance, char* buf, size_t max_size, bool reload_button = true,
                   const std::string& vKey = "ChooseDirDlgKey",
                   const std::string& vTitle = "Escolha uma pasta", const char* vFilters = nullptr);

struct UserInterface {
private:
    static void update_style();
    
    void draw_sidebar();
    void draw_reader();
    void draw_scanner();
    void draw_generator();
    void update_viewport();
    
    void read_current();
    void read_selection();
    void read_all();

    void update_fs();
    void sort_fs();
    void delete_selected_files();
    bool entry_has_warnings(const FSEntry& entry);
    bool entry_has_empty_headers(const FSEntry& entry);
    
    void check_all_entries();
    void select_all_entries();
    void update_all_checked();
    void update_all_selected();
    
    bool _all_entries_selected = false;
    bool _all_entries_checked = false;
    bool _entry_order_ascending = true;
    bool _layout_initialized = false;

    std::string _scans_directory = "resources/scans_teste_oci";
    std::string _participant_table_path = "";
    std::string _answers_table_path = "";
    
    ImGuiFileDialog _scans_directory_file_dialog_instance;
    ImGuiFileDialog _participant_file_dialog_instance;
    ImGuiFileDialog _answers_file_dialog_instance;

    RenderTexture2D _viewport = LoadRenderTexture(1264, 1760);

public:
    std::vector<FSEntry> fs_entries;
    int selected_entry = -1;

    Reader reader = {};

    UserInterface();

    void draw();
};