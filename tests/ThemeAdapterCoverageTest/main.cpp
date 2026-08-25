#include <Core/Core.h>
#include <Utilities/PropertyEditor/PropertyEditor.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>

using namespace Upp;

namespace {

int checks = 0;
int failed = 0;

void Check(bool ok, const String& what)
{
    checks++;
    if(ok)
        return;
    failed++;
    Cout() << "FAIL: " << what << '\n';
}

const UiDesignerThemeOverrideSpec* RequireOverride(
    const UiDesignerCatalog& catalog, const char *type, const char *id)
{
    const UiDesignerControlSpec *spec = catalog.Find(type);
    Check(spec != nullptr, String(type) + " exists in catalog");
    if(!spec)
        return nullptr;
    const UiDesignerThemeOverrideSpec *property = spec->FindThemeOverride(id);
    Check(property != nullptr, String(type) + " exposes theme field " + id);
    return property;
}

void CheckProjection(const UiDesignerThemeOverrideSpec *property,
                     PropertyEditorKind kind, bool slider,
                     const String& what)
{
    if(!property)
        return;
    PropertyEditorModel model;
    property->AddTo(model, property->default_value, false);
    const PropertyEditorItem *item = model.Find(property->id);
    Check(item != nullptr, what + " projects into PropertyEditor");
    if(!item)
        return;
    Check(item->kind == kind, what + " keeps the expected editor kind");
    if(slider)
        Check(item->show_slider_toggle,
              what + " keeps the shared numeric slider-toggle affordance");
}

}

