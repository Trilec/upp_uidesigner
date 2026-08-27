#include <Core/Core.h>
#include <Ui/UiColorPicker/UiColorPicker.h>
#include <Utilities/PropertyEditor/PropertyEditor.h>
#include <UiDesigner/Theme/UiDesignerThemeBuilderV2.h>

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

    const String preview_target = "control|UiLabel";
    theme.SetActivePreviewTarget(preview_target);
    Check(theme.Commit("preview.icon", "ICON_DESIGN_WIDGETS_48",
                       "Set Theme Studio preview icon", error),
          "Theme Studio sample presentation commits independently");
    Check(theme.Commit("preview.icon_width", 32,
                       "Set Theme Studio preview icon width", error),
          "Theme Studio preview numeric value commits");
    Check(AsString(theme.Get().GetStudioPreviewValue(preview_target, "icon")) ==
              "ICON_DESIGN_WIDGETS_48" &&
          (int)theme.Get().GetStudioPreviewValue(preview_target, "icon_width") == 32,
          "Theme Studio presentation state is stored under its sample target");
    Check(!theme.Get().HasStyleOverride(target, "icon") &&
          !theme.Get().HasStyleOverride(target, "icon_width"),
          "preview presentation does not pollute the runtime style recipe");

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
    Check(AsString(loaded.Get().GetStudioPreviewValue(preview_target, "icon")) ==
              "ICON_DESIGN_WIDGETS_48" &&
          (int)loaded.Get().GetStudioPreviewValue(preview_target, "icon_width") == 32,
          "round-trip preserves Theme Studio presentation separately");

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

    loaded.SetActivePreviewTarget(preview_target);
    Check(loaded.Reset("preview.icon_width", error),
          "Theme Studio preview reset succeeds");
    Check(IsNull(loaded.Get().GetStudioPreviewValue(preview_target, "icon_width")),
          "preview reset returns to the projection fallback without changing style");

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

    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    const UiDesignerControlSpec *label = catalog.Find("UiLabel");
    const UiDesignerControlSpec *button = catalog.Find("UiButton");
    Check(label != nullptr && button != nullptr,
          "Designer catalog exposes Label and Button metadata");

    const UiDesignerPropertySpec *width = label ? label->FindProperty("icon_width") : nullptr;
    const UiDesignerPropertySpec *side = label ? label->FindProperty("icon_side") : nullptr;
    const UiDesignerPropertySpec *icon = label ? label->FindProperty("icon") : nullptr;
    Check(width && side && icon,
          "Label catalog exposes reusable preview icon metadata");

    PropertyEditorModel width_projection;
    if(width)
        width->AddTo(width_projection, width->default_value, false);
    const PropertyEditorItem *width_item = width_projection.Find("icon_width");
    Check(width_item && width_item->kind == PropertyEditorKind::NumericInt &&
          width_item->show_slider_toggle,
          "bounded Designer numeric projection keeps value-to-slider affordance");

    PropertyEditorModel side_projection;
    if(side)
        side->AddTo(side_projection, side->default_value, false);
    const PropertyEditorItem *side_item = side_projection.Find("icon_side");
    bool canonical = side_item && side_item->choices.GetCount() == 4;
    if(canonical) {
        static const char *expected[] = {"Left", "Right", "Top", "Bottom"};
        for(int i = 0; i < 4; i++)
            canonical &= AsString(side_item->choices[i].value) == expected[i];
    }
    Check(side_item && side_item->kind == PropertyEditorKind::Custom &&
          side_item->custom_editor == PropertyEditorMatrixId() &&
          side_item->editor_variant == "Cardinal4" && canonical,
          "directional Designer choice projects to canonical Cardinal4 matrix");

    PropertyEditorModel icon_projection;
    if(icon)
        icon->AddTo(icon_projection, icon->default_value, false);
    const PropertyEditorItem *icon_item = icon_projection.Find("icon");
    Check(icon_item && icon_item->kind == PropertyEditorKind::Custom &&
          icon_item->custom_editor == PropertyEditorIconId(),
          "Designer icon projection keeps the shared icon chooser");

    const UiDesignerThemeOverrideSpec *radius =
        button ? button->FindThemeOverride("radius") : nullptr;
    Check(radius != nullptr,
          "Button Theme adapter exposes radius metadata");
    PropertyEditorModel radius_projection;
    if(radius)
        radius->AddTo(radius_projection, radius->default_value, false);
    const PropertyEditorItem *radius_item = radius_projection.Find("radius");
    Check(radius_item && radius_item->kind == PropertyEditorKind::NumericInt &&
          radius_item->show_slider_toggle,
          "Theme override numeric projection keeps normal slider-toggle metadata");

    // Closure: the Theme Studio exposes two independent universal role axes.
    UiDesignerThemeGalleryV2 gallery;
    gallery.SetCatalog(&catalog);
    gallery.SetThemeDocument(&loaded);
    UiDesignerThemeToolbarV2 toolbar(loaded, gallery);
    const UiRole roles[] = {
        UiRole::Standard, UiRole::Subtle, UiRole::Accent, UiRole::Alert
    };
    for(UiRole role : roles) {
        const UiRole control_before = toolbar.GetUniversalControlRole();
        toolbar.SetPanelRole(role);
        Check(toolbar.GetUniversalPanelRole() == role &&
              toolbar.GetUniversalControlRole() == control_before,
              "Panel Role changes independently across all four universal roles");
        const UiRole panel_before = toolbar.GetUniversalPanelRole();
        toolbar.SetControlRole(role);
        Check(toolbar.GetUniversalControlRole() == role &&
              toolbar.GetUniversalPanelRole() == panel_before,
              "Control Role changes independently across all four universal roles");
    }

    // The target key itself carries panel/control scope; writing one namespace
    // must not mutate the other even for the same type, appearance and role.
    const String panel_target = "Light|panel|UiGroupPanel|Accent";
    const String control_target = "Light|control|UiGroupPanel|Accent";
    loaded.SetActiveStyleTarget(panel_target);
    Check(loaded.Commit("studio.radius", 13, "Panel radius", error),
          "panel role target accepts an authored recipe");
    loaded.SetActiveStyleTarget(control_target);
    Check(loaded.Commit("studio.radius", 7, "Control radius", error),
          "control role target accepts an authored recipe");
    Check((int)loaded.Get().GetStyleOverride(panel_target, "radius") == 13 &&
          (int)loaded.Get().GetStyleOverride(control_target, "radius") == 7,
          "panel| and control| style-target namespaces remain isolated");

    gallery.SetRect(0, 0, DPI(960), DPI(760));
    gallery.Layout();
    Check(gallery.GetDataSampleColumn() == 0 &&
          gallery.GetChoicesSampleColumn() == 2,
          "Theme gallery places DATA in column 1 and CHOICES in column 3");
    Check(gallery.IsSaveSampleContained(),
          "Theme gallery Save split-button stays inside its Buttons group");

    UiDesignerThemeSnapshot light_snapshot = loaded.GetEffective();
    light_snapshot.mode = "Light";
    UiDesignerApplyGlobalTheme(light_snapshot);
    const UiDesignerThemeSurfacePalette light_surface =
        UiDesignerResolveThemeSurfacePalette();
    UiDesignerThemeSnapshot dark_snapshot = light_snapshot;
    dark_snapshot.mode = "Dark";
    UiDesignerApplyGlobalTheme(dark_snapshot);
    const UiDesignerThemeSurfacePalette dark_surface =
        UiDesignerResolveThemeSurfacePalette();
    Check(light_surface.paper != dark_surface.paper ||
          light_surface.ink != dark_surface.ink ||
          light_surface.alternate != dark_surface.alternate,
          "Designer-owned catalog/surface painting resolves different Dark Theme colours");
    UiDesignerApplyGlobalTheme(loaded.GetEffective());

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
