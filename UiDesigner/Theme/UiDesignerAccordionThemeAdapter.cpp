#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiAccordion.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;

static const char *const kStates[] = { "normal", "hot", "pressed", "disabled" };
static const char *const kLabels[] = { "Normal", "Hot", "Pressed", "Disabled" };

static int State(const String& id, const char *prefix)
{
    const String token = String(prefix) + "_";
    if(!id.StartsWith(token)) return -1;
    const String q = id.Mid(token.GetCount());
    if(q == "normal") return ST_NORMAL;
    if(q == "hot") return ST_HOT;
    if(q == "pressed") return ST_PRESSED;
    if(q == "disabled") return ST_DISABLED;
    return -1;
}

static UiAlign Side(const Value& v)
{
    const String q = AsString(v);
    return q == "Left" ? UiAlign::LEFT : q == "Top" ? UiAlign::TOP
         : q == "Bottom" ? UiAlign::BOTTOM : q == "Center" ? UiAlign::CENTER
         : UiAlign::RIGHT;
}

static String SideName(UiAlign a)
{
    if(a == UiAlign::LEFT) return "Left";
    if(a == UiAlign::TOP) return "Top";
    if(a == UiAlign::BOTTOM) return "Bottom";
    if(a == UiAlign::CENTER) return "Center";
    return "Right";
}

static UiSpan Span(const Value& v)
{
    const String q = AsString(v);
    return q == "Small" ? SMALL : q == "Medium" ? MEDIUM : q == "Large" ? LARGE : NONE;
}

static String SpanName(UiSpan v)
{
    return v == SMALL ? "Small" : v == MEDIUM ? "Medium" : v == LARGE ? "Large" : "None";
}

static String SpanCode(UiSpan v)
{
    return v == SMALL ? "SMALL" : v == MEDIUM ? "MEDIUM" : v == LARGE ? "LARGE" : "NONE";
}

static UiLineStyle LineStyle(const Value& v)
{
    const String q = AsString(v);
    return q == "Dashed" ? DASHED : q == "Dotted" ? DOTTED : SOLID;
}

static String LineStyleName(UiLineStyle v)
{
    return v == DASHED ? "Dashed" : v == DOTTED ? "Dotted" : "Solid";
}

static String LineStyleCode(UiLineStyle v)
{
    return v == DASHED ? "DASHED" : v == DOTTED ? "DOTTED" : "SOLID";
}

static UiAccordion::Style Base()
{
    UiAccordion::Style style = UiAccordion::StyleDefault();
    UiPanel::Style panel = UiTheme::ResolvePanel(UiPanelRole::Surface);
    style.palette = panel.palette;
    style.metrics.radius = max(DPI(8), panel.metrics.radius);
    style.transparent = true;
    style.metrics.frame_width = 0;
    style.metrics.frame_enabled = false;
    style.metrics.face_enabled = false;
    style.metrics.shadow.enabled = false;

    style.body_style = UiTheme::ResolvePanel(UiPanelRole::Surface);
    style.body_style.transparent = true;
    style.body_style.metrics.face_enabled = false;
    style.body_style.metrics.frame_enabled = false;
    style.body_style.metrics.frame_width = 0;
    style.body_style.metrics.radius = 0;
    style.body_style.metrics.focus_enabled = false;
    style.body_style.metrics.content_margin = Rect(0, 0, 0, 0);
    style.body_style.metrics.shadow.enabled = false;

    style.header_style = UiTheme::ResolveTitleCard(UiRole::Accent);
    style.header_style.metrics.content_margin = Rect(DPI(10), DPI(6), DPI(10), DPI(6));
    style.header_style.hover_enabled = false;
    style.header_style.metrics.focus_enabled = false;
    style.header_style.title_line = false;
    style.header_style.card_line = true;
    style.header_style.media_tint_mono = true;
    style.header_style.title_font = SansSerifZ(11).Bold();
    style.header_style.subtitle_font = SansSerifZ(8);
    return style;
}

static bool IsField(const String& id)
{
    static const char *fields[] = {
        "face_enabled", "frame_enabled", "frame_width", "radius", "transparent",
        "ink_normal", "ink_hot", "ink_pressed", "ink_disabled",
        "shadow_enabled", "shadow_distance", "shadow_x", "shadow_y", "shadow_alpha",
        "shadow_color", "shadow_inset", "shadow_mode",
        "highlight_enabled", "highlight_thickness", "highlight_x", "highlight_y",
        "highlight_alpha", "highlight_color", "highlight_style",
        "style_header_height", "style_item_spacing", "style_header_body_gap",
        "style_body_min_height", "style_single_open", "style_enforce_one",
        "style_show_chevron", "style_chevron_side", "style_chevron_scale",
        "style_chevron_size", "style_chevron_gap", "style_show_drag_handle",
        "style_drag_side", "style_drag_size", "style_drag_gap",
        "style_unified_section_frame", "style_unified_section_radius",
        "style_unified_section_frame_width", "body_line_extent", "body_line_style",
        "body_line_thickness", "body_line_color", "style_animation_enabled",
        "style_anim_open_ms", "style_anim_close_ms",
        "header_face_enabled", "header_frame_enabled", "header_frame_width", "header_radius",
        "header_title_color", "header_subtitle_color", "header_copy_color",
        "header_title_font_face", "header_title_font_height", "header_title_font_bold",
        "header_subtitle_font_face", "header_subtitle_font_height", "header_subtitle_font_bold",
        "header_copy_font_face", "header_copy_font_height", "header_copy_font_bold",
        "header_margin_left", "header_margin_top", "header_margin_right", "header_margin_bottom",
        "body_transparent", "body_face_enabled", "body_frame_enabled", "body_frame_width",
        "body_radius", "body_margin_left", "body_margin_top", "body_margin_right", "body_margin_bottom"
    };
    if(State(id, "face") >= 0 || State(id, "frame") >= 0 ||
       DotState(id, "header_face") >= 0 || DotState(id, "header_frame") >= 0 ||
       DotState(id, "header_ink") >= 0 || DotState(id, "body_face") >= 0 ||
       DotState(id, "body_frame") >= 0)
        return true;
    for(const char *f : fields)
        if(id == f) return true;
    return false;
}

