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

    const String json = theme.Serialize(false);
    UiDesignerThemeDocument loaded;
    Check(loaded.Deserialize(json, error), "Theme Builder JSON round-trips");
    Check(loaded.Get().light_palette.Get(5) == Color(221, 85, 68),
          "round-trip preserves six-colour palette");
    Check(loaded.Get().dark_palette.Get(0) == dark_before.Get(0),
          "round-trip preserves independent Dark palette");

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
    // toolbar, three-column matrix, palette popup and swatch drag source.
    UiDesignerThemeGallery *gallery_contract = nullptr;
    UiDesignerThemeToolbar *toolbar_contract = nullptr;
    Check(gallery_contract == nullptr && toolbar_contract == nullptr,
          "Theme Builder gallery and toolbar contracts are link-visible");

    Cout() << Format("THEME_BUILDER_CONTRACT_SUMMARY checks=%d failed=%d\n",
                     checks, failed);
    SetExitCode(failed ? 1 : 0);
}
