#include <Core/Core.h>
#include <Ui/UiTheme.h>
#include <Utilities/PropertyEditor/PropertyEditor.h>
#include <UiDesigner/Theme/UiDesignerThemeGallery.h>

using namespace Upp;

static String ColorText(Color c)
{
    return Format("(%d,%d,%d)", c.GetR(), c.GetG(), c.GetB());
}

static bool IsDarkFace(Color c)
{
    return c.GetR() < 100 && c.GetG() < 100 && c.GetB() < 100;
}

static bool IsLightInk(Color c)
{
    return c.GetR() > 150 && c.GetG() > 150 && c.GetB() > 150;
}

static bool IsLightFace(Color c)
{
    return c.GetR() > 180 && c.GetG() > 180 && c.GetB() > 180;
}

static bool IsDarkInk(Color c)
{
    return c.GetR() < 120 && c.GetG() < 120 && c.GetB() < 120;
}

static bool IsDarkFill(const UiFill& fill)
{
    return fill.IsSolid() && IsDarkFace(fill.color);
}

static bool IsLightFill(const UiFill& fill)
{
    return fill.IsSolid() && IsLightFace(fill.color);
}

static int checks;
static int failed;

static void Check(bool ok, const char *what)
{
    checks++;
    if(!ok) {
        failed++;
        Cout() << "FAIL: " << what << '\n';
    }
}

CONSOLE_APP_MAIN
{
    UiDesignerThemeSnapshot snapshot;
    snapshot.preset = "Minimal";

    snapshot.mode = "Light";
    UiDesignerApplyGlobalTheme(snapshot);
    UiPanel::Style light_surface = UiTheme::ResolvePanel(UiPanelRole::Surface);
    UiLabel::Style light_label = UiTheme::ResolveLabel(UiRole::Standard);
    UiDropdown::Style light_dropdown = UiTheme::ResolveDropdown(UiRole::Accent);
    UiSlider::Style light_slider = UiTheme::ResolveSlider();
    UiTable::Style light_table = UiTheme::ResolveTable();
    PropertyEditorStyle light_style = PropertyEditorStyle::System();
    Check(UiTheme::GetMode() == UiThemeMode::Light,
          "Designer Light applies Light UiTheme mode");
    Check(light_surface.palette.face[ST_NORMAL].IsSolid() &&
          IsLightFace(light_surface.palette.face[ST_NORMAL].color),
          "Light UiTheme surface is light");
    Check(IsLightFace(light_style.background),
          "Light PropertyEditor background is light");
    Check(IsDarkInk(light_style.label_ink),
          "Light PropertyEditor label ink is dark");
    Check(IsLightFill(light_dropdown.palette.face[ST_NORMAL]) &&
          IsLightFace(light_dropdown.popup_background_color),
          "Light dropdown presentation is light");
    Check(IsLightFill(light_slider.track_palette.face[ST_NORMAL]),
          "Light slider track is light");
    Check(IsLightFace(light_table.table_bg),
          "Light table background is light");

    snapshot.mode = "Dark";
    UiDesignerApplyGlobalTheme(snapshot);
    UiPanel::Style dark_surface = UiTheme::ResolvePanel(UiPanelRole::Surface);
    UiLabel::Style dark_label = UiTheme::ResolveLabel(UiRole::Standard);
    UiDropdown::Style dark_dropdown = UiTheme::ResolveDropdown(UiRole::Accent);
    UiSlider::Style dark_slider = UiTheme::ResolveSlider();
    UiTable::Style dark_table = UiTheme::ResolveTable();
    PropertyEditorStyle dark_style = PropertyEditorStyle::System();
    Check(UiTheme::GetMode() == UiThemeMode::Dark,
          "Designer Dark applies Dark UiTheme mode");
    Check(dark_surface.palette.face[ST_NORMAL].IsSolid() &&
          IsDarkFace(dark_surface.palette.face[ST_NORMAL].color),
          "Dark UiTheme surface is materially dark");
    Check(IsDarkFace(dark_style.background),
          "Dark PropertyEditor background is dark");
    Check(IsDarkFace(dark_style.row_odd) && IsDarkFace(dark_style.row_even) &&
          IsDarkFace(dark_style.group_background),
          "Dark PropertyEditor rows and groups are dark");
    Check(IsLightInk(dark_style.label_ink) && IsLightInk(dark_style.value_ink),
          "Dark PropertyEditor labels and values are readable");
    Check(IsDarkFill(dark_dropdown.palette.face[ST_NORMAL]) &&
          IsDarkFace(dark_dropdown.popup_background_color),
          "Dark dropdown presentation is dark");
    Check(IsDarkFill(dark_slider.track_palette.face[ST_NORMAL]),
          "Dark slider track is dark");
    Check(IsDarkFace(dark_table.table_bg),
          "Dark table background is dark");
    Check(dark_style.background != light_style.background &&
          dark_style.row_odd != light_style.row_odd &&
          dark_style.group_background != light_style.group_background,
          "Dark PropertyEditor style differs from Light");

    snapshot.mode = "Light";
    UiDesignerApplyGlobalTheme(snapshot);
    PropertyEditorStyle light_again = PropertyEditorStyle::System();
    Check(IsLightFace(light_again.background) && IsDarkInk(light_again.label_ink),
          "Dark to Light restores the Light PropertyEditor style");

    snapshot.mode = "Dark";
    UiDesignerApplyGlobalTheme(snapshot);
    PropertyEditorStyle dark_again = PropertyEditorStyle::System();
    Check(IsDarkFace(dark_again.background) && IsLightInk(dark_again.label_ink),
          "repeated mode switch has no stale Light PropertyEditor palette");

    Cout() << "LIGHT surface="
           << ColorText(light_surface.palette.face[ST_NORMAL].color)
           << " label=" << ColorText(light_label.palette.ink[ST_NORMAL])
           << " pe=" << ColorText(light_style.background)
           << " / " << ColorText(light_style.label_ink) << "\n";
    Cout() << "DARK surface="
           << ColorText(dark_surface.palette.face[ST_NORMAL].color)
           << " label=" << ColorText(dark_label.palette.ink[ST_NORMAL])
           << " pe=" << ColorText(dark_style.background)
           << " / " << ColorText(dark_style.label_ink)
           << " table=" << ColorText(dark_table.table_bg) << "\n";
    Cout() << "THEME_DARK_INTEGRATION_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
}
