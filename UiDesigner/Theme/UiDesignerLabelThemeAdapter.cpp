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
    String role = AsString(value);
    if(role == "Subtle") return UiRole::Subtle;
    if(role == "Accent") return UiRole::Accent;
    if(role == "Alert") return UiRole::Alert;
    return UiRole::Standard;
}

static String LabelRoleExpr(const Value& value)
{
    String role = AsString(value);
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
                                    const String& id, const Value& canonical)
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
    if(IsNull(value)) return "Value()";
    if(value.Is<String>()) return CppStringLabel(AsString(value));
    if(value.Is<bool>()) return (bool)value ? "true" : "false";
    if(value.Is<int>() || value.Is<int64_t>()) return AsString(value);
    if(value.Is<double>()) return Format("%.12g", (double)value);
    if(value.Is<Color>()) {
        Color c = value;
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
    const String p = String(prefix) + ".";
    if(!id.StartsWith(p))
        return -1;
    String state = id.Mid(p.GetCount());
    if(state == "normal") return ST_NORMAL;
    if(state == "hot") return ST_HOT;
    if(state == "pressed") return ST_PRESSED;
    if(state == "disabled") return ST_DISABLED;
    return -1;
}

static const char *StateCode(int state)
{
    static const char *codes[] = { "ST_NORMAL", "ST_HOT", "ST_PRESSED", "ST_DISABLED" };
    return state >= 0 && state < 4 ? codes[state] : "ST_NORMAL";
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
                                                 int value, int mn, int mx,
                                                 bool layout = false)
{
    UiDesignerThemeOverrideSpec& item = AddLabelOverride(
        spec, id, label, group, PropertyEditorKind::NumericInt, value, layout);
    item.Range(mn, mx, 1);
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

    AddLabelInt(spec, "radius", "Radius", "General", base.metrics.radius, 0, 96);
    AddLabelOverride(spec, "transparent", "Transparent", "General",
                     PropertyEditorKind::Boolean, base.transparent);
    AddLabelOverride(spec, "high_contrast", "High contrast", "General",
                     PropertyEditorKind::Boolean, base.metrics.high_contrast);

    AddLabelOverride(spec, "face_enabled", "Enabled", "Face",
                     PropertyEditorKind::Boolean, base.metrics.face_enabled);
    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "face." + String(states[i]), labels[i], "Face",
                         PropertyEditorKind::Color,
                         IsNull(FillColor(base.palette.face[i])) ? White() : FillColor(base.palette.face[i]));

    AddLabelOverride(spec, "frame_enabled", "Enabled", "Frame",
                     PropertyEditorKind::Boolean, base.metrics.frame_enabled, true);
    AddLabelInt(spec, "frame_width", "Width", "Frame", base.metrics.frame_width, 0, 24, true);
    AddLabelOverride(spec, "dashed", "Dashed", "Frame",
                     PropertyEditorKind::Boolean, base.metrics.dashed);
    AddLabelOverride(spec, "dash_pattern", "Dash pattern", "Frame",
                     PropertyEditorKind::Text, base.metrics.dash_pattern);
    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "frame." + String(states[i]), labels[i], "Frame",
                         PropertyEditorKind::Color,
                         IsNull(base.palette.frame[i]) ? Color(180, 186, 196) : base.palette.frame[i]);

    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "ink." + String(states[i]), labels[i], "Ink",
                         PropertyEditorKind::Color,
                         IsNull(base.palette.ink[i]) ? SColorText() : base.palette.ink[i]);
    for(int i = 0; i < 4; i++)
        AddLabelOverride(spec, "icon." + String(states[i]), labels[i], "Icon",
                         PropertyEditorKind::Color,
                         IsNull(base.palette.icon[i]) ? SColorText() : base.palette.icon[i]);

    AddLabelOverride(spec, "font_face", "Font face", "Typography",
                     PropertyEditorKind::Text, base.font.GetFaceName(), true).Editor("property.font");
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
    AddLabelInt(spec, "focus_margin", "Margin", "Focus", base.metrics.focus_margin, 0, 20);
    AddLabelInt(spec, "focus_alpha", "Alpha", "Focus", base.metrics.focus_alpha, 0, 255);
    AddLabelOverride(spec, "focus_color", "Colour", "Focus",
                     PropertyEditorKind::Color,
                     IsNull(base.metrics.focus_color) ? Color(37, 99, 235) : base.metrics.focus_color);

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
        .Choice("Hard", "Hard").Choice("Curve", "Curve");
    UiDesignerThemeOverrideSpec& curve = AddLabelOverride(
        spec, "shadow_curve", "Falloff curve", "Shadow", PropertyEditorKind::Curve,
        MakeShadowCurveValue(base.metrics.shadow.curve));
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
        .Choice("Solid", "Solid").Choice("Dashed", "Dashed").Choice("Dotted", "Dotted");
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
        "shadow_alpha", "shadow_color", "shadow_inset", "shadow_mode", "shadow_curve",
        "highlight_enabled", "highlight_thickness", "highlight_x", "highlight_y",
        "highlight_alpha", "highlight_color", "highlight_style"
    };
    if(StateIndex(id, "face") >= 0 || StateIndex(id, "frame") >= 0 ||
       StateIndex(id, "ink") >= 0 || StateIndex(id, "icon") >= 0)
        return true;
    for(const char *field : fields)
        if(id == field)
            return true;
    return false;
}

