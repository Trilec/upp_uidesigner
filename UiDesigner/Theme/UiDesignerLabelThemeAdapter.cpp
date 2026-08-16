#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Core/UiDesignerOverlay.h>
#include <Ui/UiLabel.h>
#include <Ui/UiTheme.h>
#include <Ui/UiDraw.h>

namespace Upp {
namespace {

static UiRole LabelRole(const Value& value)
{
    const String role = AsString(value);
    if(role == "Subtle") return UiRole::Subtle;
    if(role == "Accent") return UiRole::Accent;
    if(role == "Alert") return UiRole::Alert;
    return UiRole::Standard;
}

static String LabelRoleExpr(const Value& value)
{
    const String role = AsString(value);
    if(role == "Subtle") return "UiRole::Subtle";
    if(role == "Accent") return "UiRole::Accent";
    if(role == "Alert") return "UiRole::Alert";
    return "UiRole::Standard";
}

static bool HasLabelThemeValue(const UiDesignerNode& node,
                               const UiDesignerTransientOverlay* overlay,
                               const String& id)
{
    return node.theme_overrides.Find(id) >= 0 ||
           (overlay && overlay->Has(node.id,
                UiDesignerTransientValueKind::ThemeOverride, id));
}

static Value ResolveLabelThemeValue(const UiDesignerNode& node,
                                    const UiDesignerTransientOverlay* overlay,
                                    const String& id,
                                    const Value& canonical)
{
    return overlay
        ? overlay->Resolve(node.id, UiDesignerTransientValueKind::ThemeOverride,
                           id, canonical)
        : canonical;
}

static String CppStringLabel(const String& text)
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

static String EmitLabelValue(const Value& value)
{
    if(IsNull(value))
        return "Value()";
    if(value.Is<String>())
        return CppStringLabel(AsString(value));
    if(value.Is<bool>())
        return (bool)value ? "true" : "false";
    if(value.Is<int>() || value.Is<int64_t>())
        return AsString(value);
    if(value.Is<double>())
        return Format("%.12g", (double)value);
    if(value.Is<Color>()) {
        const Color c = value;
        return Format("Color(%d, %d, %d)", c.GetR(), c.GetG(), c.GetB());
    }
    return "ParseJSON(" + CppStringLabel(AsJSON(value, false)) + ")";
}

static Color FillColor(const UiFill& fill)
{
    return fill.IsSolid() ? fill.color : Null;
}

static int StateIndex(const String& id, const char *prefix)
{
    const String token = String(prefix) + ".";
    if(!id.StartsWith(token))
        return -1;
    const String state = id.Mid(token.GetCount());
    if(state == "normal") return ST_NORMAL;
    if(state == "hot") return ST_HOT;
    if(state == "pressed") return ST_PRESSED;
    if(state == "disabled") return ST_DISABLED;
    return -1;
}

static const char *StateCode(int state)
{
    static const char *codes[] = {
        "ST_NORMAL", "ST_HOT", "ST_PRESSED", "ST_DISABLED"
    };
    return state >= 0 && state < 4 ? codes[state] : "ST_NORMAL";
}

static UiDesignerFillRecipe LabelFaceRecipe(const UiFill& fill)
{
    UiDesignerFillRecipe recipe;
    const Color color = FillColor(fill);
    recipe.mode = "Solid";
    recipe.solid = IsNull(color) ? White() : color;
    recipe.top_left = recipe.solid;
    recipe.top_right = recipe.solid;
    recipe.bottom_left = recipe.solid;
    recipe.bottom_right = recipe.solid;
    return recipe;
}

static void ApplyLabelFaceRecipe(UiFill& target, const Value& value)
{
    const UiDesignerFillRecipe recipe = UiDesignerFillRecipe::FromValue(value);
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

static String LabelFaceRecipeCode(const Value& value)
{
    const UiDesignerFillRecipe recipe = UiDesignerFillRecipe::FromValue(value);
    if(recipe.mode == "Solid")
        return "UiFill::Solid(" + EmitLabelValue(recipe.solid) + ")";
    if(recipe.mode == "QuadGradient") {
        return Format("UiFill::ImageFill(MakeQuadGradientTile(DPI(%d), %s, %s, %s, %s, %d))",
                      max(8, recipe.tile_size),
                      EmitLabelValue(recipe.top_left),
                      EmitLabelValue(recipe.top_right),
                      EmitLabelValue(recipe.bottom_left),
                      EmitLabelValue(recipe.bottom_right),
                      max(0, recipe.blur));
    }
    return "UiFill::None()";
}

static UiDesignerThemeOverrideSpec& AddLabelOverride(UiDesignerControlSpec& spec,
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

static UiDesignerThemeOverrideSpec& AddLabelInt(UiDesignerControlSpec& spec,
                                                 const String& id,
                                                 const String& label,
                                                 const String& group,
                                                 int value, int minimum,
                                                 int maximum,
                                                 bool layout = false)
{
    UiDesignerThemeOverrideSpec& item = AddLabelOverride(
        spec, id, label, group, PropertyEditorKind::NumericInt, value, layout);
    item.Range(minimum, maximum, 1);
    return item;
}

static Value MakeShadowCurveValue(const ShadowCurve& curve)
{
    ValueArray value;
    value.Add(curve.x1);
    value.Add(curve.y1);
    value.Add(curve.x2);
    value.Add(curve.y2);
    return value;
}

static void AddLabelThemeOverrides(UiDesignerControlSpec& spec)
{
    const UiLabel::Style base = UiTheme::ResolveLabel(UiRole::Standard);
    static const char *states[] = { "normal", "hot", "pressed", "disabled" };
    static const char *labels[] = { "Normal", "Hot", "Pressed", "Disabled" };

    AddLabelInt(spec, "radius", "Radius", "General",
                base.metrics.radius, 0, 96);
    AddLabelOverride(spec, "transparent", "Transparent", "General",
                     PropertyEditorKind::Boolean, base.transparent);
    AddLabelOverride(spec, "high_contrast", "High contrast", "General",
                     PropertyEditorKind::Boolean, base.metrics.high_contrast);

    AddLabelOverride(spec, "face_enabled", "Enabled", "Face",
                     PropertyEditorKind::Boolean, base.metrics.face_enabled);
    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "face." + String(states[i]), labels[i], "Face",
                         PropertyEditorKind::FillRecipe,
                         LabelFaceRecipe(base.palette.face[i]).ToValue());

    // Image-backed Skin remains intentionally deferred. The current adapter
    // contract has no UiDesignerDocument resource resolver, so exposing Skin
    // here would create a row that preview/codegen cannot truthfully consume.

    AddLabelOverride(spec, "frame_enabled", "Enabled", "Frame",
                     PropertyEditorKind::Boolean, base.metrics.frame_enabled, true);
    AddLabelInt(spec, "frame_width", "Width", "Frame",
                base.metrics.frame_width, 0, 24, true);
    AddLabelOverride(spec, "dashed", "Dashed", "Frame",
                     PropertyEditorKind::Boolean, base.metrics.dashed);
    AddLabelOverride(spec, "dash_pattern", "Dash pattern", "Frame",
                     PropertyEditorKind::Text, base.metrics.dash_pattern);
    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "frame." + String(states[i]), labels[i], "Frame",
                         PropertyEditorKind::Color,
                         IsNull(base.palette.frame[i])
                             ? Color(180, 186, 196) : base.palette.frame[i]);

    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "ink." + String(states[i]), labels[i], "Ink",
                         PropertyEditorKind::Color,
                         IsNull(base.palette.ink[i])
                             ? SColorText() : base.palette.ink[i]);

    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "icon." + String(states[i]), labels[i], "Icon",
                         PropertyEditorKind::Color,
                         IsNull(base.palette.icon[i])
                             ? SColorText() : base.palette.icon[i]);

