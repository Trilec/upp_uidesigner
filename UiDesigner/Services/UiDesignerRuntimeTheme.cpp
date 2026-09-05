#include "UiDesignerRuntimeTheme.h"

namespace Upp {

static String RuntimeThemeAppearance(const UiDesignerThemeSnapshot& theme)
{
    // Theme Studio has authored Light/Dark recipe buckets. System currently
    // resolves through the Light bucket, matching the existing Studio target
    // selection and UiTheme System behavior.
    return theme.mode == "Dark" ? "Dark" : "Light";
}

bool UiDesignerUsesPanelThemeDomain(const UiDesignerControlSpec& spec)
{
    return spec.runtime_kind == UiDesignerRuntimeKind::UiPanel ||
           spec.runtime_kind == UiDesignerRuntimeKind::UiGroupPanel ||
           spec.runtime_kind == UiDesignerRuntimeKind::UiScrollPanel;
}

String UiDesignerRuntimeThemeTarget(const UiDesignerThemeSnapshot& theme,
                                    const UiDesignerControlSpec& spec,
                                    const UiDesignerNode& node)
{
    const String domain = UiDesignerUsesPanelThemeDomain(spec)
        ? "panel" : "control";
    const String role = AsString(node.GetProperty("role", "Standard"));
    return RuntimeThemeAppearance(theme) + "|" + domain + "|" +
           spec.type_id + "|" + role;
}

ValueMap UiDesignerRuntimeThemeRecipe(const UiDesignerThemeSnapshot& theme,
                                      const UiDesignerControlSpec& spec,
                                      const UiDesignerNode& node)
{
    ValueMap recipe = theme.GetStyleOverrides(
        UiDesignerRuntimeThemeTarget(theme, spec, node));
    if(!recipe.IsEmpty())
        return recipe;

    // Historical Theme Studio code had both control/panel target helpers.
    // Prefer the canonical domain above, but accept an authored recipe in the
    // alternate bucket when no canonical recipe exists so old projects do not
    // silently lose appearance.
    const String role = AsString(node.GetProperty("role", "Standard"));
    const String alternate = RuntimeThemeAppearance(theme) + "|" +
        (UiDesignerUsesPanelThemeDomain(spec) ? "control" : "panel") + "|" +
        spec.type_id + "|" + role;
    return theme.GetStyleOverrides(alternate);
}

UiDesignerNode UiDesignerResolveRuntimeThemedNode(
    const UiDesignerNode& node,
    const UiDesignerThemeSnapshot& theme,
    const UiDesignerControlSpec& spec)
{
    UiDesignerNode effective(node);
    const ValueMap recipe = UiDesignerRuntimeThemeRecipe(theme, spec, node);
    if(recipe.IsEmpty())
        return effective;

    // ThemeDocument recipes are inherited defaults. Only fields in the real
    // adapter schema can cross into runtime. An active instance override wins;
    // a disabled/saved instance override is deliberately ignored here.
    for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
        const int q = recipe.Find(property.id);
        if(q >= 0 && effective.theme_overrides.Find(property.id) < 0)
            effective.theme_overrides.Set(property.id, recipe.GetValue(q));
    }
    return effective;
}

void UiDesignerApplyRuntimeThemeRecipes(
    UiDesignerDocument& document,
    const UiDesignerThemeSnapshot& theme,
    const UiDesignerCatalog& catalog)
{
    Vector<UiDesignerNodeId> ids;
    for(const UiDesignerNode& node : document.GetNodes())
        if(node.id != document.GetRootId())
            ids.Add(node.id);

    for(UiDesignerNodeId id : ids) {
        UiDesignerNode* node = document.Find(id);
        const UiDesignerControlSpec* spec =
            node ? catalog.Find(node->type) : nullptr;
        if(!node || !spec || spec->theme_overrides.IsEmpty())
            continue;
        UiDesignerNode effective =
            UiDesignerResolveRuntimeThemedNode(*node, theme, *spec);
        node->theme_overrides = effective.theme_overrides;
        // Keep disabled/saved local values intact in the source document clone.
        // They remain inactive and are not emitted by CodeGen.
    }
}

}
