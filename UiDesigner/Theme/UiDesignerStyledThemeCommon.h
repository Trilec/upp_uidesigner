#ifndef _UiDesigner_Theme_UiDesignerStyledThemeCommon_h_
#define _UiDesigner_Theme_UiDesignerStyledThemeCommon_h_

#include "UiDesignerNormalizedThemeCommon.h"

namespace Upp {
namespace UiDesignerStyledTheme {

using namespace UiDesignerNormalizedTheme;

inline String Id(const String& prefix, const String& leaf)
{
    return prefix.IsEmpty() ? leaf : prefix + "_" + leaf;
}

inline String Group(const String& root, const String& leaf)
{
    return root.IsEmpty() ? leaf : root + " / " + leaf;
}

inline int StateSuffix(const String& id, const String& prefix,
                       const String& stem)
{
    const String token = Id(prefix, stem + "_");
    if(!id.StartsWith(token))
        return -1;
    const String suffix = id.Mid(token.GetCount());
    if(suffix == "normal") return ST_NORMAL;
    if(suffix == "hot") return ST_HOT;
    if(suffix == "pressed") return ST_PRESSED;
    if(suffix == "disabled") return ST_DISABLED;
    return -1;
}

inline Value ShadowCurveValue(const ShadowCurve& curve)
{
    ValueArray out;
    out.Add(curve.x1);
    out.Add(curve.y1);
    out.Add(curve.x2);
    out.Add(curve.y2);
    return out;
}

inline ShadowCurve ShadowCurveFromValue(const Value& value,
                                        const ShadowCurve& fallback)
{
    if(!value.Is<ValueArray>())
        return fallback;
    ValueArray v = value;
    if(v.GetCount() < 4)
        return fallback;
    ShadowCurve out;
    out.x1 = minmax((double)v[0], 0.0, 1.0);
    out.y1 = minmax((double)v[1], 0.0, 1.0);
    out.x2 = minmax((double)v[2], 0.0, 1.0);
    out.y2 = minmax((double)v[3], 0.0, 1.0);
    return out;
}

inline String LineStyleName(UiLineStyle style)
{
    return style == DASHED ? "Dashed" : style == DOTTED ? "Dotted" : "Solid";
}

inline UiLineStyle LineStyleFromValue(const Value& value)
{
    const String s = AsString(value);
    return s == "Dashed" ? DASHED : s == "Dotted" ? DOTTED : SOLID;
}

inline String ShadowModeName(ShadowMode mode)
{
    return mode == SHADOW_HARD ? "Hard" : "Curve";
}

inline ShadowMode ShadowModeFromValue(const Value& value)
{
    return AsString(value) == "Hard" ? SHADOW_HARD : SHADOW_CURVE;
}

inline UiDesignerThemeOverrideSpec& AddNumeric(
    UiDesignerControlSpec& spec, const String& id, const String& label,
    const String& group, int value, int minimum, int maximum,
    bool layout = false)
{
    UiDesignerThemeOverrideSpec& item = Add(
        spec, id, label, group, PropertyEditorKind::NumericInt, value, layout);
    item.Range(minimum, maximum, 1);
    return item;
}

inline void AddPaletteMetrics(UiDesignerControlSpec& spec,
                              const String& prefix,
                              const String& group_root,
                              const StyledPalette& palette,
                              const StyledMetrics& metrics,
                              bool include_text_ink = true,
                              bool include_icon_ink = true,
                              bool include_content_margin = true,
                              bool include_focus = true,
                              bool include_shadow = true,
                              bool include_highlight = true)
{
    static const char *states[] = { "normal", "hot", "pressed", "disabled" };
    static const char *labels[] = { "Normal", "Hot", "Pressed", "Disabled" };

    AddNumeric(spec, Id(prefix, "radius"), "Radius",
               Group(group_root, "General"), metrics.radius, 0, 999, true);
    Add(spec, Id(prefix, "high_contrast"), "High contrast",
        Group(group_root, "General"), PropertyEditorKind::Boolean,
        metrics.high_contrast);

    Add(spec, Id(prefix, "face_enabled"), "Enabled",
        Group(group_root, "Face"), PropertyEditorKind::Boolean,
        metrics.face_enabled, true);
    for(int i = 0; i < 4; ++i)
        Add(spec, Id(prefix, "face_" + String(states[i])), labels[i],
            Group(group_root, "Face"), PropertyEditorKind::FillRecipe,
            FillRecipe(palette.face[i]).ToValue());

    Add(spec, Id(prefix, "frame_enabled"), "Enabled",
        Group(group_root, "Frame"), PropertyEditorKind::Boolean,
        metrics.frame_enabled, true);
    AddNumeric(spec, Id(prefix, "frame_width"), "Width",
               Group(group_root, "Frame"), metrics.frame_width, 0, 64, true);
    Add(spec, Id(prefix, "frame_dashed"), "Dashed",
        Group(group_root, "Frame"), PropertyEditorKind::Boolean,
        metrics.dashed);
    Add(spec, Id(prefix, "frame_dash_pattern"), "Dash pattern",
        Group(group_root, "Frame"), PropertyEditorKind::Text,
        metrics.dash_pattern);
    for(int i = 0; i < 4; ++i)
        Add(spec, Id(prefix, "frame_" + String(states[i])), labels[i],
            Group(group_root, "Frame"), PropertyEditorKind::Color,
            palette.frame[i]);

    if(include_text_ink)
        for(int i = 0; i < 4; ++i)
            Add(spec, Id(prefix, "ink_" + String(states[i])), labels[i],
                Group(group_root, "Text Ink"), PropertyEditorKind::Color,
                palette.ink[i]);

    if(include_icon_ink)
        for(int i = 0; i < 4; ++i)
            Add(spec, Id(prefix, "icon_" + String(states[i])), labels[i],
                Group(group_root, "Icon Ink"), PropertyEditorKind::Color,
                palette.icon[i]);

    if(include_content_margin) {
        AddNumeric(spec, Id(prefix, "margin_left"), "Left",
                   Group(group_root, "Content Margin"),
                   metrics.content_margin.left, 0, 256, true);
        AddNumeric(spec, Id(prefix, "margin_top"), "Top",
                   Group(group_root, "Content Margin"),
                   metrics.content_margin.top, 0, 256, true);
        AddNumeric(spec, Id(prefix, "margin_right"), "Right",
                   Group(group_root, "Content Margin"),
                   metrics.content_margin.right, 0, 256, true);
        AddNumeric(spec, Id(prefix, "margin_bottom"), "Bottom",
                   Group(group_root, "Content Margin"),
                   metrics.content_margin.bottom, 0, 256, true);
    }

    if(include_focus) {
        Add(spec, Id(prefix, "focus_enabled"), "Enabled",
            Group(group_root, "Focus"), PropertyEditorKind::Boolean,
            metrics.focus_enabled);
        AddNumeric(spec, Id(prefix, "focus_margin"), "Margin",
                   Group(group_root, "Focus"), metrics.focus_margin, 0, 64);
        AddNumeric(spec, Id(prefix, "focus_alpha"), "Alpha",
                   Group(group_root, "Focus"), metrics.focus_alpha, 0, 255);
        Add(spec, Id(prefix, "focus_color"), "Colour",
            Group(group_root, "Focus"), PropertyEditorKind::Color,
            metrics.focus_color);
    }

    if(include_shadow) {
        Add(spec, Id(prefix, "shadow_enabled"), "Enabled",
            Group(group_root, "Shadow"), PropertyEditorKind::Boolean,
            metrics.shadow.enabled, true);
        AddNumeric(spec, Id(prefix, "shadow_distance"), "Distance",
                   Group(group_root, "Shadow"), metrics.shadow.distance, 0, 128, true);
        AddNumeric(spec, Id(prefix, "shadow_x"), "Offset X",
                   Group(group_root, "Shadow"), metrics.shadow.offset_x, -128, 128, true);
        AddNumeric(spec, Id(prefix, "shadow_y"), "Offset Y",
                   Group(group_root, "Shadow"), metrics.shadow.offset_y, -128, 128, true);
        AddNumeric(spec, Id(prefix, "shadow_alpha"), "Alpha",
                   Group(group_root, "Shadow"), metrics.shadow.alpha, 0, 255);
        Add(spec, Id(prefix, "shadow_color"), "Colour",
            Group(group_root, "Shadow"), PropertyEditorKind::Color,
            metrics.shadow.color);
        Add(spec, Id(prefix, "shadow_inset"), "Inset",
            Group(group_root, "Shadow"), PropertyEditorKind::Boolean,
            metrics.shadow.inset, true);
        Add(spec, Id(prefix, "shadow_mode"), "Mode",
            Group(group_root, "Shadow"), PropertyEditorKind::Choice,
            ShadowModeName(metrics.shadow.mode))
            .Choice("Hard", "Hard")
            .Choice("Curve", "Curve");
        UiDesignerThemeOverrideSpec& curve = Add(
            spec, Id(prefix, "shadow_curve"), "Falloff curve",
            Group(group_root, "Shadow"), PropertyEditorKind::Curve,
            ShadowCurveValue(metrics.shadow.curve));
        curve.editor_variant = "bezier";
        curve.Range(0.0, 1.0, 0.001);
    }

    if(include_highlight) {
        Add(spec, Id(prefix, "highlight_enabled"), "Enabled",
            Group(group_root, "Highlight"), PropertyEditorKind::Boolean,
            metrics.highlight.enabled);
        AddNumeric(spec, Id(prefix, "highlight_thickness"), "Thickness",
                   Group(group_root, "Highlight"), metrics.highlight.thickness, 0, 64);
        AddNumeric(spec, Id(prefix, "highlight_x"), "Offset X",
                   Group(group_root, "Highlight"), metrics.highlight.offset_x, -64, 64);
        AddNumeric(spec, Id(prefix, "highlight_y"), "Offset Y",
                   Group(group_root, "Highlight"), metrics.highlight.offset_y, -64, 64);
        AddNumeric(spec, Id(prefix, "highlight_alpha"), "Alpha",
                   Group(group_root, "Highlight"), metrics.highlight.alpha, 0, 255);
        Add(spec, Id(prefix, "highlight_color"), "Colour",
            Group(group_root, "Highlight"), PropertyEditorKind::Color,
            metrics.highlight.color);
        Add(spec, Id(prefix, "highlight_style"), "Style",
            Group(group_root, "Highlight"), PropertyEditorKind::Choice,
            LineStyleName(metrics.highlight.style))
            .Choice("Solid", "Solid")
            .Choice("Dashed", "Dashed")
            .Choice("Dotted", "Dotted");
    }
}

inline bool IsPaletteMetricsField(const String& prefix, const String& id)
{
    if(StateSuffix(id, prefix, "face") >= 0 ||
       StateSuffix(id, prefix, "frame") >= 0 ||
       StateSuffix(id, prefix, "ink") >= 0 ||
       StateSuffix(id, prefix, "icon") >= 0)
        return true;

    static const char *fields[] = {
        "radius", "high_contrast", "face_enabled",
        "frame_enabled", "frame_width", "frame_dashed", "frame_dash_pattern",
        "margin_left", "margin_top", "margin_right", "margin_bottom",
        "focus_enabled", "focus_margin", "focus_alpha", "focus_color",
        "shadow_enabled", "shadow_distance", "shadow_x", "shadow_y",
        "shadow_alpha", "shadow_color", "shadow_inset", "shadow_mode",
        "shadow_curve", "highlight_enabled", "highlight_thickness",
        "highlight_x", "highlight_y", "highlight_alpha", "highlight_color",
        "highlight_style"
    };
    for(const char *field : fields)
        if(id == Id(prefix, field))
            return true;
    return false;
}

inline Value PaletteMetricsValue(const StyledPalette& palette,
                                 const StyledMetrics& metrics,
                                 const String& prefix,
                                 const String& id)
{
    int state = StateSuffix(id, prefix, "face");
    if(state >= 0) return FillRecipe(palette.face[state]).ToValue();
    state = StateSuffix(id, prefix, "frame");
    if(state >= 0) return palette.frame[state];
    state = StateSuffix(id, prefix, "ink");
    if(state >= 0) return palette.ink[state];
    state = StateSuffix(id, prefix, "icon");
    if(state >= 0) return palette.icon[state];

    if(id == Id(prefix, "radius")) return metrics.radius;
    if(id == Id(prefix, "high_contrast")) return metrics.high_contrast;
    if(id == Id(prefix, "face_enabled")) return metrics.face_enabled;
    if(id == Id(prefix, "frame_enabled")) return metrics.frame_enabled;
    if(id == Id(prefix, "frame_width")) return metrics.frame_width;
    if(id == Id(prefix, "frame_dashed")) return metrics.dashed;
    if(id == Id(prefix, "frame_dash_pattern")) return metrics.dash_pattern;
    if(id == Id(prefix, "margin_left")) return metrics.content_margin.left;
    if(id == Id(prefix, "margin_top")) return metrics.content_margin.top;
    if(id == Id(prefix, "margin_right")) return metrics.content_margin.right;
    if(id == Id(prefix, "margin_bottom")) return metrics.content_margin.bottom;
    if(id == Id(prefix, "focus_enabled")) return metrics.focus_enabled;
    if(id == Id(prefix, "focus_margin")) return metrics.focus_margin;
    if(id == Id(prefix, "focus_alpha")) return metrics.focus_alpha;
    if(id == Id(prefix, "focus_color")) return metrics.focus_color;
    if(id == Id(prefix, "shadow_enabled")) return metrics.shadow.enabled;
    if(id == Id(prefix, "shadow_distance")) return metrics.shadow.distance;
    if(id == Id(prefix, "shadow_x")) return metrics.shadow.offset_x;
    if(id == Id(prefix, "shadow_y")) return metrics.shadow.offset_y;
    if(id == Id(prefix, "shadow_alpha")) return metrics.shadow.alpha;
    if(id == Id(prefix, "shadow_color")) return metrics.shadow.color;
    if(id == Id(prefix, "shadow_inset")) return metrics.shadow.inset;
    if(id == Id(prefix, "shadow_mode")) return ShadowModeName(metrics.shadow.mode);
    if(id == Id(prefix, "shadow_curve")) return ShadowCurveValue(metrics.shadow.curve);
    if(id == Id(prefix, "highlight_enabled")) return metrics.highlight.enabled;
    if(id == Id(prefix, "highlight_thickness")) return metrics.highlight.thickness;
    if(id == Id(prefix, "highlight_x")) return metrics.highlight.offset_x;
    if(id == Id(prefix, "highlight_y")) return metrics.highlight.offset_y;
    if(id == Id(prefix, "highlight_alpha")) return metrics.highlight.alpha;
    if(id == Id(prefix, "highlight_color")) return metrics.highlight.color;
    if(id == Id(prefix, "highlight_style")) return LineStyleName(metrics.highlight.style);
    return Value();
}

inline bool ApplyPaletteMetrics(StyledPalette& palette,
                                StyledMetrics& metrics,
                                const String& prefix,
                                const String& id,
                                const Value& value)
{
    int state = StateSuffix(id, prefix, "face");
    if(state >= 0) { ApplyFill(palette.face[state], value); return true; }
    state = StateSuffix(id, prefix, "frame");
    if(state >= 0) { palette.frame[state] = (Color)value; return true; }
    state = StateSuffix(id, prefix, "ink");
    if(state >= 0) { palette.ink[state] = (Color)value; return true; }
    state = StateSuffix(id, prefix, "icon");
    if(state >= 0) { palette.icon[state] = (Color)value; return true; }

    if(id == Id(prefix, "radius")) metrics.radius = max(0, (int)value);
    else if(id == Id(prefix, "high_contrast")) metrics.high_contrast = (bool)value;
    else if(id == Id(prefix, "face_enabled")) metrics.face_enabled = (bool)value;
    else if(id == Id(prefix, "frame_enabled")) metrics.frame_enabled = (bool)value;
    else if(id == Id(prefix, "frame_width")) metrics.frame_width = max(0, (int)value);
    else if(id == Id(prefix, "frame_dashed")) metrics.dashed = (bool)value;
    else if(id == Id(prefix, "frame_dash_pattern")) metrics.dash_pattern = AsString(value);
    else if(id == Id(prefix, "margin_left")) metrics.content_margin.left = max(0, (int)value);
    else if(id == Id(prefix, "margin_top")) metrics.content_margin.top = max(0, (int)value);
    else if(id == Id(prefix, "margin_right")) metrics.content_margin.right = max(0, (int)value);
    else if(id == Id(prefix, "margin_bottom")) metrics.content_margin.bottom = max(0, (int)value);
    else if(id == Id(prefix, "focus_enabled")) metrics.focus_enabled = (bool)value;
    else if(id == Id(prefix, "focus_margin")) metrics.focus_margin = max(0, (int)value);
    else if(id == Id(prefix, "focus_alpha")) metrics.focus_alpha = minmax((int)value, 0, 255);
    else if(id == Id(prefix, "focus_color")) metrics.focus_color = (Color)value;
    else if(id == Id(prefix, "shadow_enabled")) metrics.shadow.enabled = (bool)value;
    else if(id == Id(prefix, "shadow_distance")) metrics.shadow.distance = max(0, (int)value);
    else if(id == Id(prefix, "shadow_x")) metrics.shadow.offset_x = (int)value;
    else if(id == Id(prefix, "shadow_y")) metrics.shadow.offset_y = (int)value;
    else if(id == Id(prefix, "shadow_alpha")) metrics.shadow.alpha = minmax((int)value, 0, 255);
    else if(id == Id(prefix, "shadow_color")) metrics.shadow.color = (Color)value;
    else if(id == Id(prefix, "shadow_inset")) metrics.shadow.inset = (bool)value;
    else if(id == Id(prefix, "shadow_mode")) metrics.shadow.mode = ShadowModeFromValue(value);
    else if(id == Id(prefix, "shadow_curve")) metrics.shadow.curve = ShadowCurveFromValue(value, metrics.shadow.curve);
    else if(id == Id(prefix, "highlight_enabled")) metrics.highlight.enabled = (bool)value;
    else if(id == Id(prefix, "highlight_thickness")) metrics.highlight.thickness = max(0, (int)value);
    else if(id == Id(prefix, "highlight_x")) metrics.highlight.offset_x = (int)value;
    else if(id == Id(prefix, "highlight_y")) metrics.highlight.offset_y = (int)value;
    else if(id == Id(prefix, "highlight_alpha")) metrics.highlight.alpha = minmax((int)value, 0, 255);
    else if(id == Id(prefix, "highlight_color")) metrics.highlight.color = (Color)value;
    else if(id == Id(prefix, "highlight_style")) metrics.highlight.style = LineStyleFromValue(value);
    else return false;
    return true;
}

inline bool PaletteMetricsAffectsLayout(const String& prefix, const String& id)
{
    return id == Id(prefix, "radius") || id == Id(prefix, "face_enabled") ||
           id == Id(prefix, "frame_enabled") || id == Id(prefix, "frame_width") ||
           id == Id(prefix, "margin_left") || id == Id(prefix, "margin_top") ||
           id == Id(prefix, "margin_right") || id == Id(prefix, "margin_bottom") ||
           id == Id(prefix, "shadow_enabled") || id == Id(prefix, "shadow_distance") ||
           id == Id(prefix, "shadow_x") || id == Id(prefix, "shadow_y");
}

inline bool EmitPaletteMetrics(String& out,
                               const String& palette_expr,
                               const String& metrics_expr,
                               const String& prefix,
                               const String& id,
                               const Value& value)
{
    int state = StateSuffix(id, prefix, "face");
    if(state >= 0) {
        out << "\t" << palette_expr << ".face[" << StateCode(state)
            << "] = " << FillCode(value) << ";\n";
        return true;
    }
    state = StateSuffix(id, prefix, "frame");
    if(state >= 0) {
        out << "\t" << palette_expr << ".frame[" << StateCode(state)
            << "] = " << EmitValue(value) << ";\n";
        return true;
    }
    state = StateSuffix(id, prefix, "ink");
    if(state >= 0) {
        out << "\t" << palette_expr << ".ink[" << StateCode(state)
            << "] = " << EmitValue(value) << ";\n";
        return true;
    }
    state = StateSuffix(id, prefix, "icon");
    if(state >= 0) {
        out << "\t" << palette_expr << ".icon[" << StateCode(state)
            << "] = " << EmitValue(value) << ";\n";
        return true;
    }

    if(id == Id(prefix, "radius")) out << "\t" << metrics_expr << ".radius = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "high_contrast")) out << "\t" << metrics_expr << ".high_contrast = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "face_enabled")) out << "\t" << metrics_expr << ".face_enabled = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "frame_enabled")) out << "\t" << metrics_expr << ".frame_enabled = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "frame_width")) out << "\t" << metrics_expr << ".frame_width = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "frame_dashed")) out << "\t" << metrics_expr << ".dashed = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "frame_dash_pattern")) out << "\t" << metrics_expr << ".dash_pattern = " << CppString(AsString(value)) << ";\n";
    else if(id == Id(prefix, "margin_left")) out << "\t" << metrics_expr << ".content_margin.left = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "margin_top")) out << "\t" << metrics_expr << ".content_margin.top = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "margin_right")) out << "\t" << metrics_expr << ".content_margin.right = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "margin_bottom")) out << "\t" << metrics_expr << ".content_margin.bottom = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "focus_enabled")) out << "\t" << metrics_expr << ".focus_enabled = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "focus_margin")) out << "\t" << metrics_expr << ".focus_margin = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "focus_alpha")) out << "\t" << metrics_expr << ".focus_alpha = " << minmax((int)value, 0, 255) << ";\n";
    else if(id == Id(prefix, "focus_color")) out << "\t" << metrics_expr << ".focus_color = " << EmitValue(value) << ";\n";
    else if(id == Id(prefix, "shadow_enabled")) out << "\t" << metrics_expr << ".shadow.enabled = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "shadow_distance")) out << "\t" << metrics_expr << ".shadow.distance = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "shadow_x")) out << "\t" << metrics_expr << ".shadow.offset_x = " << (int)value << ";\n";
    else if(id == Id(prefix, "shadow_y")) out << "\t" << metrics_expr << ".shadow.offset_y = " << (int)value << ";\n";
    else if(id == Id(prefix, "shadow_alpha")) out << "\t" << metrics_expr << ".shadow.alpha = " << minmax((int)value, 0, 255) << ";\n";
    else if(id == Id(prefix, "shadow_color")) out << "\t" << metrics_expr << ".shadow.color = " << EmitValue(value) << ";\n";
    else if(id == Id(prefix, "shadow_inset")) out << "\t" << metrics_expr << ".shadow.inset = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "shadow_mode")) out << "\t" << metrics_expr << ".shadow.mode = " << (AsString(value) == "Hard" ? "SHADOW_HARD" : "SHADOW_CURVE") << ";\n";
    else if(id == Id(prefix, "shadow_curve")) {
        const ShadowCurve curve = ShadowCurveFromValue(value, ShadowLinear());
        out << "\t" << metrics_expr << ".shadow.curve = ShadowCurve{" << Format("%.6g", curve.x1)
            << ", " << Format("%.6g", curve.y1) << ", " << Format("%.6g", curve.x2)
            << ", " << Format("%.6g", curve.y2) << "};\n";
    }
    else if(id == Id(prefix, "highlight_enabled")) out << "\t" << metrics_expr << ".highlight.enabled = " << AsString((bool)value) << ";\n";
    else if(id == Id(prefix, "highlight_thickness")) out << "\t" << metrics_expr << ".highlight.thickness = " << max(0, (int)value) << ";\n";
    else if(id == Id(prefix, "highlight_x")) out << "\t" << metrics_expr << ".highlight.offset_x = " << (int)value << ";\n";
    else if(id == Id(prefix, "highlight_y")) out << "\t" << metrics_expr << ".highlight.offset_y = " << (int)value << ";\n";
    else if(id == Id(prefix, "highlight_alpha")) out << "\t" << metrics_expr << ".highlight.alpha = " << minmax((int)value, 0, 255) << ";\n";
    else if(id == Id(prefix, "highlight_color")) out << "\t" << metrics_expr << ".highlight.color = " << EmitValue(value) << ";\n";
    else if(id == Id(prefix, "highlight_style")) out << "\t" << metrics_expr << ".highlight.style = " << (AsString(value) == "Dashed" ? "DASHED" : AsString(value) == "Dotted" ? "DOTTED" : "SOLID") << ";\n";
    else return false;
    return true;
}

