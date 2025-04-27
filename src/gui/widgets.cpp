#include <utility>

#include "ImGuiFileDialog.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "gui.hpp"
#include "imgui.h"
#include "imgui_internal.h"

// default checkbox but with white square instead of check. ugh.
bool CustomCheckbox(const char* label, bool* v) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    const float square_sz = ImGui::GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(
        pos,
        pos
            + ImVec2(
                square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f),
                label_size.y + style.FramePadding.y * 2.0f));
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    const bool is_visible = ImGui::ItemAdd(total_bb, id);
    const bool is_multi_select = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
    if (!is_visible)
        if (!is_multi_select || !g.BoxSelectState.UnclipMode
            || !g.BoxSelectState.UnclipRect.Overlaps(
                total_bb))  // Extra layer of "no logic clip" for box-select support
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label,
                                        g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable
                                            | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

    // Range-Selection/Multi-selection support (header)
    bool checked = *v;
    if (is_multi_select) ImGui::MultiSelectItemHeader(id, &checked, NULL);

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

    // Range-Selection/Multi-selection support (footer)
    if (is_multi_select)
        ImGui::MultiSelectItemFooter(id, &checked, &pressed);
    else if (pressed)
        checked = !checked;

    if (*v != checked) {
        *v = checked;
        pressed = true;  // return value
        ImGui::MarkItemEdited(id);
    }

    const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
    const bool mixed_value = (g.LastItemData.ItemFlags & ImGuiItemFlags_MixedValue) != 0;
    if (is_visible) {
        ImGui::RenderNavCursor(total_bb, id);
        ImGui::RenderFrame(check_bb.Min, check_bb.Max,
                           ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive
                                              : hovered         ? ImGuiCol_FrameBgHovered
                                                                : ImGuiCol_FrameBg),
                           true, style.FrameRounding);
        ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
        if (mixed_value) {
            // Undocumented tristate/mixed/indeterminate checkbox (#2644)
            // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue
            // supported by all widgets (not just checkbox)
            ImVec2 pad(ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)),
                       ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col,
                                            style.FrameRounding);
        } else if (*v) {
            const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 6.0f)) * 1.5f;
            window->DrawList->AddRectFilled(check_bb.Min + ImVec2(pad, pad),
                                            check_bb.Max - ImVec2(pad, pad),
                                            IM_COL32(255, 255, 255, 255), style.FrameRounding);
        }
    }
    const ImVec2 label_pos =
        ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
    if (g.LogEnabled) ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
    if (is_visible && label_size.x > 0.0f) ImGui::RenderText(label_pos, label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label,
                                g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable
                                    | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool InputTextTitle(const char* label, char* buf, size_t buf_size) {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 0});
    ImGui::Text("%s", label);
    ImGui::PushID(label);
    bool b = ImGui::InputText("", buf, buf_size);
    ImGui::PopID();
    ImGui::PopStyleVar();
    ImGui::Spacing();
    return b;
}

static char cu[101] = "";
std::pair<bool, std::string> DirectoryChooser() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {5, 2});

    ImVec2 ws = ImGui::GetContentRegionAvail();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
    if (ImGui::Button("\uf07b")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.flags = ImGuiFileDialogFlags_DisableCreateDirectoryButton
                     | ImGuiFileDialogFlags_DisableQuickPathSelection | ImGuiFileDialogFlags_Modal;
        ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose Directory", nullptr,
                                                config);
    }
    ImVec2 bs = ImGui::GetItemRectSize();
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ws.x - bs.x * 2 - 1);
    ImGui::InputText("##51231", cu, 101);
    ImGui::SameLine();
    ImGui::Button("\uf2f9");
    ImGui::PopStyleVar();

    // FileDialog
    std::string path = "";
    bool update = false;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 2});
    if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey", ImGuiWindowFlags_NoCollapse,
                                             {600, 400})) {  // => will show a dialog
        if (ImGuiFileDialog::Instance()->IsOk()) {
            path = ImGuiFileDialog::Instance()->GetCurrentPath();
            update = true;
        }

        ImGuiFileDialog::Instance()->Close();
    }
    ImGui::PopStyleVar(3);

    return make_pair(update, path);
}