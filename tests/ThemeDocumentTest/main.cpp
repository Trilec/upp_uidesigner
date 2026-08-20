#include <UiDesigner/ThemeCore/UiDesignerTheme.h>

using namespace Upp;

struct ThemeDocumentTest {
    int checks = 0;
    int failed = 0;

    void Check(bool condition, const String& message)
    {
        checks++;
        if(!condition) {
            failed++;
            Cout() << "FAIL: " << message << "\n";
        }
    }
};

CONSOLE_APP_MAIN
{
    ThemeDocumentTest t;
    String error;

    UiDesignerThemeSnapshot defaults;
    t.Check(defaults.light_palette.Get(4) == Color(58, 132, 255),
            "Light palette exposes the default accent swatch");
    t.Check(defaults.dark_palette.Get(4) == Color(96, 165, 250),
            "Dark palette has an independent accent swatch");
    t.Check(defaults.roles.control_accent == 4 &&
            defaults.roles.panel_surface == 1,
            "Control and panel roles have independent palette assignments");

    UiDesignerThemeDocument theme;
    t.Check(theme.Commit("palette.light.4", Color(12, 34, 56),
                         "Set Light accent swatch", error),
            "Light palette swatch commits: " + error);
    t.Check(theme.Get().light_palette.Get(4) == Color(12, 34, 56) &&
            theme.Get().accent == Color(12, 34, 56),
            "Active Light accent projection follows its assigned swatch");

    t.Check(theme.Commit("palette.dark.4", Color(90, 80, 70),
                         "Set Dark accent swatch", error),
            "Dark palette swatch commits independently: " + error);
    t.Check(theme.Get().accent == Color(12, 34, 56),
            "Editing inactive Dark palette does not change Light projection");
    t.Check(theme.Commit("mode", "Dark", "Use Dark palette", error),
            "Theme switches to Dark: " + error);
    t.Check(theme.Get().accent == Color(90, 80, 70),
            "Dark mode projects the Dark assigned accent");

    t.Check(theme.Commit("roles.control.accent", 3,
                         "Assign control Accent", error),
            "Control Accent role assignment commits: " + error);
    t.Check(theme.Get().roles.control_accent == 3 &&
            theme.Get().accent == theme.Get().dark_palette.Get(3),
            "Changing Accent role immediately changes active projection");
    t.Check(theme.Commit("roles.panel.strong", 5,
                         "Assign panel Strong", error),
            "Panel Strong role assignment commits separately: " + error);
    t.Check(theme.Get().roles.panel_strong == 5 &&
            theme.Get().roles.control_alert == 5,
            "Panel role assignment does not overwrite control role state");

    UiDesignerThemeDocument palette_history;
    const UiDesignerThemePalette original_light = palette_history.Get().light_palette;
    UiDesignerThemePalette edited_light = original_light;
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        edited_light.Set(i, Color(10 + i * 20, 20 + i * 15, 30 + i * 10));
    t.Check(palette_history.CommitPalette(false, edited_light,
                                          "Edit Light palette", error),
            "Six-colour Light palette commits atomically: " + error);
    t.Check(palette_history.Get().light_palette.ToValue() == edited_light.ToValue(),
            "Atomic palette commit stores all six colours");
    t.Check(palette_history.CanUndo() && palette_history.Undo() &&
            palette_history.Get().light_palette.ToValue() == original_light.ToValue(),
            "One undo restores the complete previous Light palette");
    t.Check(palette_history.CanRedo() && palette_history.Redo() &&
            palette_history.Get().light_palette.ToValue() == edited_light.ToValue(),
            "One redo restores the complete edited Light palette");

    const String serialized = theme.Serialize(true);
    Value serialized_value = ParseJSON(serialized);
    ValueMap serialized_root = serialized_value;
    t.Check((int)UiDesignerMapValue(serialized_root, "schema", 0) == 2,
            "Theme Builder serializes schema 2");

    UiDesignerThemeDocument roundtrip;
    t.Check(roundtrip.Deserialize(serialized, error),
            "Schema-2 theme round-trips: " + error);
    t.Check(roundtrip.Get().ToValue() == theme.Get().ToValue(),
            "Light/Dark palettes and role assignments survive round-trip");

    ValueMap legacy_theme;
    legacy_theme.Set("preset", "Pill");
    legacy_theme.Set("mode", "Dark");
    legacy_theme.Set("accent", "#123456");
    legacy_theme.Set("spacing", 11);
    ValueMap legacy_root;
    legacy_root.Set("format", "upp-ui-theme-designer");
    legacy_root.Set("schema", 1);
    legacy_root.Set("theme", legacy_theme);

    UiDesignerThemeDocument migrated;
    t.Check(migrated.Deserialize(AsJSON(legacy_root, true), error),
            "Schema-1 theme migrates: " + error);
    const int migrated_accent = migrated.Get().roles.control_accent;
    t.Check(migrated.Get().light_palette.Get(migrated_accent) == Color(18, 52, 86) &&
            migrated.Get().dark_palette.Get(migrated_accent) == Color(18, 52, 86),
            "Legacy single accent is preserved in both appearance palettes");
    t.Check(migrated.Get().accent == Color(18, 52, 86) &&
            migrated.Get().spacing == 11 && migrated.Get().preset == "Pill",
            "Legacy theme values survive migration");

    UiDesignerThemeDocument history;
    t.Check(history.Preview("palette.light.0", Color(1, 2, 3), error),
            "Palette preview is accepted: " + error);
    t.Check(history.GetEffective().light_palette.Get(0) == Color(1, 2, 3) &&
            history.Get().light_palette.Get(0) != Color(1, 2, 3),
            "Palette preview remains transient");
    history.CancelPreview();
    t.Check(history.GetEffective().light_palette.Get(0) ==
            history.Get().light_palette.Get(0),
            "Palette preview cancel restores committed state");
    t.Check(history.Commit("roles.panel.surface", 3,
                           "Set panel Surface", error),
            "Role assignment enters theme history: " + error);
    t.Check(history.CanUndo() && history.Undo() &&
            history.Get().roles.panel_surface == 1,
            "Theme role assignment undoes");
    t.Check(history.CanRedo() && history.Redo() &&
            history.Get().roles.panel_surface == 3,
            "Theme role assignment redoes");

    PropertyEditorModel model;
    history.BuildPropertyModel(model);
    t.Check(model.Find("palette.light.0") && model.Find("palette.dark.5"),
            "Theme Inspector exposes all Light/Dark palette domains");
    t.Check(model.Find("roles.control.standard") &&
            model.Find("roles.control.accent") &&
            model.Find("roles.panel.surface") &&
            model.Find("roles.panel.strong"),
            "Theme Inspector separates Control Roles from Panel Roles");

    Cout() << Format("THEME_DOCUMENT_SUMMARY checks=%d failed=%d\n",
                     t.checks, t.failed);
    SetExitCode(t.failed ? 1 : 0);
}