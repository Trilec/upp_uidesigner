#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiBaseEdit.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;

static const char *const kEditStates[] = { "normal", "hot", "pressed", "disabled" };
static const char *const kEditStateLabels[] = { "Normal", "Hot", "Pressed", "Disabled" };

static int EditState(const String& id, const char *prefix)
{
    const String token = String(prefix) + "_";
    if(!id.StartsWith(token)) return -1;
    const String state = id.Mid(token.GetCount());
    if(state == "normal") return ST_NORMAL;
    if(state == "hot") return ST_HOT;
    if(state == "pressed") return ST_PRESSED;
    if(state == "disabled") return ST_DISABLED;
    return -1;
}

static UiAlign EditAlign(const Value& v)
{
    const String q = AsString(v);
    if(q == "Center") return UiAlign::CENTER;
    if(q == "Right") return UiAlign::RIGHT;
    return UiAlign::LEFT;
}

static String EditAlignName(UiAlign v)
{
    return v == UiAlign::CENTER ? "Center" : v == UiAlign::RIGHT ? "Right" : "Left";
}

static UiBaseEdit::Style EditBase(const UiDesignerNode& node)
{
    return UiTheme::ResolveEdit(UiTheme::GetContext(), Role(node.GetProperty("role", "Standard")));
}

static bool IsEditKind(UiDesignerRuntimeKind kind)
{
    return kind == UiDesignerRuntimeKind::UiLineEdit ||
           kind == UiDesignerRuntimeKind::UiIntEdit ||
           kind == UiDesignerRuntimeKind::UiFloatEdit ||
           kind == UiDesignerRuntimeKind::UiPasswordEdit ||
           kind == UiDesignerRuntimeKind::UiMultiEdit ||
           kind == UiDesignerRuntimeKind::UiMaskEdit;
}

static bool IsEditField(const String& id)
{
    static const char *fields[] = {
        "radius", "high_contrast", "text_align", "face_enabled", "frame_enabled",
        "frame_width", "font_face", "font_size", "font_bold", "font_italic",
        "margin_left", "margin_top", "margin_right", "margin_bottom",
        "caret_color", "caret_width", "block_caret", "selection_color",
        "selection_ink", "placeholder_ink", "show_readonly_bg",
        "underline_enabled", "underline_width", "whitespace_color", "tab_char_color",
        "tab_size", "show_tabs", "show_spaces", "show_line_endings",
        "focus_enabled", "focus_margin", "focus_alpha", "focus_color",
        "shadow_enabled", "shadow_distance", "shadow_x", "shadow_y", "shadow_alpha",
        "shadow_color", "shadow_inset", "shadow_mode",
        "highlight_enabled", "highlight_thickness", "highlight_x", "highlight_y",
        "highlight_alpha", "highlight_color", "highlight_style"
    };
    if(EditState(id, "face") >= 0 || EditState(id, "frame") >= 0 ||
       EditState(id, "text") >= 0 || EditState(id, "underline") >= 0)
        return true;
    for(const char *field : fields)
        if(id == field) return true;
    return false;
}

