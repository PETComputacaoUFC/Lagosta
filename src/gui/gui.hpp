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

struct UserInterface {
private:
    static void update_style();
    void draw_sidebar();
    void draw_reader();
    void draw_scanner();
    void draw_generator();
    void draw_viewport();
    bool all_entries_selected = false;
    bool all_entries_checked = false;
    bool entry_order_ascending = true;
    bool layout_initialized = false;

public:
    std::vector<FSEntry> fs_entries;
    int selected_entry = -1;

    RenderTexture2D rl_viewport = LoadRenderTexture(1264, 1760);
    Reader reader = {};

    UserInterface();

    void draw();
};