static Value ValueOf(const UiAccordion::Style& s, const String& id)
{
    int st = State(id, "face");
    if(st >= 0) return FillRecipe(s.palette.face[st]).ToValue();
    st = State(id, "frame");
    if(st >= 0) return s.palette.frame[st];
    st = State(id, "ink");
    if(st >= 0) return s.palette.ink[st];
    st = DotState(id, "header_face");
    if(st >= 0) return FillRecipe(s.header_style.palette.face[st]).ToValue();
    st = DotState(id, "header_frame");
    if(st >= 0) return s.header_style.palette.frame[st];
    st = DotState(id, "header_ink");
    if(st >= 0) return s.header_style.palette.ink[st];
    st = DotState(id, "body_face");
    if(st >= 0) return FillRecipe(s.body_style.palette.face[st]).ToValue();
    st = DotState(id, "body_frame");
    if(st >= 0) return s.body_style.palette.frame[st];

    if(id == "face_enabled") return s.metrics.face_enabled;
    if(id == "frame_enabled") return s.metrics.frame_enabled;
    if(id == "frame_width") return s.metrics.frame_width;
    if(id == "radius") return s.metrics.radius;
    if(id == "transparent") return s.transparent;
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
    if(id == "highlight_style") return LineStyleName(s.metrics.highlight.style);
    if(id == "style_header_height") return s.header_height;
    if(id == "style_item_spacing") return s.item_spacing;
    if(id == "style_header_body_gap") return s.header_body_gap;
    if(id == "style_body_min_height") return s.body_min_height;
    if(id == "style_single_open") return s.single_open;
    if(id == "style_enforce_one") return s.enforce_one;
    if(id == "style_show_chevron") return s.show_chevron;
    if(id == "style_chevron_side") return SideName(s.chevron_side);
    if(id == "style_chevron_scale") return s.chevron_scale;
    if(id == "style_chevron_size") return s.chevron_size;
    if(id == "style_chevron_gap") return s.chevron_gap;
    if(id == "style_show_drag_handle") return s.show_drag_handle;
    if(id == "style_drag_side") return SideName(s.drag_side);
    if(id == "style_drag_size") return s.drag_size;
    if(id == "style_drag_gap") return s.drag_gap;
    if(id == "style_unified_section_frame") return s.unified_section_frame;
    if(id == "style_unified_section_radius") return s.unified_section_radius;
    if(id == "style_unified_section_frame_width") return s.unified_section_frame_width;
    if(id == "body_line_extent") return SpanName(s.body_line_extent);
    if(id == "body_line_style") return LineStyleName(s.body_line_style);
    if(id == "body_line_thickness") return s.body_line_thickness;
    if(id == "body_line_color") return s.body_line_color;
    if(id == "style_animation_enabled") return s.animation_enabled;
    if(id == "style_anim_open_ms") return s.anim_open_ms;
    if(id == "style_anim_close_ms") return s.anim_close_ms;

    if(id == "header_face_enabled") return s.header_style.metrics.face_enabled;
    if(id == "header_frame_enabled") return s.header_style.metrics.frame_enabled;
    if(id == "header_frame_width") return s.header_style.metrics.frame_width;
    if(id == "header_radius") return s.header_style.metrics.radius;
    if(id == "header_title_color") return s.header_style.title_color;
    if(id == "header_subtitle_color") return s.header_style.subtitle_color;
    if(id == "header_copy_color") return s.header_style.copy_color;
    if(id == "header_title_font_face") return s.header_style.title_font.GetFaceName();
    if(id == "header_title_font_height") return s.header_style.title_font.GetHeight();
    if(id == "header_title_font_bold") return s.header_style.title_font.IsBold();
    if(id == "header_subtitle_font_face") return s.header_style.subtitle_font.GetFaceName();
    if(id == "header_subtitle_font_height") return s.header_style.subtitle_font.GetHeight();
    if(id == "header_subtitle_font_bold") return s.header_style.subtitle_font.IsBold();
    if(id == "header_copy_font_face") return s.header_style.copy_font.GetFaceName();
    if(id == "header_copy_font_height") return s.header_style.copy_font.GetHeight();
    if(id == "header_copy_font_bold") return s.header_style.copy_font.IsBold();
    if(id == "header_margin_left") return s.header_style.metrics.content_margin.left;
    if(id == "header_margin_top") return s.header_style.metrics.content_margin.top;
    if(id == "header_margin_right") return s.header_style.metrics.content_margin.right;
    if(id == "header_margin_bottom") return s.header_style.metrics.content_margin.bottom;

    if(id == "body_transparent") return s.body_style.transparent;
    if(id == "body_face_enabled") return s.body_style.metrics.face_enabled;
    if(id == "body_frame_enabled") return s.body_style.metrics.frame_enabled;
    if(id == "body_frame_width") return s.body_style.metrics.frame_width;
    if(id == "body_radius") return s.body_style.metrics.radius;
    if(id == "body_margin_left") return s.body_style.metrics.content_margin.left;
    if(id == "body_margin_top") return s.body_style.metrics.content_margin.top;
    if(id == "body_margin_right") return s.body_style.metrics.content_margin.right;
    if(id == "body_margin_bottom") return s.body_style.metrics.content_margin.bottom;
    return Value();
}