inline UiAlign AlignFromValue(const Value& value, UiAlign fallback = UiAlign::CENTER)
{
    const String s = AsString(value);
    if(s == "Left") return UiAlign::LEFT;
    if(s == "Right") return UiAlign::RIGHT;
    if(s == "Top") return UiAlign::TOP;
    if(s == "Bottom") return UiAlign::BOTTOM;
    if(s == "Center") return UiAlign::CENTER;
    return fallback;
}

inline String AlignName(UiAlign value)
{
    if(value == UiAlign::LEFT) return "Left";
    if(value == UiAlign::RIGHT) return "Right";
    if(value == UiAlign::TOP) return "Top";
    if(value == UiAlign::BOTTOM) return "Bottom";
    return "Center";
}

inline String AlignCode(UiAlign value)
{
    if(value == UiAlign::LEFT) return "UiAlign::LEFT";
    if(value == UiAlign::RIGHT) return "UiAlign::RIGHT";
    if(value == UiAlign::TOP) return "UiAlign::TOP";
    if(value == UiAlign::BOTTOM) return "UiAlign::BOTTOM";
    return "UiAlign::CENTER";
}

inline UiIconRenderMode IconModeFromValue(const Value& value)
{
    const String s = AsString(value);
    if(s == "Auto") return UiIconRenderMode::Auto;
    if(s == "PreserveColor") return UiIconRenderMode::PreserveColor;
    return UiIconRenderMode::MonoTint;
}

