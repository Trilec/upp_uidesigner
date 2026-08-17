#include <Core/Core.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

namespace {
int checks = 0;
int failed = 0;

void Check(bool ok, const String& message)
{
    checks++;
    if(!ok) {
        failed++;
        Cout() << "FAIL: " << message << '\n';
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
    if(!p) return;
    Check(p->label == label, String(id) + " label");
    Check(p->group == group, String(id) + " group");
    if(check_kind)
        Check(p->kind == kind, String(id) + " editor kind");
}

Vector<String> Groups(const UiDesignerControlSpec& spec)
{
    Vector<String> out;
    for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
        if(out.IsEmpty() || out.Top() != p.group)
            out.Add(p.group);
    return out;
}

bool SameGroups(const Vector<String>& got, const Vector<String>& expected)
{
    if(got.GetCount() != expected.GetCount()) return false;
    for(int i = 0; i < got.GetCount(); i++)
        if(got[i] != expected[i]) return false;
    return true;
}
}

CONSOLE_APP_MAIN
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    String error;
    Check(catalog.Validate(error), "catalog validates: " + error);

    const UiDesignerControlSpec *dropdown = catalog.Find("UiDropdown");
    Check(dropdown != nullptr, "UiDropdown catalog spec exists");
    if(dropdown) {
        const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*dropdown);
        Check(adapter == &UiDesignerDropdownThemeAdapterInstance(),
              "Dropdown resolves dedicated normalized adapter");
        Check(dropdown->theme_adapter_id == "dropdown",
              "Dropdown preserves adapter id");