static void Apply(UiAccordion::Style& s, const String& id, const Value& v)
{
    int st = State(id, "face");
    if(st >= 0) { ApplyFill(s.palette.face[st], v); return; }
    st = State(id, "frame");
    if(st >= 0) { s.palette.frame[st] = (Color)v; return; }
    st = State(id, "ink");
    if(st >= 0) { s.palette.ink[st] = (Color)v; return; }
    st = DotState(id, "header_face");
    if(st >= 0) { ApplyFill(s.header_style.palette.face[st], v); return; }
    st = DotState(id, "header_frame");
    if(st >= 0) { s.header_style.palette.frame[st] = (Color)v; return; }
    st = DotState(id, "header_ink");
    if(st >= 0) { s.header_style.palette.ink[st] = (Color)v; return; }
    st = DotState(id, "body_face");
    if(st >= 0) { ApplyFill(s.body_style.palette.face[st], v); return; }
    st = DotState(id, "body_frame");
    if(st >= 0) { s.body_style.palette.frame[st] = (Color)v; return; }

    if(id == "face_enabled") s.metrics.face_enabled = (bool)v;
    else if(id == "frame_enabled") s.metrics.frame_enabled = (bool)v;
    else if(id == "frame_width") s.metrics.frame_width = max(0, (int)v);
    else if(id == "radius") s.metrics.radius = max(0, (int)v);
    else if(id == "transparent") s.transparent = (bool)v;
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
    else if(id == "highlight_style") s.metrics.highlight.style = LineStyle(v);
    else if(id == "style_header_height") s.header_height = max(18, (int)v);
    else if(id == "style_item_spacing") s.item_spacing = max(0, (int)v);
    else if(id == "style_header_body_gap") s.header_body_gap = max(0, (int)v);
    else if(id == "style_body_min_height") s.body_min_height = max(0, (int)v);
    else if(id == "style_single_open") s.single_open = (bool)v;
    else if(id == "style_enforce_one") s.enforce_one = (bool)v;
    else if(id == "style_show_chevron") s.show_chevron = (bool)v;
    else if(id == "style_chevron_side") s.chevron_side = Side(v);
    else if(id == "style_chevron_scale") s.chevron_scale = (bool)v;
    else if(id == "style_chevron_size") s.chevron_size = max(0, (int)v);
    else if(id == "style_chevron_gap") s.chevron_gap = max(0, (int)v);
    else if(id == "style_show_drag_handle") s.show_drag_handle = (bool)v;
    else if(id == "style_drag_side") s.drag_side = Side(v);
    else if(id == "style_drag_size") s.drag_size = max(1, (int)v);
    else if(id == "style_drag_gap") s.drag_gap = max(0, (int)v);
    else if(id == "style_unified_section_frame") s.unified_section_frame = (bool)v;
    else if(id == "style_unified_section_radius") s.unified_section_radius = max(0, (int)v);
    else if(id == "style_unified_section_frame_width") s.unified_section_frame_width = max(0, (int)v);
    else if(id == "body_line_extent") s.body_line_extent = Span(v);
    else if(id == "body_line_style") s.body_line_style = LineStyle(v);
    else if(id == "body_line_thickness") s.body_line_thickness = max(1, (int)v);
    else if(id == "body_line_color") s.body_line_color = (Color)v;
    else if(id == "style_animation_enabled") s.animation_enabled = (bool)v;
    else if(id == "style_anim_open_ms") s.anim_open_ms = max(0, (int)v);
    else if(id == "style_anim_close_ms") s.anim_close_ms = max(0, (int)v);

    else if(id == "header_face_enabled") s.header_style.metrics.face_enabled = (bool)v;
    else if(id == "header_frame_enabled") s.header_style.metrics.frame_enabled = (bool)v;
    else if(id == "header_frame_width") s.header_style.metrics.frame_width = max(0, (int)v);
    else if(id == "header_radius") s.header_style.metrics.radius = max(0, (int)v);
    else if(id == "header_title_color") s.header_style.title_color = (Color)v;
    else if(id == "header_subtitle_color") s.header_style.subtitle_color = (Color)v;
    else if(id == "header_copy_color") s.header_style.copy_color = (Color)v;
    else if(id == "header_title_font_face") s.header_style.title_font.FaceName(AsString(v));
    else if(id == "header_title_font_height") s.header_style.title_font.Height(max(1, (int)v));
    else if(id == "header_title_font_bold") s.header_style.title_font.Bold((bool)v);
    else if(id == "header_subtitle_font_face") s.header_style.subtitle_font.FaceName(AsString(v));
    else if(id == "header_subtitle_font_height") s.header_style.subtitle_font.Height(max(1, (int)v));
    else if(id == "header_subtitle_font_bold") s.header_style.subtitle_font.Bold((bool)v);
    else if(id == "header_copy_font_face") s.header_style.copy_font.FaceName(AsString(v));
    else if(id == "header_copy_font_height") s.header_style.copy_font.Height(max(1, (int)v));
    else if(id == "header_copy_font_bold") s.header_style.copy_font.Bold((bool)v);
    else if(id == "header_margin_left") s.header_style.metrics.content_margin.left = max(0, (int)v);
    else if(id == "header_margin_top") s.header_style.metrics.content_margin.top = max(0, (int)v);
    else if(id == "header_margin_right") s.header_style.metrics.content_margin.right = max(0, (int)v);
    else if(id == "header_margin_bottom") s.header_style.metrics.content_margin.bottom = max(0, (int)v);

    else if(id == "body_transparent") s.body_style.transparent = (bool)v;
    else if(id == "body_face_enabled") s.body_style.metrics.face_enabled = (bool)v;
    else if(id == "body_frame_enabled") s.body_style.metrics.frame_enabled = (bool)v;
    else if(id == "body_frame_width") s.body_style.metrics.frame_width = max(0, (int)v);
    else if(id == "body_radius") s.body_style.metrics.radius = max(0, (int)v);
    else if(id == "body_margin_left") s.body_style.metrics.content_margin.left = max(0, (int)v);
    else if(id == "body_margin_top") s.body_style.metrics.content_margin.top = max(0, (int)v);
    else if(id == "body_margin_right") s.body_style.metrics.content_margin.right = max(0, (int)v);
    else if(id == "body_margin_bottom") s.body_style.metrics.content_margin.bottom = max(0, (int)v);
}

