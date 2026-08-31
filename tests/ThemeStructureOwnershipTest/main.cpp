#include <Core/Core.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

namespace {

int checks = 0;
int failed = 0;

void Check(bool ok, const String& what)
{
    ++checks;
    if(ok)
        return;
    ++failed;
    Cout() << "FAIL: " << what << '\n';
}

void CheckNoThemeField(const UiDesignerControlSpec *spec,
                       const char *id)
{
    const String type = spec ? spec->type_id : String("missing");
    Check(spec && spec->FindThemeOverride(id) == nullptr,
          type + " does not expose structural theme field " + id);
}

void CheckNormalProperty(const UiDesignerControlSpec *spec,
                         const char *id)
{
    const String type = spec ? spec->type_id : String("missing");
    Check(spec && spec->FindProperty(id) != nullptr,
          type + " keeps authored property " + id);
}

}

CONSOLE_APP_MAIN
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);

    String error;
    Check(catalog.Validate(error), "catalog validates: " + error);

    const UiDesignerControlSpec *title = catalog.Find("UiTitleCard");
    Check(title != nullptr, "UiTitleCard exists");
    for(const char *id : {
            "show_title_line", "title_line_length", "title_line_thickness",
            "title_line_style", "show_card_line", "card_line_side",
            "card_line_length", "card_line_thickness", "card_line_gap",
            "media_side", "media_auto_fit"})
        CheckNormalProperty(title, id);
    for(const char *id : {
            "theme_media_auto_fit", "theme_title_line",
            "theme_title_line_thickness", "theme_title_line_style",
            "theme_card_line", "theme_card_line_side",
            "theme_card_line_thickness", "theme_card_line_gap"})
        CheckNoThemeField(title, id);

    const UiDesignerControlSpec *section = catalog.Find("UiAccordionSection");
    Check(section != nullptr, "UiAccordionSection exists");
    for(const char *id : {"show_title_line", "show_card_line", "card_line_side"})
        CheckNormalProperty(section, id);
    for(const char *id : {
            "theme_media_auto_fit", "theme_title_line",
            "theme_title_line_thickness", "theme_title_line_style",
            "theme_card_line", "theme_card_line_side",
            "theme_card_line_thickness", "theme_card_line_gap"})
        CheckNoThemeField(section, id);

    // Button content placement is ordinary authored Appearance state. A theme
    // can colour/style the button but cannot move its text/icon arrangement.
    for(const char *type : {"UiButton", "UiToolButton", "UiSplitButton"}) {
        const UiDesignerControlSpec *button = catalog.Find(type);
        Check(button != nullptr, String(type) + " exists");
        for(const char *id : {"align_h", "align_v", "icon_side",
                              "content_gap", "icon_render_mode"})
            CheckNormalProperty(button, id);
        for(const char *id : {"style_align_h", "style_align_v",
                              "style_icon_side", "style_content_gap",
                              "style_icon_render_mode"})
            CheckNoThemeField(button, id);
    }

    // The Tab visual family and placement are control configuration. Theme
    // styling may change colours/metrics inside that family but not select a
    // different family or move the strip/icon.
    const UiDesignerControlSpec *tab = catalog.Find("UiTab");
    Check(tab != nullptr, "UiTab exists");
    for(const char *id : {"visual", "placement", "tab_icon_side"})
        CheckNormalProperty(tab, id);
    for(const char *id : {"style_visual", "icon_side", "style_tab_font_face"})
        CheckNoThemeField(tab, id);

    const UiDesignerControlSpec *group = catalog.Find("UiGroupPanel");
    Check(group != nullptr, "UiGroupPanel exists");
    CheckNoThemeField(group, "header_mode");

    const UiDesignerControlSpec *list = catalog.Find("UiList");
    Check(list != nullptr, "UiList exists");
    CheckNoThemeField(list, "show_row_separator");
    CheckNoThemeField(list, "right_text_as_badge");

    Cout() << Format("THEME_STRUCTURE_OWNERSHIP_SUMMARY checks=%d failed=%d\n",
                     checks, failed);
    SetExitCode(failed ? 1 : 0);
}