static Value EditFieldValue(const UiBaseEdit::Style& s, const String& id)
{
    int state = EditState(id, "face");
    if(state >= 0) return FillRecipe(s.palette.face[state]).ToValue();
    state = EditState(id, "frame");
    if(state >= 0) return s.palette.frame[state];
    state = EditState(id, "text");
    if(state >= 0) return s.palette.ink[state];
    state = EditState(id, "underline");
    if(state >= 0) return s.underline[state];
    if(id == "radius") return s.metrics.radius;
    if(id == "high_contrast") return s.metrics.high_contrast;
    if(id == "text_align") return EditAlignName(s.text_align);
    if(id == "face_enabled") return s.metrics.face_enabled;
    if(id == "frame_enabled") return s.metrics.frame_enabled;
    if(id == "frame_width") return s.metrics.frame_width;
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_size") return s.font.GetHeight();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    if(id == "margin_left") return s.metrics.content_margin.left;
    if(id == "margin_top") return s.metrics.content_margin.top;
    if(id == "margin_right") return s.metrics.content_margin.right;
    if(id == "margin_bottom") return s.metrics.content_margin.bottom;
    if(id == "caret_color") return s.caret_color;
    if(id == "caret_width") return s.caret_width;
    if(id == "block_caret") return s.block_caret;
    if(id == "selection_color") return s.selection_color;
    if(id == "selection_ink") return s.selection_ink;
    if(id == "placeholder_ink") return s.placeholder_ink;
    if(id == "show_readonly_bg") return s.show_readonly_bg;
    if(id == "underline_enabled") return s.underline_enabled;
    if(id == "underline_width") return s.underline_width;
    if(id == "whitespace_color") return s.whitespace_color;
    if(id == "tab_char_color") return s.tab_char_color;
    if(id == "tab_size") return s.tab_size;
    if(id == "show_tabs") return s.show_tabs;
    if(id == "show_spaces") return s.show_spaces;
    if(id == "show_line_endings") return s.show_line_endings;
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
    if(id == "highlight_enabled") return s.metrics.highlight.enabled;
    if(id == "highlight_thickness") return s.metrics.highlight.thickness;
    if(id == "highlight_x") return s.metrics.highlight.offset_x;
    if(id == "highlight_y") return s.metrics.highlight.offset_y;
    if(id == "highlight_alpha") return s.metrics.highlight.alpha;
    if(id == "highlight_color") return s.metrics.highlight.color;
    if(id == "highlight_style") return s.metrics.highlight.style == DASHED ? "Dashed" : s.metrics.highlight.style == DOTTED ? "Dotted" : "Solid";
    return Value();
}

static void ApplyEditField(UiBaseEdit::Style& s, const String& id, const Value& v)
{
    int state = EditState(id, "face");
    if(state >= 0) { ApplyFill(s.palette.face[state], v); return; }
    state = EditState(id, "frame");
    if(state >= 0) { s.palette.frame[state] = (Color)v; return; }
    state = EditState(id, "text");
    if(state >= 0) { s.palette.ink[state] = (Color)v; return; }
    state = EditState(id, "underline");
    if(state >= 0) { s.underline[state] = (Color)v; return; }
    if(id == "radius") s.metrics.radius = max(0, (int)v);
    else if(id == "high_contrast") s.metrics.high_contrast = (bool)v;
    else if(id == "text_align") s.text_align = EditAlign(v);
    else if(id == "face_enabled") s.metrics.face_enabled = (bool)v;
    else if(id == "frame_enabled") s.metrics.frame_enabled = (bool)v;
    else if(id == "frame_width") s.metrics.frame_width = max(0, (int)v);
    else if(id == "font_face") s.font.FaceName(AsString(v));
    else if(id == "font_size") s.font.Height(max(1, (int)v));
    else if(id == "font_bold") s.font.Bold((bool)v);
    else if(id == "font_italic") s.font.Italic((bool)v);
    else if(id == "margin_left") s.metrics.content_margin.left = max(0, (int)v);
    else if(id == "margin_top") s.metrics.content_margin.top = max(0, (int)v);
    else if(id == "margin_right") s.metrics.content_margin.right = max(0, (int)v);
    else if(id == "margin_bottom") s.metrics.content_margin.bottom = max(0, (int)v);
    else if(id == "caret_color") s.caret_color = (Color)v;
    else if(id == "caret_width") s.caret_width = max(1, (int)v);
    else if(id == "block_caret") s.block_caret = (bool)v;
    else if(id == "selection_color") s.selection_color = (Color)v;
    else if(id == "selection_ink") s.selection_ink = (Color)v;
    else if(id == "placeholder_ink") s.placeholder_ink = (Color)v;
    else if(id == "show_readonly_bg") s.show_readonly_bg = (bool)v;
    else if(id == "underline_enabled") s.underline_enabled = (bool)v;
    else if(id == "underline_width") s.underline_width = max(1, (int)v);
    else if(id == "whitespace_color") s.whitespace_color = (Color)v;
    else if(id == "tab_char_color") s.tab_char_color = (Color)v;
    else if(id == "tab_size") s.tab_size = max(1, (int)v);
    else if(id == "show_tabs") s.show_tabs = (bool)v;
    else if(id == "show_spaces") s.show_spaces = (bool)v;
    else if(id == "show_line_endings") s.show_line_endings = (bool)v;
    else if(id == "focus_enabled") s.metrics.focus_enabled = (bool)v;
    else if(id == "focus_margin") s.metrics.focus_margin = max(0, (int)v);
    else if(id == "focus_alpha") s.metrics.focus_alpha = minmax((int)v, 0, 255);
    else if(id == "focus_color") s.metrics.focus_color = (Color)v;
    else if(id == "shadow_enabled") s.metrics.shadow.enabled = (bool)v;
    else if(id == "shadow_distance") s.metrics.shadow.distance = max(0, (int)v);
    else if(id == "shadow_x") s.metrics.shadow.offset_x = (int)v;
    else if(id == "shadow_y") s.metrics.shadow.offset_y = (int)v;
    else if(id == "shadow_alpha") s.metrics.shadow.alpha = minmax((int)v, 0, 255);
    else if(id == "shadow_color") s.metrics.shadow.color = (Color)v;
    else if(id == "shadow_inset") s.metrics.shadow.inset = (bool)v;
    else if(id == "shadow_mode") s.metrics.shadow.mode = AsString(v) == "Hard" ? SHADOW_HARD : SHADOW_CURVE;
    else if(id == "highlight_enabled") s.metrics.highlight.enabled = (bool)v;
    else if(id == "highlight_thickness") s.metrics.highlight.thickness = max(0, (int)v);
    else if(id == "highlight_x") s.metrics.highlight.offset_x = (int)v;
    else if(id == "highlight_y") s.metrics.highlight.offset_y = (int)v;
    else if(id == "highlight_alpha") s.metrics.highlight.alpha = minmax((int)v, 0, 255);
    else if(id == "highlight_color") s.metrics.highlight.color = (Color)v;
    else if(id == "highlight_style") { const String q = AsString(v); s.metrics.highlight.style = q == "Dashed" ? DASHED : q == "Dotted" ? DOTTED : SOLID; }
}