    AddLabelOverride(spec, "font_face", "Font face", "Typography",
                     PropertyEditorKind::Text, base.font.GetFaceName(), true)
        .Editor("property.font");
    AddLabelInt(spec, "font_height", "Font height", "Typography",
                max(1, base.font.GetHeight()), 6, 96, true);
    AddLabelOverride(spec, "font_bold", "Bold", "Typography",
                     PropertyEditorKind::Boolean, base.font.IsBold(), true);
    AddLabelOverride(spec, "font_italic", "Italic", "Typography",
                     PropertyEditorKind::Boolean, base.font.IsItalic(), true);
    AddLabelOverride(spec, "underline", "Underline", "Typography",
                     PropertyEditorKind::Boolean, base.underline);
    AddLabelInt(spec, "underline_width", "Underline width", "Typography",
                base.underline_width, 1, 12);
    AddLabelInt(spec, "underline_offset", "Underline offset", "Typography",
                base.underline_offset, -20, 40);
    AddLabelOverride(spec, "nowrap", "No wrap", "Typography",
                     PropertyEditorKind::Boolean, base.nowrap, true);

    AddLabelInt(spec, "margin_left", "Left", "Content Margin",
                base.metrics.content_margin.left, 0, 80, true);
    AddLabelInt(spec, "margin_top", "Top", "Content Margin",
                base.metrics.content_margin.top, 0, 80, true);
    AddLabelInt(spec, "margin_right", "Right", "Content Margin",
                base.metrics.content_margin.right, 0, 80, true);
    AddLabelInt(spec, "margin_bottom", "Bottom", "Content Margin",
                base.metrics.content_margin.bottom, 0, 80, true);

