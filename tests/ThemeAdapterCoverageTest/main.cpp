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

void CheckNoThemeOverride(const UiDesignerCatalog& catalog,
                          const char *type, const char *id)
{
    const UiDesignerControlSpec *spec = catalog.Find(type);
    Check(spec != nullptr, String(type) + " exists in catalog");
    if(spec)
        Check(spec->FindThemeOverride(id) == nullptr,
              String(type) + " does not expose structural Theme field " + id);
}

PropertyEditorKind ProjectedKind(const UiDesignerThemeOverrideSpec& property)
{
    const bool bounded = !IsNull(property.minimum) && !IsNull(property.maximum);
    if(bounded && property.kind == PropertyEditorKind::Integer)
        return PropertyEditorKind::NumericInt;
    if(bounded && property.kind == PropertyEditorKind::Double)
        return PropertyEditorKind::NumericDouble;
    return property.kind;
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

void CheckProjectedThemeField(const UiDesignerThemeOverrideSpec& property,
                              const String& type)
{
    PropertyEditorModel model;
    property.AddTo(model, property.default_value, false);
    const PropertyEditorItem *item = model.Find(property.id);
    const String prefix = type + "." + property.id + " ";
    Check(item != nullptr, prefix + "projects into PropertyEditor");
    if(!item)
        return;
    Check(item->kind == ProjectedKind(property),
          prefix + "preserves projected editor kind");
    Check(item->domain == PropertyEditorDomain::Theme,
          prefix + "remains Theme-domain data");
}

void CheckThemeOverrideCount(const UiDesignerCatalog& catalog, const char *type,
                             const char *id, int expected)
{
    const UiDesignerControlSpec *spec = catalog.Find(type);
    int count = 0;
    if(spec)
        for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides)
            if(property.id == id)
                count++;
    Check(spec && count == expected,
          Format("%s.%s theme override count is %d", type, id, expected));
}

void CheckThemeOverrideIdsUnique(const UiDesignerControlSpec& spec)
{
    String duplicates;
    for(int i = 0; i < spec.theme_overrides.GetCount(); ++i) {
        const String& id = spec.theme_overrides[i].id;
        bool repeated = false;
        for(int j = 0; j < i; ++j)
            if(spec.theme_overrides[j].id == id) {
                repeated = true;
                break;
            }
        if(repeated) {
            if(!duplicates.IsEmpty())
                duplicates << ", ";
            duplicates << id;
        }
    }
    Check(duplicates.IsEmpty(),
          spec.type_id + " has unique Theme override ids" +
          (duplicates.IsEmpty() ? String() : ": " + duplicates));
}

}

CONSOLE_APP_MAIN
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    String catalog_error;
    Check(catalog.Validate(catalog_error), "catalog validates: " + catalog_error);

    static const char *selectable_types[] = {
        "UiPanel", "UiGroupPanel", "UiScrollPanel", "UiTab", "UiTitleCard",
        "UiLabel", "UiButton", "UiToolButton", "UiSplitButton",
        "UiCheckBox", "UiRadioButton", "UiToggle", "UiDropdown",
        "UiIntEdit", "UiFloatEdit", "UiLineEdit", "UiMultiEdit",
        "UiSlider", "UiProgressBar", "UiScrollBar",
        "UiList", "UiTree", "UiMenu", "UiAccordion"
    };

    // This is deliberately live-schema coverage: every field currently
    // advertised by a Theme-capable control must be owned by its adapter and
    // must project through the shared PropertyEditor contract. It prevents a
    // stale hand-maintained inventory from becoming the test oracle again.
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
        CheckThemeOverrideIdsUnique(*spec);
        for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides) {
            Check(adapter->HasField(property.adapter_field_id),
                  String(type) + " adapter owns field " + property.adapter_field_id);
            CheckProjectedThemeField(property, type);
        }
    }

    // Full fill recipes remain first-class where the current adapter owns a
    // recipe surface rather than a simple state colour.
    CheckProjection(RequireOverride(catalog, "UiLabel", "face.normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Label normal face");
    CheckProjection(RequireOverride(catalog, "UiPanel", "face.normal"),
                    PropertyEditorKind::FillRecipe, false,
                    "Panel normal face");

    // The compact/basic control adapters still have exactly one canonical
    // normal face/frame/ink field. Do not let alias duplication return.
    for(const char *type : {"UiCheckBox", "UiRadioButton", "UiToggle",
                            "UiDropdown", "UiProgressBar", "UiSlider",
                            "UiScrollBar"}) {
        CheckThemeOverrideCount(catalog, type, "face_normal", 1);
        CheckThemeOverrideCount(catalog, type, "frame_normal", 1);
        CheckThemeOverrideCount(catalog, type, "ink_normal", 1);
    }

    // Structure/configuration is authored on the control/document, never as
    // a parallel Theme Studio value. These are the historical aliases that
    // previously allowed a preset/Theme refresh to change authored structure.
    for(const char *id : {"style_align_h", "style_align_v",
                           "style_icon_side", "style_content_gap",
                           "style_icon_render_mode"})
        CheckNoThemeOverride(catalog, "UiButton", id);

    CheckNoThemeOverride(catalog, "UiGroupPanel", "header_mode");

    for(const char *id : {"style_visual", "icon_side", "style_tab_font_face"})
        CheckNoThemeOverride(catalog, "UiTab", id);

    for(const char *id : {"theme_media_auto_fit", "theme_title_line",
                           "theme_title_line_thickness", "theme_title_line_style",
                           "theme_card_line", "theme_card_line_side",
                           "theme_card_line_thickness", "theme_card_line_gap"})
        CheckNoThemeOverride(catalog, "UiTitleCard", id);

    CheckNoThemeOverride(catalog, "UiList", "show_row_separator");
    CheckNoThemeOverride(catalog, "UiList", "right_text_as_badge");

    // Keep representative current Tab styling fields covered while its visual
    // family/placement/icon-side remain normal authored configuration.
    RequireOverride(catalog, "UiTab", "radius");
    RequireOverride(catalog, "UiTab", "tab_face_normal");
    RequireOverride(catalog, "UiTab", "indicator_thickness");
    RequireOverride(catalog, "UiTab", "style_tab_extent");

    // Resource-backed skin images are intentionally not faked as filesystem
    // paths. The eventual resource-backed skin editor remains a separate seam.
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