static void AddOverrides(UiDesignerControlSpec& spec)
{
    const UiAccordion::Style s = Base();
    Add(spec, "transparent", "Transparent", "General", PropertyEditorKind::Boolean, s.transparent);
    AddInt(spec, "radius", "Radius", "General", s.metrics.radius, 0, 96);
    Add(spec, "face_enabled", "Enabled", "Face", PropertyEditorKind::Boolean, s.metrics.face_enabled);
    Add(spec, "face_normal", "Normal", "Face", PropertyEditorKind::FillRecipe, FillRecipe(s.palette.face[ST_NORMAL]).ToValue());
    for(int i = 1; i < 4; i++) Add(spec, "face_" + String(kStates[i]), kLabels[i], "Face", PropertyEditorKind::FillRecipe, FillRecipe(s.palette.face[i]).ToValue());
    Add(spec, "frame_enabled", "Enabled", "Frame", PropertyEditorKind::Boolean, s.metrics.frame_enabled, true);
    AddInt(spec, "frame_width", "Width", "Frame", s.metrics.frame_width, 0, 24, true);
    for(int i = 0; i < 4; i++) Add(spec, "frame_" + String(kStates[i]), kLabels[i], "Frame", PropertyEditorKind::Color, s.palette.frame[i]);
    Add(spec, "ink_normal", "Normal", "Ink", PropertyEditorKind::Color, s.palette.ink[ST_NORMAL]);
    Add(spec, "ink_hot", "Hot", "Ink", PropertyEditorKind::Color, s.palette.ink[ST_HOT]);
    Add(spec, "ink_pressed", "Pressed", "Ink", PropertyEditorKind::Color, s.palette.ink[ST_PRESSED]);
    Add(spec, "ink_disabled", "Disabled", "Ink", PropertyEditorKind::Color, s.palette.ink[ST_DISABLED]);
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
    Add(spec, "highlight_style", "Style", "Highlight", PropertyEditorKind::Choice, LineStyleName(s.metrics.highlight.style)).Choice("Solid", "Solid").Choice("Dashed", "Dashed").Choice("Dotted", "Dotted");

    AddInt(spec, "style_header_height", "Header Height", "Layout", s.header_height, 18, 160, true);
    AddInt(spec, "style_item_spacing", "Item Spacing", "Layout", s.item_spacing, 0, 64, true);
    AddInt(spec, "style_header_body_gap", "Header Body Gap", "Layout", s.header_body_gap, 0, 64, true);
    AddInt(spec, "style_body_min_height", "Body Min Height", "Layout", s.body_min_height, 0, 800, true);
    Add(spec, "style_unified_section_frame", "Unified Frame", "Section", PropertyEditorKind::Boolean, s.unified_section_frame, true);
    AddInt(spec, "style_unified_section_radius", "Radius", "Section", s.unified_section_radius, 0, 96);
    AddInt(spec, "style_unified_section_frame_width", "Frame Width", "Section", s.unified_section_frame_width, 0, 24, true);

    Add(spec, "header_face_enabled", "Enabled", "Header/Face", PropertyEditorKind::Boolean, s.header_style.metrics.face_enabled);
    for(int i = 0; i < 4; i++) Add(spec, "header_face." + String(kStates[i]), kLabels[i], "Header/Face", PropertyEditorKind::FillRecipe, FillRecipe(s.header_style.palette.face[i]).ToValue());
    Add(spec, "header_frame_enabled", "Enabled", "Header/Frame", PropertyEditorKind::Boolean, s.header_style.metrics.frame_enabled, true);
    AddInt(spec, "header_frame_width", "Width", "Header/Frame", s.header_style.metrics.frame_width, 0, 24, true);
    AddInt(spec, "header_radius", "Radius", "Header/Frame", s.header_style.metrics.radius, 0, 96);
    for(int i = 0; i < 4; i++) Add(spec, "header_frame." + String(kStates[i]), kLabels[i], "Header/Frame", PropertyEditorKind::Color, s.header_style.palette.frame[i]);
    for(int i = 0; i < 4; i++) Add(spec, "header_ink." + String(kStates[i]), kLabels[i], "Header/Ink", PropertyEditorKind::Color, s.header_style.palette.ink[i]);
    Add(spec, "header_title_color", "Title", "Header/Ink", PropertyEditorKind::Color, s.header_style.title_color);
    Add(spec, "header_subtitle_color", "Subtitle", "Header/Ink", PropertyEditorKind::Color, s.header_style.subtitle_color);
    Add(spec, "header_copy_color", "Copy", "Header/Ink", PropertyEditorKind::Color, s.header_style.copy_color);
    Add(spec, "header_title_font_face", "Title Font", "Header/Typography", PropertyEditorKind::Text, s.header_style.title_font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "header_title_font_height", "Title Height", "Header/Typography", max(1, s.header_style.title_font.GetHeight()), 6, 96, true);
    Add(spec, "header_title_font_bold", "Title Bold", "Header/Typography", PropertyEditorKind::Boolean, s.header_style.title_font.IsBold(), true);
    Add(spec, "header_subtitle_font_face", "Subtitle Font", "Header/Typography", PropertyEditorKind::Text, s.header_style.subtitle_font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "header_subtitle_font_height", "Subtitle Height", "Header/Typography", max(1, s.header_style.subtitle_font.GetHeight()), 6, 96, true);
    Add(spec, "header_subtitle_font_bold", "Subtitle Bold", "Header/Typography", PropertyEditorKind::Boolean, s.header_style.subtitle_font.IsBold(), true);
    Add(spec, "header_copy_font_face", "Copy Font", "Header/Typography", PropertyEditorKind::Text, s.header_style.copy_font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "header_copy_font_height", "Copy Height", "Header/Typography", max(1, s.header_style.copy_font.GetHeight()), 6, 96, true);
    Add(spec, "header_copy_font_bold", "Copy Bold", "Header/Typography", PropertyEditorKind::Boolean, s.header_style.copy_font.IsBold(), true);
    AddInt(spec, "header_margin_left", "Left", "Header/Content Margin", s.header_style.metrics.content_margin.left, 0, 80, true);
    AddInt(spec, "header_margin_top", "Top", "Header/Content Margin", s.header_style.metrics.content_margin.top, 0, 80, true);
    AddInt(spec, "header_margin_right", "Right", "Header/Content Margin", s.header_style.metrics.content_margin.right, 0, 80, true);
    AddInt(spec, "header_margin_bottom", "Bottom", "Header/Content Margin", s.header_style.metrics.content_margin.bottom, 0, 80, true);

    Add(spec, "style_show_chevron", "Show", "Header/Chevron", PropertyEditorKind::Boolean, s.show_chevron, true);
    Add(spec, "style_chevron_side", "Side", "Header/Chevron", PropertyEditorKind::Choice, SideName(s.chevron_side), true).Choice("Left", "Left").Choice("Right", "Right");
    Add(spec, "style_chevron_scale", "Scale", "Header/Chevron", PropertyEditorKind::Boolean, s.chevron_scale, true);
    AddInt(spec, "style_chevron_size", "Size", "Header/Chevron", s.chevron_size, 0, 64, true);
    AddInt(spec, "style_chevron_gap", "Gap", "Header/Chevron", s.chevron_gap, 0, 40, true);
    Add(spec, "style_show_drag_handle", "Show Handle", "Header/Drag", PropertyEditorKind::Boolean, s.show_drag_handle, true);
    Add(spec, "style_drag_side", "Side", "Header/Drag", PropertyEditorKind::Choice, SideName(s.drag_side), true).Choice("Left", "Left").Choice("Right", "Right");
    AddInt(spec, "style_drag_size", "Size", "Header/Drag", s.drag_size, 1, 64, true);
    AddInt(spec, "style_drag_gap", "Gap", "Header/Drag", s.drag_gap, 0, 40, true);

    Add(spec, "body_transparent", "Transparent", "Body/Face", PropertyEditorKind::Boolean, s.body_style.transparent);
    Add(spec, "body_face_enabled", "Enabled", "Body/Face", PropertyEditorKind::Boolean, s.body_style.metrics.face_enabled);
    for(int i = 0; i < 4; i++) Add(spec, "body_face." + String(kStates[i]), kLabels[i], "Body/Face", PropertyEditorKind::FillRecipe, FillRecipe(s.body_style.palette.face[i]).ToValue());
    Add(spec, "body_frame_enabled", "Enabled", "Body/Frame", PropertyEditorKind::Boolean, s.body_style.metrics.frame_enabled, true);
    AddInt(spec, "body_frame_width", "Width", "Body/Frame", s.body_style.metrics.frame_width, 0, 24, true);
    AddInt(spec, "body_radius", "Radius", "Body/Frame", s.body_style.metrics.radius, 0, 96);
    for(int i = 0; i < 4; i++) Add(spec, "body_frame." + String(kStates[i]), kLabels[i], "Body/Frame", PropertyEditorKind::Color, s.body_style.palette.frame[i]);
    AddInt(spec, "body_margin_left", "Left", "Body/Content Margin", s.body_style.metrics.content_margin.left, 0, 80, true);
    AddInt(spec, "body_margin_top", "Top", "Body/Content Margin", s.body_style.metrics.content_margin.top, 0, 80, true);
    AddInt(spec, "body_margin_right", "Right", "Body/Content Margin", s.body_style.metrics.content_margin.right, 0, 80, true);
    AddInt(spec, "body_margin_bottom", "Bottom", "Body/Content Margin", s.body_style.metrics.content_margin.bottom, 0, 80, true);
    Add(spec, "body_line_extent", "Extent", "Body/Line", PropertyEditorKind::Choice, SpanName(s.body_line_extent)).Choice("None", "None").Choice("Small", "Small").Choice("Medium", "Medium").Choice("Large", "Large");
    Add(spec, "body_line_style", "Style", "Body/Line", PropertyEditorKind::Choice, LineStyleName(s.body_line_style)).Choice("Solid", "Solid").Choice("Dashed", "Dashed").Choice("Dotted", "Dotted");
    AddInt(spec, "body_line_thickness", "Thickness", "Body/Line", s.body_line_thickness, 1, 12);
    Add(spec, "body_line_color", "Colour", "Body/Line", PropertyEditorKind::Color, s.body_line_color);

    Add(spec, "style_single_open", "Single Open", "Behaviour", PropertyEditorKind::Boolean, s.single_open, true);
    Add(spec, "style_enforce_one", "Enforce One", "Behaviour", PropertyEditorKind::Boolean, s.enforce_one, true);
    Add(spec, "style_animation_enabled", "Enabled", "Animation", PropertyEditorKind::Boolean, s.animation_enabled);
    AddInt(spec, "style_anim_open_ms", "Open ms", "Animation", s.anim_open_ms, 0, 2000);
    AddInt(spec, "style_anim_close_ms", "Close ms", "Animation", s.anim_close_ms, 0, 2000);
}

