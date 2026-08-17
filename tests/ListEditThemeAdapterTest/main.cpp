#include <Core/Core.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

namespace {
int checks = 0;
int failed = 0;

void Check(bool ok, const String& what)
{
    checks++;
    if(!ok) {
        failed++;
        Cout() << "FAIL: " << what << '\n';
    }
}

const UiDesignerThemeOverrideSpec* Find(const UiDesignerControlSpec& spec,
                                        const char *id)
{
    return spec.FindThemeOverride(id);
}

void CheckEntry(const UiDesignerControlSpec& spec, const char *id,
                const char *label, const char *group,
                PropertyEditorKind kind = PropertyEditorKind::Text,
                bool check_kind = false)
{
    const UiDesignerThemeOverrideSpec *p = Find(spec, id);
    Check(p != nullptr, String(id) + " exists");
    if(!p)
        return;
    Check(p->label == label, String(id) + " label");
    Check(p->group == group, String(id) + " group");
    Check(p->adapter_field_id == id, String(id) + " adapter field");
    if(check_kind)
        Check(p->kind == kind, String(id) + " editor kind");
}

Vector<String> GroupSequence(const UiDesignerControlSpec& spec)
{
    Vector<String> out;
    for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
        if(out.IsEmpty() || out.Top() != p.group)
            out.Add(p.group);
    return out;
}

bool SameSequence(const Vector<String>& a, const Vector<String>& b)
{
    if(a.GetCount() != b.GetCount())
        return false;
    for(int i = 0; i < a.GetCount(); i++)
        if(a[i] != b[i])
            return false;
    return true;
}

Vector<String> MakeSequence(std::initializer_list<const char *> names)
{
    Vector<String> out;
    for(const char *name : names)
        out.Add(name);
    return out;
}
}

