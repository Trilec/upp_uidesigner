#ifndef _UiDesigner_Theme_UiDesignerNormalizedThemeCommon_h_
#define _UiDesigner_Theme_UiDesignerNormalizedThemeCommon_h_

#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Core/UiDesignerOverlay.h>
#include <Ui/UiDraw.h>
#include <Ui/UiTheme.h>

namespace Upp {
namespace UiDesignerNormalizedTheme {

inline UiRole Role(const Value& value)
{
    const String role = AsString(value);
    if(role == "Subtle") return UiRole::Subtle;
    if(role == "Accent") return UiRole::Accent;
    if(role == "Alert") return UiRole::Alert;
    return UiRole::Standard;
}

inline String RoleExpr(const Value& value)
{
    const String role = AsString(value);
    if(role == "Subtle") return "UiRole::Subtle";
    if(role == "Accent") return "UiRole::Accent";
    if(role == "Alert") return "UiRole::Alert";
    return "UiRole::Standard";
}

inline bool HasValue(const UiDesignerNode& node,
                     const UiDesignerTransientOverlay* overlay,
                     const String& id)
{
    return node.theme_overrides.Find(id) >= 0 ||
           (overlay && overlay->Has(node.id,
                UiDesignerTransientValueKind::ThemeOverride, id));
}

inline Value ResolveValue(const UiDesignerNode& node,
                          const UiDesignerTransientOverlay* overlay,
                          const String& id,
                          const Value& canonical)
{
    return overlay
        ? overlay->Resolve(node.id, UiDesignerTransientValueKind::ThemeOverride,
                           id, canonical)
        : canonical;
}

inline String CppString(const String& text)
{
    String out = "\"";
    for(int i = 0; i < text.GetCount(); i++) {
        const byte c = text[i];
        if(c == '\\') out << "\\\\";
        else if(c == '"') out << "\\\"";
        else if(c == '\n') out << "\\n";
        else if(c == '\r') out << "\\r";
        else if(c == '\t') out << "\\t";
        else out.Cat(c);
    }
    out << "\"";
    return out;
}

inline String EmitValue(const Value& value)
{
    if(IsNull(value)) return "Null";
    if(value.Is<String>()) return CppString(AsString(value));
    if(value.Is<bool>()) return (bool)value ? "true" : "false";
    if(value.Is<int>() || value.Is<int64_t>()) return AsString(value);
    if(value.Is<double>()) return Format("%.12g", (double)value);
    if(value.Is<Color>()) {
        const Color c = value;
        return Format("Color(%d, %d, %d)", c.GetR(), c.GetG(), c.GetB());
    }
    return "ParseJSON(" + CppString(AsJSON(value, false)) + ")";
}

inline UiDesignerFillRecipe FillRecipe(const UiFill& fill)
{
    UiDesignerFillRecipe recipe;
    if(fill.IsNone()) {
        recipe.mode = "None";
        return recipe;
    }
    recipe.mode = "Solid";
    recipe.solid = fill.IsSolid() ? fill.color : SColorFace();
    recipe.top_left = recipe.solid;
    recipe.top_right = recipe.solid;
    recipe.bottom_left = recipe.solid;
    recipe.bottom_right = recipe.solid;
    return recipe;
}

inline UiDesignerFillRecipe FillRecipeValue(const Value& value)
{
    if(value.Is<Color>()) {
        UiDesignerFillRecipe recipe;
        recipe.mode = "Solid";
        recipe.solid = (Color)value;
        recipe.top_left = recipe.solid;
        recipe.top_right = recipe.solid;
        recipe.bottom_left = recipe.solid;
        recipe.bottom_right = recipe.solid;
        return recipe;
    }
    return UiDesignerFillRecipe::FromValue(value);
}

inline Value NormalizeFillValue(const Value& value)
{
    return FillRecipeValue(value).ToValue();
}

inline void ApplyFill(UiFill& target, const Value& value)
{
    const UiDesignerFillRecipe recipe = FillRecipeValue(value);
    if(recipe.mode == "Solid") {
        target = UiFill::Solid(recipe.solid);
        return;
    }
    if(recipe.mode == "QuadGradient") {
        target = UiFill::ImageFill(MakeQuadGradientTile(
            max(8, recipe.tile_size), recipe.top_left, recipe.top_right,
            recipe.bottom_left, recipe.bottom_right, max(0, recipe.blur)));
        return;
    }
    target = UiFill::None();
}

inline String FillCode(const Value& value)
{
    const UiDesignerFillRecipe recipe = FillRecipeValue(value);
    if(recipe.mode == "Solid")
        return "UiFill::Solid(" + EmitValue(recipe.solid) + ")";
    if(recipe.mode == "QuadGradient")
        return Format("UiFill::ImageFill(MakeQuadGradientTile(DPI(%d), %s, %s, %s, %s, %d))",
                      max(8, recipe.tile_size),
                      EmitValue(recipe.top_left), EmitValue(recipe.top_right),
                      EmitValue(recipe.bottom_left), EmitValue(recipe.bottom_right),
                      max(0, recipe.blur));
    return "UiFill::None()";
}

inline const char *StateCode(int state)
{
    static const char *code[] = { "ST_NORMAL", "ST_HOT", "ST_PRESSED", "ST_DISABLED" };
    return state >= 0 && state < 4 ? code[state] : "ST_NORMAL";
}

inline int DotState(const String& id, const String& prefix)
{
    const String token = prefix + ".";
    if(!id.StartsWith(token)) return -1;
    const String state = id.Mid(token.GetCount());
    if(state == "normal") return ST_NORMAL;
    if(state == "hot") return ST_HOT;
    if(state == "pressed") return ST_PRESSED;
    if(state == "disabled") return ST_DISABLED;
    return -1;
}

inline UiDesignerThemeOverrideSpec& Add(UiDesignerControlSpec& spec,
                                        const String& id,
                                        const String& label,
                                        const String& group,
                                        PropertyEditorKind kind,
                                        const Value& value,
                                        bool layout = false)
{
    UiDesignerThemeOverrideSpec item;
    item.id = id;
    item.label = label;
    item.group = group;
    item.kind = kind;
    item.domain = PropertyEditorDomain::Theme;
    item.default_value = value;
    item.impact = PropertyImpactPaint | PropertyImpactCode |
                  (layout ? PropertyImpactLocalLayout : PropertyImpactNone);
    item.adapter_field_id = id;
    return spec.theme_overrides.Add(pick(item));
}

inline UiDesignerThemeOverrideSpec& AddInt(UiDesignerControlSpec& spec,
                                           const String& id,
                                           const String& label,
                                           const String& group,
                                           int value, int minimum,
                                           int maximum, bool layout = false)
{
    UiDesignerThemeOverrideSpec& item = Add(spec, id, label, group,
        PropertyEditorKind::NumericInt, value, layout);
    item.Range(minimum, maximum, 1);
    return item;
}

inline Value AuthoredOrDefault(const UiDesignerNode& node,
                               const UiDesignerThemeOverrideSpec& property)
{
    const int q = node.theme_overrides.Find(property.id);
    Value value = q >= 0 ? node.theme_overrides.GetValue(q) : property.default_value;
    if(property.kind == PropertyEditorKind::FillRecipe)
        value = NormalizeFillValue(value);
    return value;
}

} // namespace UiDesignerNormalizedTheme
} // namespace Upp

#endif