static bool AffectsLayout(const String& id)
{
    return id.StartsWith("style_header_") || id.StartsWith("style_item_") ||
           id.StartsWith("style_body_min_") || id.StartsWith("style_chevron_") ||
           id.StartsWith("style_drag_") || id.StartsWith("style_unified_") ||
           id.StartsWith("header_") || id.StartsWith("body_") ||
           id == "frame_enabled" || id == "frame_width" || id.StartsWith("shadow_") ||
           id == "style_single_open" || id == "style_enforce_one";
}

static Value ResolveFace(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                         const String& id, const UiDesignerTransientOverlay* overlay,
                         const UiAccordion::Style& base)
{
    for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
        if(p.adapter_field_id != id) continue;
        return NormalizeFillValue(ResolveValue(node, overlay, p.id, AuthoredOrDefault(node, p)));
    }
    int st = State(id, "face");
    if(st >= 0) return FillRecipe(base.palette.face[st]).ToValue();
    st = DotState(id, "header_face");
    if(st >= 0) return FillRecipe(base.header_style.palette.face[st]).ToValue();
    st = DotState(id, "body_face");
    if(st >= 0) return FillRecipe(base.body_style.palette.face[st]).ToValue();
    return Value();
}

static void Emit(String& out, const String& var, const String& id, const Value& v)
{
    int st = State(id, "face");
    if(st >= 0) { out << "\t" << var << ".palette.face[" << StateCode(st) << "] = " << FillCode(v) << ";\n"; return; }
    st = State(id, "frame");
    if(st >= 0) { out << "\t" << var << ".palette.frame[" << StateCode(st) << "] = " << EmitValue(v) << ";\n"; return; }
    st = State(id, "ink");
    if(st >= 0) { out << "\t" << var << ".palette.ink[" << StateCode(st) << "] = " << EmitValue(v) << ";\n"; return; }
    st = DotState(id, "header_face");
    if(st >= 0) { out << "\t" << var << ".header_style.palette.face[" << StateCode(st) << "] = " << FillCode(v) << ";\n"; return; }
    st = DotState(id, "header_frame");
    if(st >= 0) { out << "\t" << var << ".header_style.palette.frame[" << StateCode(st) << "] = " << EmitValue(v) << ";\n"; return; }
    st = DotState(id, "header_ink");
    if(st >= 0) { out << "\t" << var << ".header_style.palette.ink[" << StateCode(st) << "] = " << EmitValue(v) << ";\n"; return; }
    st = DotState(id, "body_face");
    if(st >= 0) { out << "\t" << var << ".body_style.palette.face[" << StateCode(st) << "] = " << FillCode(v) << ";\n"; return; }
    st = DotState(id, "body_frame");
    if(st >= 0) { out << "\t" << var << ".body_style.palette.frame[" << StateCode(st) << "] = " << EmitValue(v) << ";\n"; return; }

    if(id == "face_enabled") out << "\t" << var << ".metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_enabled") out << "\t" << var << ".metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_width") out << "\t" << var << ".metrics.frame_width = " << (int)v << ";\n";
    else if(id == "radius") out << "\t" << var << ".metrics.radius = " << (int)v << ";\n";
    else if(id == "transparent") out << "\t" << var << ".transparent = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "shadow_enabled") out << "\t" << var << ".metrics.shadow.enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "shadow_distance") out << "\t" << var << ".metrics.shadow.distance = " << (int)v << ";\n";
    else if(id == "shadow_x") out << "\t" << var << ".metrics.shadow.offset_x = " << (int)v << ";\n";
    else if(id == "shadow_y") out << "\t" << var << ".metrics.shadow.offset_y = " << (int)v << ";\n";
    else if(id == "shadow_alpha") out << "\t" << var << ".metrics.shadow.alpha = " << (int)v << ";\n";
    else if(id == "shadow_color") out << "\t" << var << ".metrics.shadow.color = " << EmitValue(v) << ";\n";
    else if(id == "shadow_inset") out << "\t" << var << ".metrics.shadow.inset = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "shadow_mode") out << "\t" << var << ".metrics.shadow.mode = " << (AsString(v) == "Hard" ? "SHADOW_HARD" : "SHADOW_CURVE") << ";\n";
    else if(id == "highlight_enabled") out << "\t" << var << ".metrics.highlight.enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "highlight_thickness") out << "\t" << var << ".metrics.highlight.thickness = " << (int)v << ";\n";
    else if(id == "highlight_x") out << "\t" << var << ".metrics.highlight.offset_x = " << (int)v << ";\n";
    else if(id == "highlight_y") out << "\t" << var << ".metrics.highlight.offset_y = " << (int)v << ";\n";
    else if(id == "highlight_alpha") out << "\t" << var << ".metrics.highlight.alpha = " << (int)v << ";\n";
    else if(id == "highlight_color") out << "\t" << var << ".metrics.highlight.color = " << EmitValue(v) << ";\n";
    else if(id == "highlight_style") out << "\t" << var << ".metrics.highlight.style = " << LineStyleCode(LineStyle(v)) << ";\n";
    else if(id == "style_header_height") out << "\t" << var << ".header_height = " << (int)v << ";\n";
    else if(id == "style_item_spacing") out << "\t" << var << ".item_spacing = " << (int)v << ";\n";
    else if(id == "style_header_body_gap") out << "\t" << var << ".header_body_gap = " << (int)v << ";\n";
    else if(id == "style_body_min_height") out << "\t" << var << ".body_min_height = " << (int)v << ";\n";
    else if(id == "style_single_open") out << "\t" << var << ".single_open = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_enforce_one") out << "\t" << var << ".enforce_one = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_show_chevron") out << "\t" << var << ".show_chevron = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_chevron_side") out << "\t" << var << ".chevron_side = " << (AsString(v) == "Left" ? "UiAlign::LEFT" : "UiAlign::RIGHT") << ";\n";
    else if(id == "style_chevron_scale") out << "\t" << var << ".chevron_scale = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_chevron_size") out << "\t" << var << ".chevron_size = " << (int)v << ";\n";
    else if(id == "style_chevron_gap") out << "\t" << var << ".chevron_gap = " << (int)v << ";\n";
    else if(id == "style_show_drag_handle") out << "\t" << var << ".show_drag_handle = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_drag_side") out << "\t" << var << ".drag_side = " << (AsString(v) == "Left" ? "UiAlign::LEFT" : "UiAlign::RIGHT") << ";\n";
    else if(id == "style_drag_size") out << "\t" << var << ".drag_size = " << (int)v << ";\n";
    else if(id == "style_drag_gap") out << "\t" << var << ".drag_gap = " << (int)v << ";\n";
    else if(id == "style_unified_section_frame") out << "\t" << var << ".unified_section_frame = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_unified_section_radius") out << "\t" << var << ".unified_section_radius = " << (int)v << ";\n";
    else if(id == "style_unified_section_frame_width") out << "\t" << var << ".unified_section_frame_width = " << (int)v << ";\n";
    else if(id == "body_line_extent") out << "\t" << var << ".body_line_extent = " << SpanCode(Span(v)) << ";\n";
    else if(id == "body_line_style") out << "\t" << var << ".body_line_style = " << LineStyleCode(LineStyle(v)) << ";\n";
    else if(id == "body_line_thickness") out << "\t" << var << ".body_line_thickness = " << (int)v << ";\n";
    else if(id == "body_line_color") out << "\t" << var << ".body_line_color = " << EmitValue(v) << ";\n";
    else if(id == "style_animation_enabled") out << "\t" << var << ".animation_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "style_anim_open_ms") out << "\t" << var << ".anim_open_ms = " << (int)v << ";\n";
    else if(id == "style_anim_close_ms") out << "\t" << var << ".anim_close_ms = " << (int)v << ";\n";
    else if(id == "header_face_enabled") out << "\t" << var << ".header_style.metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "header_frame_enabled") out << "\t" << var << ".header_style.metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "header_frame_width") out << "\t" << var << ".header_style.metrics.frame_width = " << (int)v << ";\n";
    else if(id == "header_radius") out << "\t" << var << ".header_style.metrics.radius = " << (int)v << ";\n";
    else if(id == "header_title_color") out << "\t" << var << ".header_style.title_color = " << EmitValue(v) << ";\n";
    else if(id == "header_subtitle_color") out << "\t" << var << ".header_style.subtitle_color = " << EmitValue(v) << ";\n";
    else if(id == "header_copy_color") out << "\t" << var << ".header_style.copy_color = " << EmitValue(v) << ";\n";
    else if(id == "header_title_font_face") out << "\t" << var << ".header_style.title_font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "header_title_font_height") out << "\t" << var << ".header_style.title_font.Height(" << (int)v << ");\n";
    else if(id == "header_title_font_bold") out << "\t" << var << ".header_style.title_font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id == "header_subtitle_font_face") out << "\t" << var << ".header_style.subtitle_font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "header_subtitle_font_height") out << "\t" << var << ".header_style.subtitle_font.Height(" << (int)v << ");\n";
    else if(id == "header_subtitle_font_bold") out << "\t" << var << ".header_style.subtitle_font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id == "header_copy_font_face") out << "\t" << var << ".header_style.copy_font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "header_copy_font_height") out << "\t" << var << ".header_style.copy_font.Height(" << (int)v << ");\n";
    else if(id == "header_copy_font_bold") out << "\t" << var << ".header_style.copy_font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id.StartsWith("header_margin_")) out << "\t" << var << ".header_style.metrics.content_margin." << id.Mid(14) << " = " << (int)v << ";\n";
    else if(id == "body_transparent") out << "\t" << var << ".body_style.transparent = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "body_face_enabled") out << "\t" << var << ".body_style.metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "body_frame_enabled") out << "\t" << var << ".body_style.metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "body_frame_width") out << "\t" << var << ".body_style.metrics.frame_width = " << (int)v << ";\n";
    else if(id == "body_radius") out << "\t" << var << ".body_style.metrics.radius = " << (int)v << ";\n";
    else if(id.StartsWith("body_margin_")) out << "\t" << var << ".body_style.metrics.content_margin." << id.Mid(12) << " = " << (int)v << ";\n";
}

class Adapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "accordion"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == UiDesignerRuntimeKind::UiAccordion; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override { AddOverrides(spec); }
    bool HasField(const String& id) const override { return IsField(id); }
    bool FieldAffectsLayout(const String& id) const override { return AffectsLayout(id); }

    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        UiAccordion::Style style = Base();
        if(State(id, "face") >= 0 || DotState(id, "header_face") >= 0 || DotState(id, "body_face") >= 0)
            return ResolveFace(node, spec, id, overlay, style);
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            if(!HasValue(node, overlay, p.id)) continue;
            Apply(style, p.adapter_field_id,
                  ResolveValue(node, overlay, p.id, AuthoredOrDefault(node, p)));
        }
        return ValueOf(style, id);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiAccordion *accordion = dynamic_cast<UiAccordion *>(&ctrl);
        if(!accordion) return;
        UiAccordion::Style style = Base();
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            if(!HasValue(node, overlay, p.id)) continue;
            authored = true;
            Apply(style, p.adapter_field_id,
                  ResolveValue(node, overlay, p.id, AuthoredOrDefault(node, p)));
        }
        if(authored) accordion->SetCustomStyle(style); else accordion->ClearCustomStyle();
    }

    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
            authored |= node.theme_overrides.Find(p.id) >= 0;
        if(!authored) return;
        const String var = member + "_style";
        out << "\tUiAccordion::Style " << var << " = UiAccordion::StyleDefault();\n";
        out << "\t{ UiPanel::Style panel = UiTheme::ResolvePanel(UiPanelRole::Surface);\n";
        out << "\t  " << var << ".palette = panel.palette;\n";
        out << "\t  " << var << ".metrics.radius = max(DPI(8), panel.metrics.radius);\n";
        out << "\t  " << var << ".transparent = true;\n";
        out << "\t  " << var << ".metrics.frame_width = 0; " << var << ".metrics.frame_enabled = false; " << var << ".metrics.face_enabled = false; " << var << ".metrics.shadow.enabled = false;\n";
        out << "\t  " << var << ".body_style = UiTheme::ResolvePanel(UiPanelRole::Surface);\n";
        out << "\t  " << var << ".body_style.transparent = true; " << var << ".body_style.metrics.face_enabled = false; " << var << ".body_style.metrics.frame_enabled = false; " << var << ".body_style.metrics.frame_width = 0; " << var << ".body_style.metrics.radius = 0; " << var << ".body_style.metrics.focus_enabled = false; " << var << ".body_style.metrics.content_margin = Rect(0, 0, 0, 0); " << var << ".body_style.metrics.shadow.enabled = false;\n";
        out << "\t  " << var << ".header_style = UiTheme::ResolveTitleCard(UiRole::Accent);\n";
        out << "\t  " << var << ".header_style.metrics.content_margin = Rect(DPI(10), DPI(6), DPI(10), DPI(6)); " << var << ".header_style.hover_enabled = false; " << var << ".header_style.metrics.focus_enabled = false; " << var << ".header_style.title_line = false; " << var << ".header_style.card_line = true; " << var << ".header_style.media_tint_mono = true; " << var << ".header_style.title_font = SansSerifZ(11).Bold(); " << var << ".header_style.subtitle_font = SansSerifZ(8); }\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0) Emit(out, var, p.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

} // namespace

const UiDesignerThemeAdapter& UiDesignerAccordionThemeAdapterInstance()
{
    static Adapter adapter;
    return adapter;
}

} // namespace Upp