inline String IconModeName(UiIconRenderMode value)
{
    if(value == UiIconRenderMode::Auto) return "Auto";
    if(value == UiIconRenderMode::PreserveColor) return "PreserveColor";
    return "MonoTint";
}

inline String IconModeCode(UiIconRenderMode value)
{
    if(value == UiIconRenderMode::Auto) return "UiIconRenderMode::Auto";
    if(value == UiIconRenderMode::PreserveColor) return "UiIconRenderMode::PreserveColor";
    return "UiIconRenderMode::MonoTint";
}

inline void AddAlignChoice(UiDesignerControlSpec& spec, const String& id,
                           const String& label, const String& group,
                           UiAlign value, bool cardinal = false)
{
    UiDesignerThemeOverrideSpec& item = Add(
        spec, id, label, group, PropertyEditorKind::Choice, AlignName(value), true);
    if(cardinal) {
        item.Choice("Left", "Left").Choice("Right", "Right")
            .Choice("Top", "Top").Choice("Bottom", "Bottom");
        item.Editor("property.matrix");
        item.editor_variant = "Cardinal4";
    }
    else
        item.Choice("Left", "Left").Choice("Center", "Center").Choice("Right", "Right")
            .Choice("Top", "Top").Choice("Bottom", "Bottom");
}

inline void AddIconMode(UiDesignerControlSpec& spec, const String& id,
                        const String& label, const String& group,
                        UiIconRenderMode value)
{
    Add(spec, id, label, group, PropertyEditorKind::Choice, IconModeName(value))
        .Choice("Auto", "Auto")
        .Choice("MonoTint", "Mono tint")
        .Choice("PreserveColor", "Preserve colour");
}

} // namespace UiDesignerStyledTheme
} // namespace Upp

#endif