static void AddEditOverrides(UiDesignerControlSpec& spec)
{
    UiDesignerNode dummy;
    const UiBaseEdit::Style s = UiTheme::ResolveEdit(UiTheme::GetContext(), UiRole::Standard);
    AddInt(spec, "radius", "Radius", "General", s.metrics.radius, 0, 96);
    Add(spec, "high_contrast", "High contrast", "General", PropertyEditorKind::Boolean, s.metrics.high_contrast);
    Add(spec, "text_align", "Text Align", "General", PropertyEditorKind::Choice, EditAlignName(s.text_align), true).Choice("Left", "Left").Choice("Center", "Center").Choice("Right", "Right");
    Add(spec, "face_enabled", "Enabled", "Face", PropertyEditorKind::Boolean, s.metrics.face_enabled);
    for(int i = 0; i < 4; i++) Add(spec, "face_" + String(kEditStates[i]), kEditStateLabels[i], "Face", PropertyEditorKind::FillRecipe, FillRecipe(s.palette.face[i]).ToValue());
    Add(spec, "frame_enabled", "Enabled", "Frame", PropertyEditorKind::Boolean, s.metrics.frame_enabled, true);
    AddInt(spec, "frame_width", "Width", "Frame", s.metrics.frame_width, 0, 24, true);
    for(int i = 0; i < 4; i++) Add(spec, "frame_" + String(kEditStates[i]), kEditStateLabels[i], "Frame", PropertyEditorKind::Color, s.palette.frame[i]);
    for(int i = 0; i < 4; i++) Add(spec, "text_" + String(kEditStates[i]), kEditStateLabels[i], "Ink", PropertyEditorKind::Color, s.palette.ink[i]);
    Add(spec, "font_face", "Font face", "Typography", PropertyEditorKind::Text, s.font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "font_size", "Font height", "Typography", max(1, s.font.GetHeight()), 6, 96, true);
    Add(spec, "font_bold", "Bold", "Typography", PropertyEditorKind::Boolean, s.font.IsBold(), true);
    Add(spec, "font_italic", "Italic", "Typography", PropertyEditorKind::Boolean, s.font.IsItalic(), true);
    AddInt(spec, "margin_left", "Left", "Content Margin", s.metrics.content_margin.left, 0, 80, true);
    AddInt(spec, "margin_top", "Top", "Content Margin", s.metrics.content_margin.top, 0, 80, true);
    AddInt(spec, "margin_right", "Right", "Content Margin", s.metrics.content_margin.right, 0, 80, true);
    AddInt(spec, "margin_bottom", "Bottom", "Content Margin", s.metrics.content_margin.bottom, 0, 80, true);
    Add(spec, "caret_color", "Caret Colour", "Editing", PropertyEditorKind::Color, s.caret_color);
    AddInt(spec, "caret_width", "Caret Width", "Editing", s.caret_width, 1, 12);
    Add(spec, "block_caret", "Block Caret", "Editing", PropertyEditorKind::Boolean, s.block_caret);
    Add(spec, "selection_color", "Selection Face", "Editing", PropertyEditorKind::Color, s.selection_color);
    Add(spec, "selection_ink", "Selection Ink", "Editing", PropertyEditorKind::Color, s.selection_ink);
    Add(spec, "placeholder_ink", "Placeholder Ink", "Editing", PropertyEditorKind::Color, s.placeholder_ink);
    Add(spec, "show_readonly_bg", "Readonly Background", "Editing", PropertyEditorKind::Boolean, s.show_readonly_bg);
    Add(spec, "underline_enabled", "Enabled", "Underline", PropertyEditorKind::Boolean, s.underline_enabled);
    AddInt(spec, "underline_width", "Width", "Underline", s.underline_width, 1, 12);
    for(int i = 0; i < 4; i++) Add(spec, "underline_" + String(kEditStates[i]), kEditStateLabels[i], "Underline", PropertyEditorKind::Color, s.underline[i]);
    Add(spec, "whitespace_color", "Whitespace Colour", "Whitespace", PropertyEditorKind::Color, s.whitespace_color);
    Add(spec, "tab_char_color", "Tab Colour", "Whitespace", PropertyEditorKind::Color, s.tab_char_color);
    AddInt(spec, "tab_size", "Tab Size", "Whitespace", s.tab_size, 1, 16, true);
    Add(spec, "show_tabs", "Show Tabs", "Whitespace", PropertyEditorKind::Boolean, s.show_tabs);
    Add(spec, "show_spaces", "Show Spaces", "Whitespace", PropertyEditorKind::Boolean, s.show_spaces);
    Add(spec, "show_line_endings", "Show Line Endings", "Whitespace", PropertyEditorKind::Boolean, s.show_line_endings);
    Add(spec, "focus_enabled", "Enabled", "Focus", PropertyEditorKind::Boolean, s.metrics.focus_enabled);
    AddInt(spec, "focus_margin", "Margin", "Focus", s.metrics.focus_margin, 0, 20);
    AddInt(spec, "focus_alpha", "Alpha", "Focus", s.metrics.focus_alpha, 0, 255);
    Add(spec, "focus_color", "Colour", "Focus", PropertyEditorKind::Color, s.metrics.focus_color);
    Add(spec, "shadow_enabled", "Enabled", "Shadow", PropertyEditorKind::Boolean, s.metrics.shadow.enabled, true);
    AddInt(spec, "shadow_distance", "Distance", "Shadow", s.metrics.shadow.distance, 0, 64, true);
    AddInt(spec, "shadow_x", "Offset X", "Shadow", s.metrics.shadow.offset_x, -64, 64, true);
    AddInt(spec, "shadow_y", "Offset Y", "Shadow", s.metrics.shadow.offset_y, -64, 64, true);
    AddInt(spec, "shadow_alpha", "Alpha", "Shadow", s.metrics.shadow.alpha, 0, 255);
    Add(spec, "shadow_color", "Colour", "Shadow", PropertyEditorKind::Color, s.metrics.shadow.color);
    Add(spec, "shadow_inset", "Inset", "Shadow", PropertyEditorKind::Boolean, s.metrics.shadow.inset, true);
    Add(spec, "shadow_mode", "Mode", "Shadow", PropertyEditorKind::Choice, s.metrics.shadow.mode == SHADOW_HARD ? "Hard" : "Curve").Choice("Hard", "Hard").Choice("Curve", "Curve");
    Add(spec, "highlight_enabled", "Enabled", "Highlight", PropertyEditorKind::Boolean, s.metrics.highlight.enabled);
    AddInt(spec, "highlight_thickness", "Thickness", "Highlight", s.metrics.highlight.thickness, 0, 24);
    AddInt(spec, "highlight_x", "Offset X", "Highlight", s.metrics.highlight.offset_x, -32, 32);
    AddInt(spec, "highlight_y", "Offset Y", "Highlight", s.metrics.highlight.offset_y, -32, 32);
    AddInt(spec, "highlight_alpha", "Alpha", "Highlight", s.metrics.highlight.alpha, 0, 255);
    Add(spec, "highlight_color", "Colour", "Highlight", PropertyEditorKind::Color, s.metrics.highlight.color);
    Add(spec, "highlight_style", "Style", "Highlight", PropertyEditorKind::Choice, "Solid").Choice("Solid", "Solid").Choice("Dashed", "Dashed").Choice("Dotted", "Dotted");
}