CONSOLE_APP_MAIN
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    String catalog_error;
    Check(catalog.Validate(catalog_error), "catalog validates: " + catalog_error);

    static const char *selectable_types[] = {
        "UiPanel", "UiGroupPanel", "UiScrollPanel", "UiTab",
        "UiLabel", "UiButton", "UiToolButton", "UiSplitButton",
        "UiCheckBox", "UiRadioButton", "UiToggle", "UiDropdown",
        "UiIntEdit", "UiFloatEdit", "UiLineEdit", "UiMultiEdit",
        "UiSlider", "UiProgressBar", "UiScrollBar",
        "UiList", "UiTree", "UiAccordion"
    };

    for(const char *type : selectable_types) {
        const UiDesignerControlSpec *spec = catalog.Find(type);
        Check(spec != nullptr, String(type) + " is registered");
        if(!spec)
            continue;
        Check(spec->theme, String(type) + " declares Theme support");
        const UiDesignerThemeAdapter *adapter = UiDesignerGetThemeAdapter(*spec);
        Check(adapter != nullptr, String(type) + " resolves a Theme adapter");
        if(!adapter)
            continue;
        Check(adapter->Supports(spec->runtime_kind),
              String(type) + " adapter supports its runtime kind");
        Check(!spec->theme_overrides.IsEmpty(),
              String(type) + " exposes an editable Theme surface");
        for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides)
            Check(adapter->HasField(property.adapter_field_id),
                  String(type) + " adapter owns field " + property.adapter_field_id);
    }

    // Fill surfaces must remain full UiFill recipes rather than being reduced
    // to a single colour field. This is the shared Solid / None / Gradient path.
    CheckProjection(RequireOverride(catalog, "UiLabel", "face.normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Label normal face");
    CheckProjection(RequireOverride(catalog, "UiButton", "face_normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Button normal face");
    CheckProjection(RequireOverride(catalog, "UiProgressBar", "face_normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Progress track normal face");
    CheckProjection(RequireOverride(catalog, "UiProgressBar", "fill_face_normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Progress fill normal face");

    // Previously-missing StyledMetrics radius coverage.
    CheckProjection(RequireOverride(catalog, "UiProgressBar", "radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Progress track radius");
    CheckProjection(RequireOverride(catalog, "UiProgressBar", "fill_radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Progress fill radius");
    CheckProjection(RequireOverride(catalog, "UiSlider", "radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Slider track radius");
    CheckProjection(RequireOverride(catalog, "UiSlider", "thumb_radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Slider thumb radius");
    CheckProjection(RequireOverride(catalog, "UiScrollBar", "radius"),
                    PropertyEditorKind::NumericInt, true,
                    "ScrollBar track radius");
    CheckProjection(RequireOverride(catalog, "UiScrollBar", "thumb_radius"),
                    PropertyEditorKind::NumericInt, true,
                    "ScrollBar thumb radius");
    CheckProjection(RequireOverride(catalog, "UiScrollBar", "arrow_radius"),
                    PropertyEditorKind::NumericInt, true,
                    "ScrollBar arrow radius");

    // Container coverage is now Style-shaped rather than a generic surface.
    CheckProjection(RequireOverride(catalog, "UiPanel", "radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Panel radius");
    CheckProjection(RequireOverride(catalog, "UiPanel", "face.normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Panel face");
    CheckProjection(RequireOverride(catalog, "UiGroupPanel", "title_font_height"),
                    PropertyEditorKind::NumericInt, true,
                    "GroupPanel title font height");
    RequireOverride(catalog, "UiGroupPanel", "header_mode");
    RequireOverride(catalog, "UiGroupPanel", "inset_left");
    CheckProjection(RequireOverride(catalog, "UiScrollPanel", "radius"),
                    PropertyEditorKind::NumericInt, true,
                    "ScrollPanel radius");

    // Tab is intentionally deep: body, tab surface, typography, layout and
    // indicator state must all remain first-class Theme Studio fields.
    CheckProjection(RequireOverride(catalog, "UiTab", "radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Tab body radius");
    CheckProjection(RequireOverride(catalog, "UiTab", "tab_radius"),
                    PropertyEditorKind::NumericInt, true,
                    "Tab item radius");
    CheckProjection(RequireOverride(catalog, "UiTab", "indicator_thickness"),
                    PropertyEditorKind::NumericInt, true,
                    "Tab indicator thickness");
    RequireOverride(catalog, "UiTab", "style_tab_font_face");
    const UiDesignerThemeOverrideSpec *tab_side =
        RequireOverride(catalog, "UiTab", "icon_side");
    if(tab_side) {
        PropertyEditorModel model;
        tab_side->AddTo(model, tab_side->default_value, false);
        const PropertyEditorItem *item = model.Find("icon_side");
        Check(item && item->kind == PropertyEditorKind::Custom &&
                    item->custom_editor == PropertyEditorMatrixId() &&
                    item->editor_variant == "Cardinal4",
              "Tab icon side keeps the shared Cardinal4 selector");
    }

    const UiDesignerControlSpec *tab = catalog.Find("UiTab");
    const UiDesignerThemeAdapter *tab_adapter =
        tab ? UiDesignerGetThemeAdapter(*tab) : nullptr;
    const UiDesignerThemeOverrideSpec *visual =
        tab ? tab->FindThemeOverride("style_visual") : nullptr;
    Check(tab_adapter != nullptr && visual != nullptr,
          "Tab visual has a runtime-aware Theme adapter");
    if(tab_adapter && visual) {
        UiDesignerNode node;
        node.type = "UiTab";
        node.SetProperty("role", "Accent");
        node.SetThemeOverride(visual->id, "Underline");
        String code;
        tab_adapter->EmitSetup(code, "tabs", node, *tab);
        Check(code.Find("tabs.SetVisual(UITAB_UNDERLINE)") >= 0,
              "Tab generated code applies the visual to runtime instance state");
        Check(code.Find("UITAB_UNDERLINE") >= 0,
              "Tab generated style resolves from the authored visual family");
    }

    // Resource-backed skin images are intentionally not faked as filesystem
    // paths. Policy flags such as popup_use_main_skin are ordinary API fields.
    // When the document resource resolver lands, the eventual public label is
    // required to be Skin (Nine Slice).
    for(const char *type : selectable_types) {
        const UiDesignerControlSpec *spec = catalog.Find(type);
        if(!spec)
            continue;
        for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides)
            Check(property.id.Find("skin_image") < 0,
                  String(type) + " does not expose an unresolved fake skin field");
    }

    Cout() << Format("THEME_ADAPTER_COVERAGE_SUMMARY checks=%d failed=%d\n",
                     checks, failed);
    SetExitCode(failed ? 1 : 0);
}