        CheckEntry(*dropdown, "face_normal", "Normal", "Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*dropdown, "frame_normal", "Normal", "Frame");
        CheckEntry(*dropdown, "ink_normal", "Normal", "Ink");
        CheckEntry(*dropdown, "face_enabled", "Enabled", "Face");
        CheckEntry(*dropdown, "frame_enabled", "Enabled", "Frame");
        CheckEntry(*dropdown, "frame_width", "Width", "Frame");
        CheckEntry(*dropdown, "popup_item_face.normal", "Normal", "Popup/Items/Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*dropdown, "popup_background_color", "Background", "Popup/Face");
        CheckEntry(*dropdown, "popup_frame_width", "Width", "Popup/Frame");
        CheckEntry(*dropdown, "show_popup_selection_marker", "Show Selection Marker", "Popup/Marker");
        CheckEntry(*dropdown, "show_selection_badge", "Show Badge", "Popup/Badge");
        CheckEntry(*dropdown, "drag_side", "Side", "Drag");
        Check(Find(*dropdown, "skin_image") == nullptr,
              "Dropdown does not fake document-resource Skin editing");

        Vector<String> expected;
        expected << "General" << "Face" << "Frame" << "Ink" << "Typography"
                 << "Content Margin" << "Layout" << "Indicator" << "Focus"
                 << "Shadow" << "Highlight" << "Popup/Layout" << "Popup/Face"
                 << "Popup/Frame" << "Popup/Items" << "Popup/Items/Face"
                 << "Popup/Items/Frame" << "Popup/Items/Ink"
                 << "Popup/Items/Typography" << "Popup/Items/Content Margin"
                 << "Popup/Marker" << "Popup/Badge" << "Drag";
        Check(SameGroups(Groups(*dropdown), expected),
              "Dropdown group sequence preserves nested Popup domains");

        if(adapter) {
            UiDesignerNode node;
            node.id = 31;
            node.type = "UiDropdown";
            node.SetProperty("role", "Accent");
            node.theme_overrides.Set("face_normal", Color(12, 34, 56));
            node.theme_overrides.Set("popup_item_frame_width", 3);
            node.theme_overrides.Set("popup_marker_side", "Left");

            UiDesignerFillRecipe legacy = UiDesignerFillRecipe::FromValue(
                adapter->ResolveFieldValue(node, *dropdown, "face_normal"));
            Check(legacy.mode == "Solid" && legacy.solid == Color(12, 34, 56),
                  "legacy Dropdown Color face upgrades to Solid FillRecipe");

            UiDesignerFillRecipe recipe;
            recipe.mode = "QuadGradient";
            recipe.top_left = Color(1, 10, 20);
            recipe.top_right = Color(30, 40, 50);
            recipe.bottom_left = Color(60, 70, 80);
            recipe.bottom_right = Color(90, 100, 110);
            recipe.tile_size = 28;
            recipe.blur = 2;
            node.theme_overrides.Set("popup_item_face.normal", recipe.ToValue());
            UiDesignerFillRecipe resolved = UiDesignerFillRecipe::FromValue(
                adapter->ResolveFieldValue(node, *dropdown, "popup_item_face.normal"));
            Check(resolved.mode == "QuadGradient" && resolved.tile_size == 28 &&
                      resolved.bottom_right == Color(90, 100, 110),
                  "Dropdown popup item QuadGradient resolves losslessly");

            String code;
            adapter->EmitSetup(code, "dropdown", node, *dropdown);
            Check(code.Find("UiTheme::ResolveDropdown(UiTheme::GetContext(), UiRole::Accent)") >= 0,
                  "Dropdown codegen preserves role inheritance");
            Check(code.Find("UiFill::Solid(Color(12, 34, 56))") >= 0,
                  "Dropdown codegen upgrades legacy Color face");
            Check(code.Find("popup_item_style.palette.face[ST_NORMAL]") >= 0 &&
                      code.Find("MakeQuadGradientTile") >= 0,
                  "Dropdown codegen emits nested popup item FillRecipe");
            Check(code.Find("popup_item_style.metrics.frame_width = 3;") >= 0,
                  "Dropdown codegen writes nested popup item metrics");
            Check(code.Find("popup_marker_side = UiAlign::LEFT") >= 0,
                  "Dropdown codegen writes popup marker domain");
        }
    }

    const UiDesignerControlSpec *accordion = catalog.Find("UiAccordion");
    Check(accordion != nullptr, "UiAccordion catalog spec exists");
    if(accordion) {
        const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*accordion);
        Check(adapter == &UiDesignerAccordionThemeAdapterInstance(),
              "Accordion resolves dedicated normalized adapter");
        Check(accordion->theme_adapter_id == "accordion",
              "Accordion preserves adapter id");

        CheckEntry(*accordion, "face_normal", "Normal", "Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*accordion, "style_header_height", "Header Height", "Layout");
        CheckEntry(*accordion, "style_item_spacing", "Item Spacing", "Layout");
        CheckEntry(*accordion, "style_single_open", "Single Open", "Behaviour");
        CheckEntry(*accordion, "style_animation_enabled", "Enabled", "Animation");
        CheckEntry(*accordion, "header_face.normal", "Normal", "Header/Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*accordion, "header_title_color", "Title", "Header/Ink");
        CheckEntry(*accordion, "style_show_chevron", "Show", "Header/Chevron");
        CheckEntry(*accordion, "style_show_drag_handle", "Show Handle", "Header/Drag");
        CheckEntry(*accordion, "body_face.normal", "Normal", "Body/Face",
                   PropertyEditorKind::FillRecipe, true);
        CheckEntry(*accordion, "body_line_extent", "Extent", "Body/Line");
        Check(Find(*accordion, "header_skin_image") == nullptr &&
                  Find(*accordion, "body_skin_image") == nullptr,
              "Accordion does not fake resource-backed nested Skin editing");

        Vector<String> expected;
        expected << "General" << "Face" << "Frame" << "Ink" << "Shadow"
                 << "Highlight" << "Layout" << "Section" << "Header/Face"
                 << "Header/Frame" << "Header/Ink" << "Header/Typography"
                 << "Header/Content Margin" << "Header/Chevron" << "Header/Drag"
                 << "Body/Face" << "Body/Frame" << "Body/Content Margin"
                 << "Body/Line" << "Behaviour" << "Animation";
        Check(SameGroups(Groups(*accordion), expected),
              "Accordion group sequence preserves Header/Body composition");

        if(adapter) {
            UiDesignerNode node;
            node.id = 32;
            node.type = "UiAccordion";
            node.theme_overrides.Set("face_normal", Color(22, 44, 66));
            node.theme_overrides.Set("style_single_open", true);
            node.theme_overrides.Set("style_chevron_gap", 13);
            node.theme_overrides.Set("body_line_extent", "Medium");
            node.theme_overrides.Set("body_line_style", "Dotted");

            UiDesignerFillRecipe legacy = UiDesignerFillRecipe::FromValue(
                adapter->ResolveFieldValue(node, *accordion, "face_normal"));
            Check(legacy.mode == "Solid" && legacy.solid == Color(22, 44, 66),
                  "legacy Accordion Color face upgrades to Solid FillRecipe");

            UiDesignerFillRecipe header;
            header.mode = "QuadGradient";
            header.top_left = Color(5, 15, 25);
            header.top_right = Color(35, 45, 55);
            header.bottom_left = Color(65, 75, 85);
            header.bottom_right = Color(95, 105, 115);
            header.tile_size = 26;
            header.blur = 1;
            node.theme_overrides.Set("header_face.normal", header.ToValue());
            UiDesignerFillRecipe resolved = UiDesignerFillRecipe::FromValue(
                adapter->ResolveFieldValue(node, *accordion, "header_face.normal"));
            Check(resolved.mode == "QuadGradient" && resolved.tile_size == 26 &&
                      resolved.top_right == Color(35, 45, 55),
                  "Accordion Header QuadGradient resolves losslessly");
            Check((bool)adapter->ResolveFieldValue(node, *accordion, "style_single_open"),
                  "Accordion preserved behaviour field resolves");
            Check(AsString(adapter->ResolveFieldValue(node, *accordion, "body_line_extent")) == "Medium",
                  "Accordion Body line extent resolves shared UiSpan vocabulary");

            String code;
            adapter->EmitSetup(code, "accordion", node, *accordion);
            Check(code.Find("UiAccordion::Style accordion_style = UiAccordion::StyleDefault();") >= 0 &&
                      code.Find("UiTheme::ResolveTitleCard(UiRole::Accent)") >= 0 &&
                      code.Find("UiTheme::ResolvePanel(UiPanelRole::Surface)") >= 0,
                  "Accordion codegen reconstructs theme-derived composite base");
            Check(code.Find("header_style.palette.face[ST_NORMAL]") >= 0 &&
                      code.Find("MakeQuadGradientTile") >= 0,
                  "Accordion codegen emits nested Header FillRecipe");
            Check(code.Find("single_open = true") >= 0 &&
                      code.Find("chevron_gap = 13") >= 0,
                  "Accordion codegen emits behaviour and Header geometry");
            Check(code.Find("body_line_extent = MEDIUM") >= 0 &&
                      code.Find("body_line_style = DOTTED") >= 0,
                  "Accordion codegen emits canonical Body line enums");
            Check(code.Find("UiDesigner") < 0,
                  "generated Accordion setup has no Designer runtime dependency");
        }
    }

    const UiDesignerControlSpec *edit = catalog.Find("UiLineEdit");
    if(edit) {
        const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*edit);
        UiDesignerNode node;
        node.id = 33;
        node.type = "UiLineEdit";
        node.theme_overrides.Set("face_normal", Color(9, 19, 29));
        UiDesignerFillRecipe legacy = adapter ? UiDesignerFillRecipe::FromValue(
            adapter->ResolveFieldValue(node, *edit, "face_normal")) : UiDesignerFillRecipe();
        Check(adapter && legacy.mode == "Solid" && legacy.solid == Color(9, 19, 29),
              "shared FillRecipe bridge preserves legacy Edit Color overrides");
        String code;
        if(adapter) adapter->EmitSetup(code, "edit", node, *edit);
        Check(code.Find("UiFill::Solid(Color(9, 19, 29))") >= 0,
              "Edit codegen upgrades legacy Color face after common bridge correction");
    }

    Cout() << "DROPDOWN_ACCORDION_THEME_ADAPTER_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