    AddLabelOverride(spec, "focus_enabled", "Enabled", "Focus",
                     PropertyEditorKind::Boolean, base.metrics.focus_enabled);
    AddLabelInt(spec, "focus_margin", "Margin", "Focus",
                base.metrics.focus_margin, 0, 20);
    AddLabelInt(spec, "focus_alpha", "Alpha", "Focus",
                base.metrics.focus_alpha, 0, 255);
    AddLabelOverride(spec, "focus_color", "Colour", "Focus",
                     PropertyEditorKind::Color,
                     IsNull(base.metrics.focus_color)
                         ? Color(37, 99, 235) : base.metrics.focus_color);

    AddLabelOverride(spec, "shadow_enabled", "Enabled", "Shadow",
                     PropertyEditorKind::Boolean, base.metrics.shadow.enabled, true);
    AddLabelInt(spec, "shadow_distance", "Distance", "Shadow",
                base.metrics.shadow.distance, 0, 64, true);
    AddLabelInt(spec, "shadow_x", "Offset X", "Shadow",
                base.metrics.shadow.offset_x, -64, 64, true);
    AddLabelInt(spec, "shadow_y", "Offset Y", "Shadow",
                base.metrics.shadow.offset_y, -64, 64, true);
    AddLabelInt(spec, "shadow_alpha", "Alpha", "Shadow",
                base.metrics.shadow.alpha, 0, 255);
    AddLabelOverride(spec, "shadow_color", "Colour", "Shadow",
                     PropertyEditorKind::Color, base.metrics.shadow.color);
    AddLabelOverride(spec, "shadow_inset", "Inset", "Shadow",
                     PropertyEditorKind::Boolean, base.metrics.shadow.inset, true);
    AddLabelOverride(spec, "shadow_mode", "Mode", "Shadow",
                     PropertyEditorKind::Choice,
                     base.metrics.shadow.mode == SHADOW_HARD ? "Hard" : "Curve")
        .Choice("Hard", "Hard")
        .Choice("Curve", "Curve");
    UiDesignerThemeOverrideSpec& curve = AddLabelOverride(
        spec, "shadow_curve", "Falloff curve", "Shadow",
        PropertyEditorKind::Curve, MakeShadowCurveValue(base.metrics.shadow.curve));
    curve.editor_variant = "bezier";
    curve.Range(0.0, 1.0, 0.001);

