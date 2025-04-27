#include <string>

#include "../reader.hpp"
#include "raylib.h"


struct FSEntry {
    std::string name;
    Image image;
    Reading reading;
    bool selected;
    bool checked;
};

bool CustomCheckbox(const char *label, bool *v);
bool InputTextTitle(const char *label, char *buf, size_t buf_size);
std::pair<bool, std::string> DirectoryChooser();

struct UserInterface {
private:
    static void update_style();
    void draw_sidebar();
    void draw_reader();
    void draw_scanner();
    void draw_generator();
    void draw_viewport();
    void update_fs();
    bool _all_entries_selected = false;
    bool _all_entries_checked = false;
    bool _entry_order_ascending = true;
    bool _layout_initialized = false;
    std::string _scans_directory = "resources/scans_teste_oci";
    RenderTexture2D _viewport = LoadRenderTexture(1264, 1760);

public:
    std::vector<FSEntry> fs_entries;
    int selected_entry = -1;

    Reader reader = {};

    UserInterface();

    void draw();
};