static void ApplyLabelField(UiLabel::Style& s, const String& id, const Value& value)
{
    int state = StateIndex(id, "face");
    if(state >= 0) { s.palette.face[state] = UiFill::Solid((Color)value); return; }
    state = StateIndex(id, "frame");
    if(state >= 0) { s.palette.frame[state] = (Color)value; return; }
    state = StateIndex(id, "ink");
    if(state >= 0) { s.palette.ink[state] = (Color)value; return; }
    state = StateIndex(id, "icon");
    if(state >= 0) { s.palette.icon[state] = (Color)value; return; }

    if(id == "radius") s.metrics.radius = max(0, (int)value);
    else if(id == "transparent") s.transparent = (bool)value;
    else if(id == "high_contrast") s.metrics.high_contrast = (bool)value;
    else if(id == "face_enabled") s.metrics.face_enabled = (bool)value;
    else if(id == "frame_enabled") s.metrics.frame_enabled = (bool)value;
    else if(id == "frame_width") s.metrics.frame_width = max(0, (int)value);
    else if(id == "dashed") s.metrics.dashed = (bool)value;
    else if(id == "dash_pattern") s.metrics.dash_pattern = AsString(value);
    else if(id == "font_face") s.font.FaceName(AsString(value));
    else if(id == "font_height") s.font.Height(max(1, (int)value));
    else if(id == "font_bold") s.font.Bold((bool)value);
    else if(id == "font_italic") s.font.Italic((bool)value);
    else if(id == "underline") s.underline = (bool)value;
    else if(id == "underline_width") s.underline_width = max(0, (int)value);
    else if(id == "underline_offset") s.underline_offset = (int)value;
    else if(id == "nowrap") s.nowrap = (bool)value;
    else if(id == "margin_left") s.metrics.content_margin.left = max(0, (int)value);
    else if(id == "margin_top") s.metrics.content_margin.top = max(0, (int)value);
    else if(id == "margin_right") s.metrics.content_margin.right = max(0, (int)value);
    else if(id == "margin_bottom") s.metrics.content_margin.bottom = max(0, (int)value);
    else if(id == "focus_enabled") s.metrics.focus_enabled = (bool)value;
    else if(id == "focus_margin") s.metrics.focus_margin = max(0, (int)value);
    else if(id == "focus_alpha") s.metrics.focus_alpha = minmax((int)value, 0, 255);
    else if(id == "focus_color") s.metrics.focus_color = (Color)value;
    else if(id == "shadow_enabled") s.metrics.shadow.enabled = (bool)value;
    else if(id == "shadow_distance") s.metrics.shadow.distance = max(0, (int)value);
    else if(id == "shadow_x") s.metrics.shadow.offset_x = (int)value;
    else if(id == "shadow_y") s.metrics.shadow.offset_y = (int)value;
    else if(id == "shadow_alpha") s.metrics.shadow.alpha = minmax((int)value, 0, 255);
    else if(id == "shadow_color") s.metrics.shadow.color = (Color)value;
    else if(id == "shadow_inset") s.metrics.shadow.inset = (bool)value;
    else if(id == "shadow_mode") s.metrics.shadow.mode = AsString(value) == "Hard" ? SHADOW_HARD : SHADOW_CURVE;
    else if(id == "shadow_curve") {
        ValueArray v = value;
        if(v.GetCount() >= 4)
            s.metrics.shadow.curve = ShadowCurve{ (double)v[0], (double)v[1], (double)v[2], (double)v[3] };
    }
    else if(id == "highlight_enabled") s.metrics.highlight.enabled = (bool)value;
    else if(id == "highlight_thickness") s.metrics.highlight.thickness = max(0, (int)value);
    else if(id == "highlight_x") s.metrics.highlight.offset_x = (int)value;
    else if(id == "highlight_y") s.metrics.highlight.offset_y = (int)value;
    else if(id == "highlight_alpha") s.metrics.highlight.alpha = minmax((int)value, 0, 255);
    else if(id == "highlight_color") s.metrics.highlight.color = (Color)value;
    else if(id == "highlight_style") {
        String style = AsString(value);
        s.metrics.highlight.style = style == "Dashed" ? DASHED : style == "Dotted" ? DOTTED : SOLID;
    }
}