    AddLabelOverride(spec, "highlight_enabled", "Enabled", "Highlight",
                     PropertyEditorKind::Boolean, base.metrics.highlight.enabled);
    AddLabelInt(spec, "highlight_thickness", "Thickness", "Highlight",
                base.metrics.highlight.thickness, 0, 24);
    AddLabelInt(spec, "highlight_x", "Offset X", "Highlight",
                base.metrics.highlight.offset_x, -32, 32);
    AddLabelInt(spec, "highlight_y", "Offset Y", "Highlight",
                base.metrics.highlight.offset_y, -32, 32);
    AddLabelInt(spec, "highlight_alpha", "Alpha", "Highlight",
                base.metrics.highlight.alpha, 0, 255);
    AddLabelOverride(spec, "highlight_color", "Colour", "Highlight",
                     PropertyEditorKind::Color, base.metrics.highlight.color);
    AddLabelOverride(spec, "highlight_style", "Style", "Highlight",
                     PropertyEditorKind::Choice, "Solid")
        .Choice("Solid", "Solid")
        .Choice("Dashed", "Dashed")
        .Choice("Dotted", "Dotted");
}

static bool IsLabelField(const String& id)
{
    static const char *fields[] = {
        "radius", "transparent", "high_contrast", "face_enabled",
        "frame_enabled", "frame_width", "dashed", "dash_pattern",
        "font_face", "font_height", "font_bold", "font_italic",
        "underline", "underline_width", "underline_offset", "nowrap",
        "margin_left", "margin_top", "margin_right", "margin_bottom",
        "focus_enabled", "focus_margin", "focus_alpha", "focus_color",
        "shadow_enabled", "shadow_distance", "shadow_x", "shadow_y",
        "shadow_alpha", "shadow_color", "shadow_inset", "shadow_mode",
        "shadow_curve", "highlight_enabled", "highlight_thickness",
        "highlight_x", "highlight_y", "highlight_alpha", "highlight_color",
        "highlight_style"
    };
    if(StateIndex(id, "face") >= 0 || StateIndex(id, "frame") >= 0 ||
       StateIndex(id, "ink") >= 0 || StateIndex(id, "icon") >= 0)
        return true;
    for(const char *field : fields)
        if(id == field)
            return true;
    return false;
}