static bool EditAffectsLayout(const String& id)
{
    return id == "frame_enabled" || id == "frame_width" || id == "text_align" ||
           id.StartsWith("font_") || id.StartsWith("margin_") || id == "tab_size" ||
           id == "shadow_enabled" || id == "shadow_distance" || id == "shadow_x" ||
           id == "shadow_y" || id == "shadow_inset";
}

static Value ResolveEditFace(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                             const String& id, const UiDesignerTransientOverlay* overlay,
                             const UiBaseEdit::Style& base)
{
    const int state = EditState(id, "face");
    if(state < 0) return Value();
    const Value inherited = FillRecipe(base.palette.face[state]).ToValue();
    for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
        if(property.adapter_field_id != id) continue;
        if(!HasValue(node, overlay, property.id)) return inherited;
        return ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property));
    }
    return inherited;
}

static void EmitEditField(String& out, const String& var, const String& id, const Value& v)
{
    int state = EditState(id, "face");
    if(state >= 0) { out << "\t" << var << ".palette.face[" << StateCode(state) << "] = " << FillCode(v) << ";\n"; return; }
    state = EditState(id, "frame");
    if(state >= 0) { out << "\t" << var << ".palette.frame[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    state = EditState(id, "text");
    if(state >= 0) { out << "\t" << var << ".palette.ink[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    state = EditState(id, "underline");
    if(state >= 0) { out << "\t" << var << ".underline[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    if(id == "radius") out << "\t" << var << ".metrics.radius = " << max(0, (int)v) << ";\n";
    else if(id == "high_contrast") out << "\t" << var << ".metrics.high_contrast = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "text_align") out << "\t" << var << ".text_align = " << (AsString(v) == "Center" ? "UiAlign::CENTER" : AsString(v) == "Right" ? "UiAlign::RIGHT" : "UiAlign::LEFT") << ";\n";
    else if(id == "face_enabled") out << "\t" << var << ".metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_enabled") out << "\t" << var << ".metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_width") out << "\t" << var << ".metrics.frame_width = " << max(0, (int)v) << ";\n";
    else if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "font_size") out << "\t" << var << ".font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id.StartsWith("margin_")) out << "\t" << var << ".metrics.content_margin." << id.Mid(7) << " = " << max(0, (int)v) << ";\n";
    else if(id == "caret_color" || id == "selection_color" || id == "selection_ink" || id == "placeholder_ink" || id == "whitespace_color" || id == "tab_char_color") out << "\t" << var << "." << id << " = " << EmitValue(v) << ";\n";
    else if(id == "caret_width" || id == "underline_width" || id == "tab_size") out << "\t" << var << "." << id << " = " << (int)v << ";\n";
    else if(id == "block_caret" || id == "show_readonly_bg" || id == "underline_enabled" || id == "show_tabs" || id == "show_spaces" || id == "show_line_endings") out << "\t" << var << "." << id << " = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "focus_enabled") out << "\t" << var << ".metrics.focus_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "focus_margin") out << "\t" << var << ".metrics.focus_margin = " << max(0, (int)v) << ";\n";
    else if(id == "focus_alpha") out << "\t" << var << ".metrics.focus_alpha = " << minmax((int)v, 0, 255) << ";\n";
    else if(id == "focus_color") out << "\t" << var << ".metrics.focus_color = " << EmitValue(v) << ";\n";
    else if(id == "shadow_enabled") out << "\t" << var << ".metrics.shadow.enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "shadow_distance") out << "\t" << var << ".metrics.shadow.distance = " << max(0, (int)v) << ";\n";
    else if(id == "shadow_x") out << "\t" << var << ".metrics.shadow.offset_x = " << (int)v << ";\n";
    else if(id == "shadow_y") out << "\t" << var << ".metrics.shadow.offset_y = " << (int)v << ";\n";
    else if(id == "shadow_alpha") out << "\t" << var << ".metrics.shadow.alpha = " << minmax((int)v, 0, 255) << ";\n";
    else if(id == "shadow_color") out << "\t" << var << ".metrics.shadow.color = " << EmitValue(v) << ";\n";
    else if(id == "shadow_inset") out << "\t" << var << ".metrics.shadow.inset = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "shadow_mode") out << "\t" << var << ".metrics.shadow.mode = " << (AsString(v) == "Hard" ? "SHADOW_HARD" : "SHADOW_CURVE") << ";\n";
    else if(id == "highlight_enabled") out << "\t" << var << ".metrics.highlight.enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "highlight_thickness") out << "\t" << var << ".metrics.highlight.thickness = " << max(0, (int)v) << ";\n";
    else if(id == "highlight_x") out << "\t" << var << ".metrics.highlight.offset_x = " << (int)v << ";\n";
    else if(id == "highlight_y") out << "\t" << var << ".metrics.highlight.offset_y = " << (int)v << ";\n";
    else if(id == "highlight_alpha") out << "\t" << var << ".metrics.highlight.alpha = " << minmax((int)v, 0, 255) << ";\n";
    else if(id == "highlight_color") out << "\t" << var << ".metrics.highlight.color = " << EmitValue(v) << ";\n";
    else if(id == "highlight_style") out << "\t" << var << ".metrics.highlight.style = " << (AsString(v) == "Dashed" ? "DASHED" : AsString(v) == "Dotted" ? "DOTTED" : "SOLID") << ";\n";
}

class NormalizedEditAdapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "edit"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return IsEditKind(kind); }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override { AddEditOverrides(spec); }
    bool HasField(const String& id) const override { return IsEditField(id); }
    bool FieldAffectsLayout(const String& id) const override { return EditAffectsLayout(id); }

    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        UiBaseEdit::Style style = EditBase(node);
        if(EditState(id, "face") >= 0) return ResolveEditFace(node, spec, id, overlay, style);
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            if(!HasValue(node, overlay, property.id)) continue;
            ApplyEditField(style, property.adapter_field_id,
                           ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property)));
        }
        return EditFieldValue(style, id);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiBaseEdit *edit = dynamic_cast<UiBaseEdit *>(&ctrl);
        if(!edit) return;
        UiBaseEdit::Style style = EditBase(node);
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            if(!HasValue(node, overlay, property.id)) continue;
            authored = true;
            ApplyEditField(style, property.adapter_field_id,
                           ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property)));
        }
        if(authored || Role(node.GetProperty("role", "Standard")) != UiRole::Standard)
            edit->SetCustomStyle(style);
        else
            edit->ClearCustomStyle();
    }

    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides)
            authored |= node.theme_overrides.Find(property.id) >= 0;
        const Value role = node.GetProperty("role", "Standard");
        if(!authored && Role(role) == UiRole::Standard) return;
        const String var = member + "_style";
        out << "\tUiBaseEdit::Style " << var << " = UiTheme::ResolveEdit(UiTheme::GetContext(), " << RoleExpr(role) << ");\n";
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(property.id);
            if(q >= 0) EmitEditField(out, var, property.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

} // namespace

const UiDesignerThemeAdapter& UiDesignerEditThemeAdapterInstance()
{
    static NormalizedEditAdapter adapter;
    return adapter;
}

} // namespace Upp