static Value LabelFieldValue(const UiLabel::Style& s, const String& id)
{
    int state = StateIndex(id, "face");
    if(state >= 0) return FillColor(s.palette.face[state]);
    state = StateIndex(id, "frame"); if(state >= 0) return s.palette.frame[state];
    state = StateIndex(id, "ink"); if(state >= 0) return s.palette.ink[state];
    state = StateIndex(id, "icon"); if(state >= 0) return s.palette.icon[state];

    if(id == "radius") return s.metrics.radius;
    if(id == "transparent") return s.transparent;
    if(id == "high_contrast") return s.metrics.high_contrast;
    if(id == "face_enabled") return s.metrics.face_enabled;
    if(id == "frame_enabled") return s.metrics.frame_enabled;
    if(id == "frame_width") return s.metrics.frame_width;
    if(id == "dashed") return s.metrics.dashed;
    if(id == "dash_pattern") return s.metrics.dash_pattern;
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_height") return s.font.GetHeight();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    if(id == "underline") return s.underline;
    if(id == "underline_width") return s.underline_width;
    if(id == "underline_offset") return s.underline_offset;
    if(id == "nowrap") return s.nowrap;
    if(id == "margin_left") return s.metrics.content_margin.left;
    if(id == "margin_top") return s.metrics.content_margin.top;
    if(id == "margin_right") return s.metrics.content_margin.right;
    if(id == "margin_bottom") return s.metrics.content_margin.bottom;
    if(id == "focus_enabled") return s.metrics.focus_enabled;
    if(id == "focus_margin") return s.metrics.focus_margin;
    if(id == "focus_alpha") return s.metrics.focus_alpha;
    if(id == "focus_color") return s.metrics.focus_color;
    if(id == "shadow_enabled") return s.metrics.shadow.enabled;
    if(id == "shadow_distance") return s.metrics.shadow.distance;
    if(id == "shadow_x") return s.metrics.shadow.offset_x;
    if(id == "shadow_y") return s.metrics.shadow.offset_y;
    if(id == "shadow_alpha") return s.metrics.shadow.alpha;
    if(id == "shadow_color") return s.metrics.shadow.color;
    if(id == "shadow_inset") return s.metrics.shadow.inset;
    if(id == "shadow_mode") return s.metrics.shadow.mode == SHADOW_HARD ? "Hard" : "Curve";
    if(id == "shadow_curve") return MakeShadowCurveValue(s.metrics.shadow.curve);
    if(id == "highlight_enabled") return s.metrics.highlight.enabled;
    if(id == "highlight_thickness") return s.metrics.highlight.thickness;
    if(id == "highlight_x") return s.metrics.highlight.offset_x;
    if(id == "highlight_y") return s.metrics.highlight.offset_y;
    if(id == "highlight_alpha") return s.metrics.highlight.alpha;
    if(id == "highlight_color") return s.metrics.highlight.color;
    if(id == "highlight_style") return s.metrics.highlight.style == DASHED ? "Dashed" : s.metrics.highlight.style == DOTTED ? "Dotted" : "Solid";
    return Value();
}