static void ApplyLabelField(UiLabel::Style& style,
                            const String& id,
                            const Value& value)
{
    int state = StateIndex(id, "face");
    if(state >= 0) {
        ApplyLabelFaceRecipe(style.palette.face[state], value);
        return;
    }
    state = StateIndex(id, "frame");
    if(state >= 0) {
        style.palette.frame[state] = (Color)value;
        return;
    }
    state = StateIndex(id, "ink");
    if(state >= 0) {
        style.palette.ink[state] = (Color)value;
        return;
    }
    state = StateIndex(id, "icon");
    if(state >= 0) {
        style.palette.icon[state] = (Color)value;
        return;
    }

    if(id == "radius") style.metrics.radius = max(0, (int)value);
    else if(id == "transparent") style.transparent = (bool)value;
    else if(id == "high_contrast") style.metrics.high_contrast = (bool)value;
    else if(id == "face_enabled") style.metrics.face_enabled = (bool)value;
    else if(id == "frame_enabled") style.metrics.frame_enabled = (bool)value;
    else if(id == "frame_width") style.metrics.frame_width = max(0, (int)value);
    else if(id == "dashed") style.metrics.dashed = (bool)value;
    else if(id == "dash_pattern") style.metrics.dash_pattern = AsString(value);
    else if(id == "font_face") style.font.FaceName(AsString(value));
    else if(id == "font_height") style.font.Height(max(1, (int)value));
    else if(id == "font_bold") style.font.Bold((bool)value);
    else if(id == "font_italic") style.font.Italic((bool)value);
    else if(id == "underline") style.underline = (bool)value;
    else if(id == "underline_width") style.underline_width = max(0, (int)value);
    else if(id == "underline_offset") style.underline_offset = (int)value;
    else if(id == "nowrap") style.nowrap = (bool)value;
    else if(id == "margin_left") style.metrics.content_margin.left = max(0, (int)value);
    else if(id == "margin_top") style.metrics.content_margin.top = max(0, (int)value);
    else if(id == "margin_right") style.metrics.content_margin.right = max(0, (int)value);
    else if(id == "margin_bottom") style.metrics.content_margin.bottom = max(0, (int)value);
    else if(id == "focus_enabled") style.metrics.focus_enabled = (bool)value;
    else if(id == "focus_margin") style.metrics.focus_margin = max(0, (int)value);
    else if(id == "focus_alpha") style.metrics.focus_alpha = minmax((int)value, 0, 255);
    else if(id == "focus_color") style.metrics.focus_color = (Color)value;
    else if(id == "shadow_enabled") style.metrics.shadow.enabled = (bool)value;
    else if(id == "shadow_distance") style.metrics.shadow.distance = max(0, (int)value);
    else if(id == "shadow_x") style.metrics.shadow.offset_x = (int)value;
    else if(id == "shadow_y") style.metrics.shadow.offset_y = (int)value;
    else if(id == "shadow_alpha") style.metrics.shadow.alpha = minmax((int)value, 0, 255);
    else if(id == "shadow_color") style.metrics.shadow.color = (Color)value;
    else if(id == "shadow_inset") style.metrics.shadow.inset = (bool)value;
    else if(id == "shadow_mode") style.metrics.shadow.mode = AsString(value) == "Hard" ? SHADOW_HARD : SHADOW_CURVE;
    else if(id == "shadow_curve") {
        ValueArray v = value;
        if(v.GetCount() >= 4)
            style.metrics.shadow.curve = ShadowCurve {
                (double)v[0], (double)v[1], (double)v[2], (double)v[3]
            };
    }
    else if(id == "highlight_enabled") style.metrics.highlight.enabled = (bool)value;
    else if(id == "highlight_thickness") style.metrics.highlight.thickness = max(0, (int)value);
    else if(id == "highlight_x") style.metrics.highlight.offset_x = (int)value;
    else if(id == "highlight_y") style.metrics.highlight.offset_y = (int)value;
    else if(id == "highlight_alpha") style.metrics.highlight.alpha = minmax((int)value, 0, 255);
    else if(id == "highlight_color") style.metrics.highlight.color = (Color)value;
    else if(id == "highlight_style") {
        const String v = AsString(value);
        style.metrics.highlight.style = v == "Dashed" ? DASHED
                                      : v == "Dotted" ? DOTTED : SOLID;
    }
}

