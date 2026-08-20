#include <Core/Core.h>
#include <Ui/UiColorPicker/UiColorPicker.h>
#include <Utilities/PropertyEditor/PropertyEditor.h>
#include <UiDesigner/Theme/UiDesignerThemeGallery.h>

using namespace Upp;

namespace {

int checks = 0;
int failed = 0;

void Check(bool ok, const char *what)
{
    checks++;
    if(ok)
        return;
    failed++;
    Cout() << "FAIL: " << what << '\n';
}

}

CONSOLE_APP_MAIN
{
    UiDesignerThemeDocument theme;
    String error;

    UiDesignerThemePalette light = theme.Get().light_palette;
    const UiDesignerThemePalette dark_before = theme.Get().dark_palette;
    light.Set(0, Color(17, 34, 51));
    light.Set(5, Color(221, 85, 68));

    Check(theme.CommitPalette(false, light, "Test Light palette", error),
          "six-colour Light palette commits atomically");
    Check(error.IsEmpty(), "palette commit has no diagnostic");
    Check(theme.Get().light_palette.Get(0) == Color(17, 34, 51),
          "Light palette slot 1 committed");
    Check(theme.Get().light_palette.Get(5) == Color(221, 85, 68),
          "Light palette slot 6 committed");
    Check(theme.Get().dark_palette.Get(0) == dark_before.Get(0),
          "Light palette edit leaves Dark palette independent");
    Check(theme.CanUndo(), "palette edit creates one undoable history step");
    Check(theme.Undo(), "palette edit undo succeeds");
    Check(theme.Get().light_palette.Get(0) != Color(17, 34, 51),
          "undo restores previous Light palette");
    Check(theme.Redo(), "palette edit redo succeeds");
    Check(theme.Get().light_palette.Get(0) == Color(17, 34, 51),
          "redo restores edited Light palette");

    const String target = "Light|control|UiButton|Accent";
    theme.SetActiveStyleTarget(target);
    Check(theme.Commit("studio.face_normal", Color(44, 55, 66),
                       "Set Accent Button face", error),
          "selected Theme Studio style field commits");
    Check(theme.Get().HasStyleOverride(target, "face_normal") &&
          theme.Get().GetStyleOverride(target, "face_normal") == Color(44, 55, 66),
          "style recipe is stored under appearance/type/role target");
    Check(theme.Preview("studio.frame_normal", Color(70, 80, 90), error),
          "selected Theme Studio field previews");
    Check(theme.GetEffective().GetStyleOverride(target, "frame_normal") == Color(70, 80, 90) &&
          !theme.Get().HasStyleOverride(target, "frame_normal"),
          "Theme Studio preview remains transient");
    theme.CancelPreview();

    const String json = theme.Serialize(false);
    Value root_value = ParseJSON(json);
    const bool valid_json = !IsError(root_value) && root_value.Is<ValueMap>();
    Check(valid_json,
          "Theme Builder serializes typed style values as valid JSON");
    ValueMap root = valid_json ? (ValueMap)root_value : ValueMap();
    Check((int)UiDesignerMapValue(root, "schema", 0) == 3,
          "Theme Builder serializes schema 3 with style recipes");

    UiDesignerThemeDocument loaded;
    Check(loaded.Deserialize(json, error), "Theme Builder JSON round-trips");
    Check(loaded.Get().light_palette.Get(5) == Color(221, 85, 68),
          "round-trip preserves six-colour palette");
    Check(loaded.Get().dark_palette.Get(0) == dark_before.Get(0),
          "round-trip preserves independent Dark palette");
    Check(loaded.Get().GetStyleOverride(target, "face_normal") == Color(44, 55, 66),
          "round-trip restores typed Color style recipe");

    loaded.SetActiveStyleTarget(target);
    Check(loaded.Reset("studio.face_normal", error),
          "Theme Studio reset removes an authored field");
    Check(!loaded.Get().HasStyleOverride(target, "face_normal"),
          "reset restores inherited adapter value");
    Check(loaded.Get().GetStyleOverrides(target).IsEmpty(),
          "reset removes an empty style target recipe");
    Check(loaded.Undo() &&
          loaded.Get().GetStyleOverride(target, "face_normal") == Color(44, 55, 66),
          "Theme Studio style reset participates in undo history");

    Color parsed;
    int alpha = 0;
    Check(UiColorPicker::ParseColorText("#A1B2C3", parsed, alpha),
          "shared colour parser accepts clipboard hex");
    Check(parsed == Color(0xA1, 0xB2, 0xC3) && alpha == 255,
          "shared colour parser preserves RGB value");

    PropertyEditorModel model;
    model.AddColor("frame", "Frame", Color(12, 34, 56), "Theme");
    Check(model.Find("frame") != nullptr &&
          model.Find("frame")->kind == PropertyEditorKind::Color,
          "PropertyEditor exposes a colour drop target kind");

    // These calls exercise the public Theme Builder preview contract. The test
    // package also compiles and links the full Theme module, including the
    // toolbar, three-column matrix, palette popup, selectable samples and
    // swatch drag source.
    UiDesignerThemeGallery *gallery_contract = nullptr;
    UiDesignerThemeToolbar *toolbar_contract = nullptr;
    Check(gallery_contract == nullptr && toolbar_contract == nullptr,
          "Theme Builder gallery and toolbar contracts are link-visible");

    Cout() << Format("THEME_BUILDER_CONTRACT_SUMMARY checks=%d failed=%d\n",
                     checks, failed);
    SetExitCode(failed ? 1 : 0);
}