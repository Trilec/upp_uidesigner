#include "UiDesignerCatalog.h"

// Keep the existing catalog organization implementation intact, but compile
// its public entry point under a private name so this file can enforce the
// theme/configuration ownership boundary after the base normalization has
// run. The .inc file is the retained implementation checkpoint.
#define OrganizeUiDesignerControlSpec OrganizeUiDesignerControlSpecRaw
#include "UiDesignerInspectorCatalogImpl.inc"
#undef OrganizeUiDesignerControlSpec

namespace Upp {

static bool IsRetiredStructuralThemeOverride(const UiDesignerControlSpec& spec,
                                             const String& id)
{
    // TitleCard already exposes these as normal authored Appearance
    // properties. They must not also exist in Theme Studio, otherwise a
    // theme can create/remove/move divider/media structure.
    if(spec.theme_adapter_id == "title_card") {
        return id == "theme_media_auto_fit" ||
               id == "theme_title_line" ||
               id == "theme_title_line_thickness" ||
               id == "theme_title_line_style" ||
               id == "theme_card_line" ||
               id == "theme_card_line_side" ||
               id == "theme_card_line_thickness" ||
               id == "theme_card_line_gap";
    }

    // These are presentation modes of the authored list, not palette/theme
    // choices. Retaining them as theme overrides made a theme switch alter
    // the list's information structure.
    if(spec.theme_adapter_id == "list")
        return id == "show_row_separator" ||
               id == "right_text_as_badge";

    return false;
}

static void RemoveRetiredStructuralThemeOverrides(UiDesignerControlSpec& spec)
{
    for(int i = spec.theme_overrides.GetCount() - 1; i >= 0; --i)
        if(IsRetiredStructuralThemeOverride(spec, spec.theme_overrides[i].id))
            spec.theme_overrides.Remove(i);
}

void OrganizeUiDesignerControlSpec(
    UiDesignerControlSpec& spec,
    const Array<UiDesignerControlSpec>& registered)
{
    OrganizeUiDesignerControlSpecRaw(spec, registered);
    RemoveRetiredStructuralThemeOverrides(spec);
}

} // namespace Upp