static Value LabelFieldValue(const UiLabel::Style& style, const String& id)
{
    int state = StateIndex(id, "face");
    if(state >= 0)
        return LabelFaceRecipe(style.palette.face[state]).ToValue();
    state = StateIndex(id, "frame");
    if(state >= 0) return style.palette.frame[state];
    state = StateIndex(id, "ink");
    if(state >= 0) return style.palette.ink[state];
    state = StateIndex(id, "icon");
    if(state >= 0) return style.palette.icon[state];

    if(id == "radius") return style.metrics.radius;
    if(id == "transparent") return style.transparent;
    if(id == "high_contrast") return style.metrics.high_contrast;
    if(id == "face_enabled") return style.metrics.face_enabled;
    if(id == "frame_enabled") return style.metrics.frame_enabled;
    if(id == "frame_width") return style.metrics.frame_width;
    if(id == "dashed") return style.metrics.dashed;
    if(id == "dash_pattern") return style.metrics.dash_pattern;
    if(id == "font_face") return style.font.GetFaceName();
    if(id == "font_height") return style.font.GetHeight();
    if(id == "font_bold") return style.font.IsBold();
    if(id == "font_italic") return style.font.IsItalic();
    if(id == "underline") return style.underline;
    if(id == "underline_width") return style.underline_width;
    if(id == "underline_offset") return style.underline_offset;
    if(id == "nowrap") return style.nowrap;
    if(id == "margin_left") return style.metrics.content_margin.left;
    if(id == "margin_top") return style.metrics.content_margin.top;
    if(id == "margin_right") return style.metrics.content_margin.right;
    if(id == "margin_bottom") return style.metrics.content_margin.bottom;
    if(id == "focus_enabled") return style.metrics.focus_enabled;
    if(id == "focus_margin") return style.metrics.focus_margin;
    if(id == "focus_alpha") return style.metrics.focus_alpha;
    if(id == "focus_color") return style.metrics.focus_color;
    if(id == "shadow_enabled") return style.metrics.shadow.enabled;
    if(id == "shadow_distance") return style.metrics.shadow.distance;
    if(id == "shadow_x") return style.metrics.shadow.offset_x;
    if(id == "shadow_y") return style.metrics.shadow.offset_y;
    if(id == "shadow_alpha") return style.metrics.shadow.alpha;
    if(id == "shadow_color") return style.metrics.shadow.color;
    if(id == "shadow_inset") return style.metrics.shadow.inset;
    if(id == "shadow_mode") return style.metrics.shadow.mode == SHADOW_HARD ? "Hard" : "Curve";
    if(id == "shadow_curve") return MakeShadowCurveValue(style.metrics.shadow.curve);
    if(id == "highlight_enabled") return style.metrics.highlight.enabled;
    if(id == "highlight_thickness") return style.metrics.highlight.thickness;
    if(id == "highlight_x") return style.metrics.highlight.offset_x;
    if(id == "highlight_y") return style.metrics.highlight.offset_y;
    if(id == "highlight_alpha") return style.metrics.highlight.alpha;
    if(id == "highlight_color") return style.metrics.highlight.color;
    if(id == "highlight_style")
        return style.metrics.highlight.style == DASHED ? "Dashed"
             : style.metrics.highlight.style == DOTTED ? "Dotted" : "Solid";
    return Value();
}

static bool LabelFieldAffectsLayout(const String& id)
{
    return id == "frame_enabled" || id == "frame_width" ||
           id == "font_face" || id == "font_height" ||
           id == "font_bold" || id == "font_italic" || id == "nowrap" ||
           id.StartsWith("margin_") || id == "shadow_enabled" ||
           id == "shadow_distance" || id == "shadow_x" ||
           id == "shadow_y" || id == "shadow_inset";
}

static Value ResolveLabelFaceRecipe(const UiDesignerNode& node,
                                    const UiDesignerControlSpec& spec,
                                    const String& id,
                                    const UiDesignerTransientOverlay* overlay,
                                    const UiLabel::Style& base)
{
    const int state = StateIndex(id, "face");
    if(state < 0)
        return Value();

    const Value inherited = LabelFaceRecipe(base.palette.face[state]).ToValue();
    for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
        if(property.adapter_field_id != id)
            continue;
        const int q = node.theme_overrides.Find(property.id);
        if(q < 0 && !HasLabelThemeValue(node, overlay, property.id))
            return inherited;
        const Value canonical = q >= 0
            ? node.theme_overrides.GetValue(q) : property.default_value;
        return ResolveLabelThemeValue(node, overlay, property.id, canonical);
    }
    return inherited;
}