CONSOLE_APP_MAIN
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    String error;
    Check(catalog.Validate(error), "catalog validates: " + error);

    const UiDesignerControlSpec *list = catalog.Find("UiList");
    Check(list != nullptr, "UiList catalog spec exists");
    if(list) {
        Check(list->theme_adapter_id == "list", "UiList keeps list adapter id");
        const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*list);
        Check(adapter == &UiDesignerListThemeAdapterInstance(),
              "UiList resolves normalized dedicated adapter");
        Check(adapter && adapter->Supports(UiDesignerRuntimeKind::UiList),
              "List adapter supports UiList");
        Check(Find(*list, "skin_image") == nullptr,
              "List does not fake resource-backed Skin editing");
        CheckEntry(*list, "radius", "Radius", "General");
        CheckEntry(*list, "face.normal", "Normal", "Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*list, "frame.normal", "Normal", "Frame");
        CheckEntry(*list, "ink", "Normal", "Ink");
        CheckEntry(*list, "row_height", "Height", "Rows/Layout");
        CheckEntry(*list, "selected_face", "Selected Face", "Rows/State");
        CheckEntry(*list, "show_icons", "Show Icons", "Content");
        CheckEntry(*list, "right_text_as_badge", "Enabled", "Badge");
        CheckEntry(*list, "drag_side", "Side", "Drag");

        Check(SameSequence(GroupSequence(*list), MakeSequence({
                  "General", "Face", "Frame", "Ink", "Icon", "Typography",
                  "Content Margin", "Focus", "Shadow", "Highlight",
                  "Rows/Layout", "Rows/State", "Content", "Badge", "Drag" })),
              "List group sequence follows normalized outer-to-row contract");

        if(adapter) {
            UiDesignerNode node;
            node.id = 21;
            node.type = "UiList";
            node.theme_overrides.Set("selected_face", Color(10, 20, 30));
            UiDesignerFillRecipe recipe;
            recipe.mode = "QuadGradient";
            recipe.top_left = Color(1, 2, 3);
            recipe.top_right = Color(4, 5, 6);
            recipe.bottom_left = Color(7, 8, 9);
            recipe.bottom_right = Color(10, 11, 12);
            recipe.tile_size = 24;
            recipe.blur = 3;
            node.theme_overrides.Set("face.normal", recipe.ToValue());

            UiDesignerFillRecipe resolved = UiDesignerFillRecipe::FromValue(
                adapter->ResolveFieldValue(node, *list, "face.normal"));
            Check(resolved.mode == "QuadGradient" &&
                      resolved.top_left == Color(1, 2, 3) &&
                      resolved.bottom_right == Color(10, 11, 12) &&
                      resolved.tile_size == 24 && resolved.blur == 3,
                  "List Face QuadGradient resolves without lossy UiFill round-trip");
            Check(Color(adapter->ResolveFieldValue(node, *list, "selected_face")) ==
                      Color(10, 20, 30),
                  "List row-state override resolves through dedicated adapter");
            String code;
            adapter->EmitSetup(code, "list", node, *list);
            Check(code.Find("UiList::Style list_style = UiTheme::ResolveList();") >= 0,
                  "List codegen starts from inherited style");
            Check(code.Find("MakeQuadGradientTile") >= 0,
                  "List codegen emits authored Face FillRecipe");
            Check(code.Find("list_style.selected_face = Color(10, 20, 30);") >= 0,
                  "List codegen emits row-state override");
            Check(code.Find("list.SetCustomStyle(list_style);") >= 0,
                  "List codegen applies custom style");
        }
    }

    const UiDesignerControlSpec *edit = catalog.Find("UiLineEdit");
    Check(edit != nullptr, "UiLineEdit catalog spec exists");
    if(edit) {
        Check(edit->theme_adapter_id == "edit", "UiLineEdit keeps edit adapter id");
        const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*edit);
        Check(adapter == &UiDesignerEditThemeAdapterInstance(),
              "UiLineEdit resolves normalized shared Edit adapter");
        Check(adapter && adapter->Supports(UiDesignerRuntimeKind::UiMultiEdit) &&
                      adapter->Supports(UiDesignerRuntimeKind::UiPasswordEdit),
              "Edit adapter still supports the full UiBaseEdit family");
        Check(Find(*edit, "skin_image") == nullptr,
              "Edit does not fake resource-backed Skin editing");
        CheckEntry(*edit, "text_align", "Text Align", "General");
        CheckEntry(*edit, "face_normal", "Normal", "Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*edit, "frame_normal", "Normal", "Frame");
        CheckEntry(*edit, "text_normal", "Normal", "Ink");
        CheckEntry(*edit, "font_size", "Font height", "Typography");
        CheckEntry(*edit, "margin_left", "Left", "Content Margin");
        CheckEntry(*edit, "caret_color", "Caret Colour", "Editing");
        CheckEntry(*edit, "underline_normal", "Normal", "Underline");
        CheckEntry(*edit, "show_tabs", "Show Tabs", "Whitespace");
        CheckEntry(*edit, "focus_enabled", "Enabled", "Focus");
        CheckEntry(*edit, "shadow_enabled", "Enabled", "Shadow");
        CheckEntry(*edit, "highlight_enabled", "Enabled", "Highlight");

        Check(SameSequence(GroupSequence(*edit), MakeSequence({
                  "General", "Face", "Frame", "Ink", "Typography",
                  "Content Margin", "Editing", "Underline", "Whitespace",
                  "Focus", "Shadow", "Highlight" })),
              "Edit group sequence follows normalized contract");
        Check(Find(*edit, "face_normal") != nullptr &&
                  Find(*edit, "text_normal") != nullptr &&
                  Find(*edit, "underline_normal") != nullptr,
              "Edit preserves existing authored field ids while normalizing presentation");

        if(adapter) {
            UiDesignerNode node;
            node.id = 22;
            node.type = "UiLineEdit";
            node.SetProperty("role", "Accent");
            node.theme_overrides.Set("caret_width", 4);
            node.theme_overrides.Set("show_spaces", true);
            UiDesignerFillRecipe recipe;
            recipe.mode = "QuadGradient";
            recipe.top_left = Color(20, 30, 40);
            recipe.top_right = Color(50, 60, 70);
            recipe.bottom_left = Color(80, 90, 100);
            recipe.bottom_right = Color(110, 120, 130);
            recipe.tile_size = 30;
            recipe.blur = 1;
            node.theme_overrides.Set("face_normal", recipe.ToValue());

            UiDesignerFillRecipe resolved = UiDesignerFillRecipe::FromValue(
                adapter->ResolveFieldValue(node, *edit, "face_normal"));
            Check(resolved.mode == "QuadGradient" && resolved.tile_size == 30 &&
                      resolved.bottom_right == Color(110, 120, 130),
                  "Edit Face QuadGradient resolves without lossy UiFill round-trip");
            Check((int)adapter->ResolveFieldValue(node, *edit, "caret_width") == 4,
                  "Edit caret width override resolves");
            Check((bool)adapter->ResolveFieldValue(node, *edit, "show_spaces"),
                  "Edit whitespace override resolves");
            String code;
            adapter->EmitSetup(code, "edit", node, *edit);
            Check(code.Find("UiTheme::ResolveEdit(UiTheme::GetContext(), UiRole::Accent)") >= 0,
                  "Edit codegen preserves semantic role inheritance");
            Check(code.Find("MakeQuadGradientTile") >= 0,
                  "Edit codegen emits authored Face FillRecipe");
            Check(code.Find("edit_style.caret_width = 4;") >= 0,
                  "Edit codegen emits Editing field");
            Check(code.Find("edit_style.show_spaces = true;") >= 0,
                  "Edit codegen emits Whitespace field");
            Check(code.Find("edit.SetCustomStyle(edit_style);") >= 0,
                  "Edit codegen applies custom style");
        }
    }

    Cout() << "LIST_EDIT_THEME_ADAPTER_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
