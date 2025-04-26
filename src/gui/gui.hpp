#include <string>
#include "../reader.hpp"
#include "raylib.h"

void UpdateStyle();

struct FSEntry {
    std::string name;
    Image image;
    Reading reading;
};

struct UserInterface {
private:
    void draw_sidebar();
    void draw_reader();
    void draw_scanner();
    void draw_generator();

public:
    std::vector<FSEntry> fs_entries;
    int selected_entry = -1;

    bool layout_init = false;
    RenderTexture2D rl_viewport = LoadRenderTexture(1264, 1760);
    Reader reader = {};

    UserInterface();

    void draw();
};