static void EmitLabelField(String& out, const String& var,
                           const String& id, const Value& value)
{
    int state = StateIndex(id, "face");
    if(state >= 0) {
        out << "\t" << var << ".palette.face[" << StateCode(state)
            << "] = " << LabelFaceRecipeCode(value) << ";\n";
        return;
    }
    state = StateIndex(id, "frame");
    if(state >= 0) {
        out << "\t" << var << ".palette.frame[" << StateCode(state)
            << "] = " << EmitLabelValue(value) << ";\n";
        return;
    }
    state = StateIndex(id, "ink");
    if(state >= 0) {
        out << "\t" << var << ".palette.ink[" << StateCode(state)
            << "] = " << EmitLabelValue(value) << ";\n";
        return;
    }
    state = StateIndex(id, "icon");
    if(state >= 0) {
        out << "\t" << var << ".palette.icon[" << StateCode(state)
            << "] = " << EmitLabelValue(value) << ";\n";
        return;
    }

    if(id == "radius") out << "\t" << var << ".metrics.radius = " << max(0, (int)value) << ";\n";
    else if(id == "transparent") out << "\t" << var << ".transparent = " << AsString((bool)value) << ";\n";
    else if(id == "high_contrast") out << "\t" << var << ".metrics.high_contrast = " << AsString((bool)value) << ";\n";
    else if(id == "face_enabled") out << "\t" << var << ".metrics.face_enabled = " << AsString((bool)value) << ";\n";
    else if(id == "frame_enabled") out << "\t" << var << ".metrics.frame_enabled = " << AsString((bool)value) << ";\n";
    else if(id == "frame_width") out << "\t" << var << ".metrics.frame_width = " << max(0, (int)value) << ";\n";
    else if(id == "dashed") out << "\t" << var << ".metrics.dashed = " << AsString((bool)value) << ";\n";
    else if(id == "dash_pattern") out << "\t" << var << ".metrics.dash_pattern = " << EmitLabelValue(value) << ";\n";
    else if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppStringLabel(AsString(value)) << ");\n";
    else if(id == "font_height") out << "\t" << var << ".font.Height(" << max(1, (int)value) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << AsString((bool)value) << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << AsString((bool)value) << ");\n";
    else if(id == "underline") out << "\t" << var << ".underline = " << AsString((bool)value) << ";\n";
    else if(id == "underline_width") out << "\t" << var << ".underline_width = " << max(0, (int)value) << ";\n";
    else if(id == "underline_offset") out << "\t" << var << ".underline_offset = " << (int)value << ";\n";
    else if(id == "nowrap") out << "\t" << var << ".nowrap = " << AsString((bool)value) << ";\n";
    else if(id.StartsWith("margin_")) {
        const String side = id.Mid(7);
        out << "\t" << var << ".metrics.content_margin." << side
            << " = " << max(0, (int)value) << ";\n";
    }
    else if(id == "focus_enabled") out << "\t" << var << ".metrics.focus_enabled = " << AsString((bool)value) << ";\n";
    else if(id == "focus_margin") out << "\t" << var << ".metrics.focus_margin = " << max(0, (int)value) << ";\n";
    else if(id == "focus_alpha") out << "\t" << var << ".metrics.focus_alpha = " << minmax((int)value, 0, 255) << ";\n";
    else if(id == "focus_color") out << "\t" << var << ".metrics.focus_color = " << EmitLabelValue(value) << ";\n";
    else if(id == "shadow_enabled") out << "\t" << var << ".metrics.shadow.enabled = " << AsString((bool)value) << ";\n";
    else if(id == "shadow_distance") out << "\t" << var << ".metrics.shadow.distance = " << max(0, (int)value) << ";\n";
    else if(id == "shadow_x") out << "\t" << var << ".metrics.shadow.offset_x = " << (int)value << ";\n";
    else if(id == "shadow_y") out << "\t" << var << ".metrics.shadow.offset_y = " << (int)value << ";\n";
    else if(id == "shadow_alpha") out << "\t" << var << ".metrics.shadow.alpha = " << minmax((int)value, 0, 255) << ";\n";
    else if(id == "shadow_color") out << "\t" << var << ".metrics.shadow.color = " << EmitLabelValue(value) << ";\n";
    else if(id == "shadow_inset") out << "\t" << var << ".metrics.shadow.inset = " << AsString((bool)value) << ";\n";
    else if(id == "shadow_mode") out << "\t" << var << ".metrics.shadow.mode = " << (AsString(value) == "Hard" ? "SHADOW_HARD" : "SHADOW_CURVE") << ";\n";
    else if(id == "shadow_curve") {
        ValueArray v = value;
        if(v.GetCount() >= 4)
            out << "\t" << var << ".metrics.shadow.curve = ShadowCurve { "
                << (double)v[0] << ", " << (double)v[1] << ", "
                << (double)v[2] << ", " << (double)v[3] << " };\n";
    }
    else if(id == "highlight_enabled") out << "\t" << var << ".metrics.highlight.enabled = " << AsString((bool)value) << ";\n";
    else if(id == "highlight_thickness") out << "\t" << var << ".metrics.highlight.thickness = " << max(0, (int)value) << ";\n";
    else if(id == "highlight_x") out << "\t" << var << ".metrics.highlight.offset_x = " << (int)value << ";\n";
    else if(id == "highlight_y") out << "\t" << var << ".metrics.highlight.offset_y = " << (int)value << ";\n";
    else if(id == "highlight_alpha") out << "\t" << var << ".metrics.highlight.alpha = " << minmax((int)value, 0, 255) << ";\n";
    else if(id == "highlight_color") out << "\t" << var << ".metrics.highlight.color = " << EmitLabelValue(value) << ";\n";
    else if(id == "highlight_style") out << "\t" << var << ".metrics.highlight.style = " << (AsString(value) == "Dashed" ? "DASHED" : AsString(value) == "Dotted" ? "DOTTED" : "SOLID") << ";\n";
}

class LabelThemeAdapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "label"; }

    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return kind == UiDesignerRuntimeKind::UiLabel;
    }

    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        AddLabelThemeOverrides(spec);
    }

    bool HasField(const String& id) const override
    {
        return IsLabelField(id);
    }

    bool FieldAffectsLayout(const String& id) const override
    {
        return LabelFieldAffectsLayout(id);
    }

    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiLabel::Style style = UiTheme::ResolveLabel(
            LabelRole(node.GetProperty("role", "Standard")));

        if(StateIndex(id, "face") >= 0)
            return ResolveLabelFaceRecipe(node, spec, id, overlay, style);

        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(property.id);
            if(q < 0 && !HasLabelThemeValue(node, overlay, property.id))
                continue;
            const Value canonical = q >= 0
                ? node.theme_overrides.GetValue(q) : property.default_value;
            ApplyLabelField(style, property.adapter_field_id,
                            ResolveLabelThemeValue(node, overlay,
                                                   property.id, canonical));
        }
        return LabelFieldValue(style, id);
    }

    void ApplyPreviewStyle(Ctrl& ctrl,
                           const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiLabel *label = dynamic_cast<UiLabel *>(&ctrl);
        if(!label)
            return;

        UiLabel::Style style = UiTheme::ResolveLabel(
            LabelRole(node.GetProperty("role", "Standard")));
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(property.id);
            if(q < 0 && !HasLabelThemeValue(node, overlay, property.id))
                continue;
            authored = true;
            const Value canonical = q >= 0
                ? node.theme_overrides.GetValue(q) : property.default_value;
            ApplyLabelField(style, property.adapter_field_id,
                            ResolveLabelThemeValue(node, overlay,
                                                   property.id, canonical));
        }

        if(authored ||
           LabelRole(node.GetProperty("role", "Standard")) != UiRole::Standard)
            label->SetCustomStyle(style);
        else
            label->ClearCustomStyle();
    }

    void EmitSetup(String& out,
                   const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides)
            authored |= node.theme_overrides.Find(property.id) >= 0;

        const Value role = node.GetProperty("role", "Standard");
        if(!authored && LabelRole(role) == UiRole::Standard)
            return;

        const String var = member + "_style";
        out << "\tUiLabel::Style " << var << " = UiTheme::ResolveLabel("
            << LabelRoleExpr(role) << ");\n";
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(property.id);
            if(q >= 0)
                EmitLabelField(out, var, property.adapter_field_id,
                               node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

} // namespace

const UiDesignerThemeAdapter& UiDesignerLabelThemeAdapterInstance()
{
    static LabelThemeAdapter adapter;
    return adapter;
}

} // namespace Upp
