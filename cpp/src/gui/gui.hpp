#include <string>

#include "../reader.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "raylib.h"

#define CATPPUCCIN_PEACH  (ImVec4){0.99f, 0.39f, 0.04f, 1.00f}
#define CATPPUCCIN_YELLOW (ImVec4){0.87f, 0.56f, 0.11f, 1.00f}
#define CATPPUCCIN_GREEN  (ImVec4){0.250f, 0.627f, 0.169f, 1.000f}
#define CATPPUCCIN_RED    (ImVec4){0.8235f, 0.0588f, 0.2235f, 1.0000f}

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
//

struct UIWindow;

class UIElement {
public:
    std::string id;
    UIWindow* parent;
    virtual void fs_updated() = 0;
    virtual void draw() = 0;
};


// Sidebar - File system
#define UI_PATH_MAX_SIZE 501
class UIFilesystem : public UIElement {
public:
    std::string id = "sidebar";
    std::string directory;

    bool order_ascending = true;  // true: ascending; false: descending
    bool all_selected = false;
    bool all_checked = false;

    std::vector<FSEntry> entries;
    int selected_entry = -1;

    ImGuiFileDialog file_dialog;

    UIFilesystem();

    bool select_all();
    bool check_all();
    bool update_all_checked();
    bool update_all_selected();

    bool entry_has_warnings(const FSEntry& entry);
    bool entry_has_empty_headers(const FSEntry& entry);

    void delete_selected();

    void reload_fs();
    void sort_fs();

    void fs_updated() override {}
    void draw() override;

    ~UIFilesystem() {};
};


// Aba "leitor" - Leitor de gabaritos
class UIReader : public UIElement {
public:
    std::string id = "reader";
    RenderTexture2D viewport = LoadRenderTexture(1264, 1760);

    std::string participant_table_path = "";
    std::string answers_table_path = "";

    ImGuiFileDialog participants_file_dialog;
    ImGuiFileDialog answers_file_dialog;

    Reader reader;

    UIReader() {};

    void read_current();
    void read_selection();
    void read_all();

    void update_viewport();

    void fs_updated() override;
    void draw() override;

    ~UIReader() {};
};


// Aba "scanner" - Integração com o scanner (somente Linux)
class UIScanner : public UIElement {
public:
    std::string id = "scanner";
    Reader reader;
    ImGuiFileDialog file_dialog;

    void update_viewport();

    void draw() override;

    ~UIScanner() {};
};


// Aba "editor" - Editor de gabaritos
class UIEditor : public UIElement {
public:
    std::string id = "editor";
    ImGuiFileDialog file_dialog;

    void draw() override;
};



struct UIWindow {
private:
    std::vector<UIElement*> tabs;

public:
    UIFilesystem sidebar;
    size_t selected_tab = 0;
    bool layout_initialized = false;

    ImFont* monospace_font;

    UIWindow();

    void update_style();

    void add_tab(UIElement* tab) {
        tab->parent = this;
        tabs.push_back(tab);
    }

    void fs_updated();

    void draw();
};