static bool LabelFieldAffectsLayout(const String& id)
{
    return id == "frame_enabled" || id == "frame_width" ||
           id == "font_face" || id == "font_height" || id == "font_bold" ||
           id == "font_italic" || id == "nowrap" || id.StartsWith("margin_") ||
           id == "shadow_enabled" || id == "shadow_distance" ||
           id == "shadow_x" || id == "shadow_y" || id == "shadow_inset";
}

static void EmitLabelField(String& out, const String& var,
                           const String& id, const Value& value)
{
    int state = StateIndex(id, "face");
    if(state >= 0) { out << "\t" << var << ".palette.face[" << StateCode(state) << "] = UiFill::Solid(" << EmitLabelValue(value) << ");\n"; return; }
    state = StateIndex(id, "frame");
    if(state >= 0) { out << "\t" << var << ".palette.frame[" << StateCode(state) << "] = " << EmitLabelValue(value) << ";\n"; return; }
    state = StateIndex(id, "ink");
    if(state >= 0) { out << "\t" << var << ".palette.ink[" << StateCode(state) << "] = " << EmitLabelValue(value) << ";\n"; return; }
    state = StateIndex(id, "icon");
    if(state >= 0) { out << "\t" << var << ".palette.icon[" << StateCode(state) << "] = " << EmitLabelValue(value) << ";\n"; return; }

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
        out << "\t" << var << ".metrics.content_margin." << side << " = " << max(0, (int)value) << ";\n";
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

    bool HasField(const String& id) const override { return IsLabelField(id); }
    bool FieldAffectsLayout(const String& id) const override { return LabelFieldAffectsLayout(id); }

    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiLabel::Style style = UiTheme::ResolveLabel(LabelRole(node.GetProperty("role", "Standard")));
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            int q = node.theme_overrides.Find(p.id);
            if(q < 0 && !HasLabelThemeValue(node, overlay, p.id))
                continue;
            Value canonical = q >= 0 ? node.theme_overrides.GetValue(q) : p.default_value;
            ApplyLabelField(style, p.adapter_field_id,
                            ResolveLabelThemeValue(node, overlay, p.id, canonical));
        }
        return LabelFieldValue(style, id);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiLabel *label = dynamic_cast<UiLabel *>(&ctrl);
        if(!label)
            return;
        UiLabel::Style style = UiTheme::ResolveLabel(LabelRole(node.GetProperty("role", "Standard")));
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            int q = node.theme_overrides.Find(p.id);
            if(q < 0 && !HasLabelThemeValue(node, overlay, p.id))
                continue;
            authored = true;
            Value canonical = q >= 0 ? node.theme_overrides.GetValue(q) : p.default_value;
            ApplyLabelField(style, p.adapter_field_id,
                            ResolveLabelThemeValue(node, overlay, p.id, canonical));
        }
        if(authored || LabelRole(node.GetProperty("role", "Standard")) != UiRole::Standard)
            label->SetCustomStyle(style);
        else
            label->ClearCustomStyle();
    }

    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
            authored |= node.theme_overrides.Find(p.id) >= 0;
        const Value role = node.GetProperty("role", "Standard");
        if(!authored && LabelRole(role) == UiRole::Standard)
            return;

        const String var = member + "_style";
        out << "\tUiLabel::Style " << var << " = UiTheme::ResolveLabel("
            << LabelRoleExpr(role) << ");\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            int q = node.theme_overrides.Find(p.id);
            if(q >= 0)
                EmitLabelField(out, var, p.adapter_field_id,
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
