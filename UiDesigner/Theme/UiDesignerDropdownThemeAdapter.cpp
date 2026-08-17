#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiDropdown.h>

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

static int PopupItemState(const String& id, const char *part)
{
    return DotState(id, String("popup_item_") + part);
}

static UiAlign Side(const Value& v, UiAlign fallback = UiAlign::RIGHT)
{
    const String q = AsString(v);
    if(q == "Left") return UiAlign::LEFT;
    if(q == "Top") return UiAlign::TOP;
    if(q == "Bottom") return UiAlign::BOTTOM;
    if(q == "Center") return UiAlign::CENTER;
    if(q == "Right") return UiAlign::RIGHT;
    return fallback;
}

static String SideName(UiAlign a)
{
    if(a == UiAlign::LEFT) return "Left";
    if(a == UiAlign::TOP) return "Top";
    if(a == UiAlign::BOTTOM) return "Bottom";
    if(a == UiAlign::CENTER) return "Center";
    return "Right";
}

static UiIconRenderMode RenderMode(const Value& v)
{
    const String q = AsString(v);
    if(q == "Auto") return UiIconRenderMode::Auto;
    if(q == "PreserveColor") return UiIconRenderMode::PreserveColor;
    return UiIconRenderMode::MonoTint;
}

static String RenderModeName(UiIconRenderMode mode)
{
    if(mode == UiIconRenderMode::Auto) return "Auto";
    if(mode == UiIconRenderMode::PreserveColor) return "PreserveColor";
    return "MonoTint";
}

static UiDropdown::Style Base(const UiDesignerNode& node)
{
    return UiTheme::ResolveDropdown(UiTheme::GetContext(),
                                    Role(node.GetProperty("role", "Standard")));
}

static bool IsField(const String& id)
{
    static const char *fields[] = {
        "radius", "transparent", "align_h", "align_v", "face_enabled",
        "frame_enabled", "frame_width", "font_face", "font_height", "font_bold",
        "font_italic", "margin_left", "margin_top", "margin_right", "margin_bottom",
        "content_gap", "show_indicator", "indicator_side", "indicator_size",
        "focus_enabled", "focus_margin", "focus_alpha", "focus_color",
        "shadow_enabled", "shadow_distance", "shadow_x", "shadow_y", "shadow_alpha",
        "shadow_color", "shadow_inset", "shadow_mode",
        "highlight_enabled", "highlight_thickness", "highlight_x", "highlight_y",
        "highlight_alpha", "highlight_color", "highlight_style",
        "popup_max_height", "popup_min_width", "popup_item_height", "item_spacing",
        "popup_show_scrollbar", "popup_space", "popup_max_items",
        "popup_frame_width", "popup_radius", "popup_frame_color", "popup_background_color",
        "popup_use_main_skin", "popup_item_transparent", "popup_item_face_enabled",
        "popup_item_frame_enabled", "popup_item_frame_width", "popup_item_radius",
        "popup_item_font_face", "popup_item_font_height", "popup_item_font_bold",
        "popup_item_font_italic", "popup_item_margin_left", "popup_item_margin_top",
        "popup_item_margin_right", "popup_item_margin_bottom",
        "show_popup_selection_marker", "popup_marker_render_mode", "popup_marker_side",
        "show_selection_badge", "selection_badge_radius", "selection_badge_face",
        "selection_badge_ink", "show_drag_handle", "drag_size", "drag_gap",
        "drag_side", "drag_marker", "ink_normal", "ink_hot", "ink_pressed", "ink_disabled"
    };
    if(State(id, "face") >= 0 || State(id, "frame") >= 0 ||
       PopupItemState(id, "face") >= 0 || PopupItemState(id, "frame") >= 0 ||
       PopupItemState(id, "ink") >= 0)
        return true;
    for(const char *field : fields)
        if(id == field) return true;
    return false;
}

static Value ValueOf(const UiDropdown::Style& s, const String& id)
{
    int state = State(id, "face");
    if(state >= 0) return FillRecipe(s.palette.face[state]).ToValue();
    state = State(id, "frame");
    if(state >= 0) return s.palette.frame[state];
    state = State(id, "ink");
    if(state >= 0) return s.palette.ink[state];
    state = PopupItemState(id, "face");
    if(state >= 0) return FillRecipe(s.popup_item_style.palette.face[state]).ToValue();
    state = PopupItemState(id, "frame");
    if(state >= 0) return s.popup_item_style.palette.frame[state];
    state = PopupItemState(id, "ink");
    if(state >= 0) return s.popup_item_style.palette.ink[state];

    if(id == "radius") return s.metrics.radius;
    if(id == "transparent") return s.transparent;
    if(id == "align_h") return SideName(s.align_h);
    if(id == "align_v") return SideName(s.align_v);
    if(id == "face_enabled") return s.metrics.face_enabled;
    if(id == "frame_enabled") return s.metrics.frame_enabled;
    if(id == "frame_width") return s.metrics.frame_width;
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_height") return s.font.GetHeight();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    if(id == "margin_left") return s.metrics.content_margin.left;
    if(id == "margin_top") return s.metrics.content_margin.top;
    if(id == "margin_right") return s.metrics.content_margin.right;
    if(id == "margin_bottom") return s.metrics.content_margin.bottom;
    if(id == "content_gap") return s.content_gap;
    if(id == "show_indicator") return s.show_indicator;
    if(id == "indicator_side") return SideName(s.indicator_side);
    if(id == "indicator_size") return s.indicator_size;
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
    if(id == "popup_max_height") return s.popup_max_height;
    if(id == "popup_min_width") return s.popup_min_width;
    if(id == "popup_item_height") return s.popup_item_height;
    if(id == "item_spacing") return s.item_spacing;
    if(id == "popup_show_scrollbar") return s.popup_show_scrollbar;
    if(id == "popup_space") return s.popup_space;
    if(id == "popup_max_items") return s.popup_max_items;
    if(id == "popup_frame_width") return s.popup_frame_width;
    if(id == "popup_radius") return s.popup_radius;
    if(id == "popup_frame_color") return s.popup_frame_color;
    if(id == "popup_background_color") return s.popup_background_color;
    if(id == "popup_use_main_skin") return s.popup_use_main_skin;
    if(id == "popup_item_transparent") return s.popup_item_style.transparent;
    if(id == "popup_item_face_enabled") return s.popup_item_style.metrics.face_enabled;
    if(id == "popup_item_frame_enabled") return s.popup_item_style.metrics.frame_enabled;
    if(id == "popup_item_frame_width") return s.popup_item_style.metrics.frame_width;
    if(id == "popup_item_radius") return s.popup_item_style.metrics.radius;
    if(id == "popup_item_font_face") return s.popup_item_style.font.GetFaceName();
    if(id == "popup_item_font_height") return s.popup_item_style.font.GetHeight();
    if(id == "popup_item_font_bold") return s.popup_item_style.font.IsBold();
    if(id == "popup_item_font_italic") return s.popup_item_style.font.IsItalic();
    if(id == "popup_item_margin_left") return s.popup_item_style.metrics.content_margin.left;
    if(id == "popup_item_margin_top") return s.popup_item_style.metrics.content_margin.top;
    if(id == "popup_item_margin_right") return s.popup_item_style.metrics.content_margin.right;
    if(id == "popup_item_margin_bottom") return s.popup_item_style.metrics.content_margin.bottom;
    if(id == "show_popup_selection_marker") return s.show_popup_selection_marker;
    if(id == "popup_marker_render_mode") return RenderModeName(s.popup_marker_render_mode);
    if(id == "popup_marker_side") return SideName(s.popup_marker_side);
    if(id == "show_selection_badge") return s.show_selection_badge;
    if(id == "selection_badge_radius") return s.selection_badge_radius;
    if(id == "selection_badge_face") return s.selection_badge_face;
    if(id == "selection_badge_ink") return s.selection_badge_ink;
    if(id == "show_drag_handle") return s.show_drag_handle;
    if(id == "drag_size") return s.drag_size;
    if(id == "drag_gap") return s.drag_gap;
    if(id == "drag_side") return SideName(s.drag_side);
    if(id == "drag_marker") return s.drag_marker;
    return Value();
}

static void Apply(UiDropdown::Style& s, const String& id, const Value& v)
{
    int state = State(id, "face");
    if(state >= 0) { ApplyFill(s.palette.face[state], v); return; }
    state = State(id, "frame");
    if(state >= 0) { s.palette.frame[state] = (Color)v; return; }
    state = State(id, "ink");
    if(state >= 0) { s.palette.ink[state] = (Color)v; return; }
    state = PopupItemState(id, "face");
    if(state >= 0) { ApplyFill(s.popup_item_style.palette.face[state], v); return; }
    state = PopupItemState(id, "frame");
    if(state >= 0) { s.popup_item_style.palette.frame[state] = (Color)v; return; }
    state = PopupItemState(id, "ink");
    if(state >= 0) { s.popup_item_style.palette.ink[state] = (Color)v; return; }

    if(id == "radius") s.metrics.radius = max(0, (int)v);
    else if(id == "transparent") s.transparent = (bool)v;
    else if(id == "align_h") s.align_h = Side(v, UiAlign::LEFT);
    else if(id == "align_v") s.align_v = Side(v, UiAlign::CENTER);
    else if(id == "face_enabled") s.metrics.face_enabled = (bool)v;
    else if(id == "frame_enabled") s.metrics.frame_enabled = (bool)v;
    else if(id == "frame_width") s.metrics.frame_width = max(0, (int)v);
    else if(id == "font_face") s.font.FaceName(AsString(v));
    else if(id == "font_height") s.font.Height(max(1, (int)v));
    else if(id == "font_bold") s.font.Bold((bool)v);
    else if(id == "font_italic") s.font.Italic((bool)v);
    else if(id == "margin_left") s.metrics.content_margin.left = max(0, (int)v);
    else if(id == "margin_top") s.metrics.content_margin.top = max(0, (int)v);
    else if(id == "margin_right") s.metrics.content_margin.right = max(0, (int)v);
    else if(id == "margin_bottom") s.metrics.content_margin.bottom = max(0, (int)v);
    else if(id == "content_gap") s.content_gap = max(0, (int)v);
    else if(id == "show_indicator") s.show_indicator = (bool)v;
    else if(id == "indicator_side") s.indicator_side = Side(v);
    else if(id == "indicator_size") s.indicator_size = max(0, (int)v);
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
    else if(id == "popup_max_height") s.popup_max_height = max(40, (int)v);
    else if(id == "popup_min_width") s.popup_min_width = max(40, (int)v);
    else if(id == "popup_item_height") s.popup_item_height = max(18, (int)v);
    else if(id == "item_spacing") s.item_spacing = max(0, (int)v);
    else if(id == "popup_show_scrollbar") s.popup_show_scrollbar = (bool)v;
    else if(id == "popup_space") s.popup_space = max(0, (int)v);
    else if(id == "popup_max_items") s.popup_max_items = max(1, (int)v);
    else if(id == "popup_frame_width") s.popup_frame_width = max(0, (int)v);
    else if(id == "popup_radius") s.popup_radius = max(0, (int)v);
    else if(id == "popup_frame_color") s.popup_frame_color = (Color)v;
    else if(id == "popup_background_color") s.popup_background_color = (Color)v;
    else if(id == "popup_use_main_skin") s.popup_use_main_skin = (bool)v;
    else if(id == "popup_item_transparent") s.popup_item_style.transparent = (bool)v;
    else if(id == "popup_item_face_enabled") s.popup_item_style.metrics.face_enabled = (bool)v;
    else if(id == "popup_item_frame_enabled") s.popup_item_style.metrics.frame_enabled = (bool)v;
    else if(id == "popup_item_frame_width") s.popup_item_style.metrics.frame_width = max(0, (int)v);
    else if(id == "popup_item_radius") s.popup_item_style.metrics.radius = max(0, (int)v);
    else if(id == "popup_item_font_face") s.popup_item_style.font.FaceName(AsString(v));
    else if(id == "popup_item_font_height") s.popup_item_style.font.Height(max(1, (int)v));
    else if(id == "popup_item_font_bold") s.popup_item_style.font.Bold((bool)v);
    else if(id == "popup_item_font_italic") s.popup_item_style.font.Italic((bool)v);
    else if(id == "popup_item_margin_left") s.popup_item_style.metrics.content_margin.left = max(0, (int)v);
    else if(id == "popup_item_margin_top") s.popup_item_style.metrics.content_margin.top = max(0, (int)v);
    else if(id == "popup_item_margin_right") s.popup_item_style.metrics.content_margin.right = max(0, (int)v);
    else if(id == "popup_item_margin_bottom") s.popup_item_style.metrics.content_margin.bottom = max(0, (int)v);
    else if(id == "show_popup_selection_marker") s.show_popup_selection_marker = (bool)v;
    else if(id == "popup_marker_render_mode") s.popup_marker_render_mode = RenderMode(v);
    else if(id == "popup_marker_side") s.popup_marker_side = Side(v);
    else if(id == "show_selection_badge") s.show_selection_badge = (bool)v;
    else if(id == "selection_badge_radius") s.selection_badge_radius = max(0, (int)v);
    else if(id == "selection_badge_face") s.selection_badge_face = (Color)v;
    else if(id == "selection_badge_ink") s.selection_badge_ink = (Color)v;
    else if(id == "show_drag_handle") s.show_drag_handle = (bool)v;
    else if(id == "drag_size") s.drag_size = max(1, (int)v);
    else if(id == "drag_gap") s.drag_gap = max(0, (int)v);
    else if(id == "drag_side") s.drag_side = Side(v);
    else if(id == "drag_marker") s.drag_marker = (Color)v;
}

static void AddOverrides(UiDesignerControlSpec& spec)
{
    const UiDropdown::Style s = UiTheme::ResolveDropdown(UiTheme::GetContext(), UiRole::Standard);
    AddInt(spec, "radius", "Radius", "General", s.metrics.radius, 0, 96);
    Add(spec, "transparent", "Transparent", "General", PropertyEditorKind::Boolean, s.transparent);
    Add(spec, "face_enabled", "Enabled", "Face", PropertyEditorKind::Boolean, s.metrics.face_enabled);
    Add(spec, "face_normal", "Normal", "Face", PropertyEditorKind::FillRecipe, FillRecipe(s.palette.face[ST_NORMAL]).ToValue());
    for(int i = 1; i < 4; i++) Add(spec, "face_" + String(kStates[i]), kLabels[i], "Face", PropertyEditorKind::FillRecipe, FillRecipe(s.palette.face[i]).ToValue());
    Add(spec, "frame_enabled", "Enabled", "Frame", PropertyEditorKind::Boolean, s.metrics.frame_enabled, true);
    AddInt(spec, "frame_width", "Width", "Frame", s.metrics.frame_width, 0, 24, true);
    for(int i = 0; i < 4; i++) Add(spec, "frame_" + String(kStates[i]), kLabels[i], "Frame", PropertyEditorKind::Color, s.palette.frame[i]);
    Add(spec, "ink_normal", "Normal", "Ink", PropertyEditorKind::Color, s.palette.ink[ST_NORMAL]);
    for(int i = 1; i < 4; i++) Add(spec, "ink_" + String(kStates[i]), kLabels[i], "Ink", PropertyEditorKind::Color, s.palette.ink[i]);
    Add(spec, "font_face", "Font face", "Typography", PropertyEditorKind::Text, s.font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "font_height", "Font height", "Typography", max(1, s.font.GetHeight()), 6, 96, true);
    Add(spec, "font_bold", "Bold", "Typography", PropertyEditorKind::Boolean, s.font.IsBold(), true);
    Add(spec, "font_italic", "Italic", "Typography", PropertyEditorKind::Boolean, s.font.IsItalic(), true);
    AddInt(spec, "margin_left", "Left", "Content Margin", s.metrics.content_margin.left, 0, 80, true);
    AddInt(spec, "margin_top", "Top", "Content Margin", s.metrics.content_margin.top, 0, 80, true);
    AddInt(spec, "margin_right", "Right", "Content Margin", s.metrics.content_margin.right, 0, 80, true);
    AddInt(spec, "margin_bottom", "Bottom", "Content Margin", s.metrics.content_margin.bottom, 0, 80, true);
    Add(spec, "align_h", "Horizontal Align", "Layout", PropertyEditorKind::Choice, SideName(s.align_h), true).Choice("Left", "Left").Choice("Center", "Center").Choice("Right", "Right");
    Add(spec, "align_v", "Vertical Align", "Layout", PropertyEditorKind::Choice, SideName(s.align_v), true).Choice("Top", "Top").Choice("Center", "Center").Choice("Bottom", "Bottom");
    AddInt(spec, "content_gap", "Content Gap", "Layout", s.content_gap, 0, 40, true);
    Add(spec, "show_indicator", "Show", "Indicator", PropertyEditorKind::Boolean, s.show_indicator, true);
    Add(spec, "indicator_side", "Side", "Indicator", PropertyEditorKind::Choice, SideName(s.indicator_side), true).Choice("Left", "Left").Choice("Right", "Right").Choice("Top", "Top").Choice("Bottom", "Bottom");
    AddInt(spec, "indicator_size", "Size", "Indicator", s.indicator_size, 0, 64, true);
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

    AddInt(spec, "popup_max_height", "Max Height", "Popup/Layout", s.popup_max_height, 40, 1200, true);
    AddInt(spec, "popup_min_width", "Min Width", "Popup/Layout", s.popup_min_width, 40, 1200, true);
    AddInt(spec, "popup_item_height", "Item Height", "Popup/Layout", s.popup_item_height, 18, 160, true);
    AddInt(spec, "item_spacing", "Item Spacing", "Popup/Layout", s.item_spacing, 0, 40, true);
    Add(spec, "popup_show_scrollbar", "Show Scrollbar", "Popup/Layout", PropertyEditorKind::Boolean, s.popup_show_scrollbar, true);
    AddInt(spec, "popup_space", "Control Gap", "Popup/Layout", s.popup_space, 0, 40, true);
    AddInt(spec, "popup_max_items", "Max Items", "Popup/Layout", s.popup_max_items, 1, 100, true);
    Add(spec, "popup_background_color", "Background", "Popup/Face", PropertyEditorKind::Color, s.popup_background_color);
    Add(spec, "popup_use_main_skin", "Use Main Skin", "Popup/Face", PropertyEditorKind::Boolean, s.popup_use_main_skin);
    AddInt(spec, "popup_frame_width", "Width", "Popup/Frame", s.popup_frame_width, 0, 24, true);
    AddInt(spec, "popup_radius", "Radius", "Popup/Frame", s.popup_radius, 0, 96);
    Add(spec, "popup_frame_color", "Colour", "Popup/Frame", PropertyEditorKind::Color, s.popup_frame_color);

    Add(spec, "popup_item_transparent", "Transparent", "Popup/Items", PropertyEditorKind::Boolean, s.popup_item_style.transparent);
    Add(spec, "popup_item_face_enabled", "Face Enabled", "Popup/Items", PropertyEditorKind::Boolean, s.popup_item_style.metrics.face_enabled);
    Add(spec, "popup_item_frame_enabled", "Frame Enabled", "Popup/Items", PropertyEditorKind::Boolean, s.popup_item_style.metrics.frame_enabled);
    for(int i = 0; i < 4; i++) Add(spec, "popup_item_face." + String(kStates[i]), kLabels[i], "Popup/Items/Face", PropertyEditorKind::FillRecipe, FillRecipe(s.popup_item_style.palette.face[i]).ToValue());
    AddInt(spec, "popup_item_frame_width", "Width", "Popup/Items/Frame", s.popup_item_style.metrics.frame_width, 0, 24, true);
    AddInt(spec, "popup_item_radius", "Radius", "Popup/Items/Frame", s.popup_item_style.metrics.radius, 0, 96);
    for(int i = 0; i < 4; i++) Add(spec, "popup_item_frame." + String(kStates[i]), kLabels[i], "Popup/Items/Frame", PropertyEditorKind::Color, s.popup_item_style.palette.frame[i]);
    for(int i = 0; i < 4; i++) Add(spec, "popup_item_ink." + String(kStates[i]), kLabels[i], "Popup/Items/Ink", PropertyEditorKind::Color, s.popup_item_style.palette.ink[i]);
    Add(spec, "popup_item_font_face", "Font face", "Popup/Items/Typography", PropertyEditorKind::Text, s.popup_item_style.font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "popup_item_font_height", "Font height", "Popup/Items/Typography", max(1, s.popup_item_style.font.GetHeight()), 6, 96, true);
    Add(spec, "popup_item_font_bold", "Bold", "Popup/Items/Typography", PropertyEditorKind::Boolean, s.popup_item_style.font.IsBold(), true);
    Add(spec, "popup_item_font_italic", "Italic", "Popup/Items/Typography", PropertyEditorKind::Boolean, s.popup_item_style.font.IsItalic(), true);
    AddInt(spec, "popup_item_margin_left", "Left", "Popup/Items/Content Margin", s.popup_item_style.metrics.content_margin.left, 0, 80, true);
    AddInt(spec, "popup_item_margin_top", "Top", "Popup/Items/Content Margin", s.popup_item_style.metrics.content_margin.top, 0, 80, true);
    AddInt(spec, "popup_item_margin_right", "Right", "Popup/Items/Content Margin", s.popup_item_style.metrics.content_margin.right, 0, 80, true);
    AddInt(spec, "popup_item_margin_bottom", "Bottom", "Popup/Items/Content Margin", s.popup_item_style.metrics.content_margin.bottom, 0, 80, true);

    Add(spec, "show_popup_selection_marker", "Show Selection Marker", "Popup/Marker", PropertyEditorKind::Boolean, s.show_popup_selection_marker);
    Add(spec, "popup_marker_render_mode", "Render Mode", "Popup/Marker", PropertyEditorKind::Choice, RenderModeName(s.popup_marker_render_mode)).Choice("MonoTint", "Mono Tint").Choice("PreserveColor", "Preserve Color").Choice("Auto", "Auto");
    Add(spec, "popup_marker_side", "Side", "Popup/Marker", PropertyEditorKind::Choice, SideName(s.popup_marker_side), true).Choice("Left", "Left").Choice("Right", "Right");
    Add(spec, "show_selection_badge", "Show Badge", "Popup/Badge", PropertyEditorKind::Boolean, s.show_selection_badge, true);
    AddInt(spec, "selection_badge_radius", "Radius", "Popup/Badge", s.selection_badge_radius, 0, 96);
    Add(spec, "selection_badge_face", "Face", "Popup/Badge", PropertyEditorKind::Color, s.selection_badge_face);
    Add(spec, "selection_badge_ink", "Ink", "Popup/Badge", PropertyEditorKind::Color, s.selection_badge_ink);
    Add(spec, "show_drag_handle", "Show Handle", "Drag", PropertyEditorKind::Boolean, s.show_drag_handle, true);
    AddInt(spec, "drag_size", "Size", "Drag", s.drag_size, 1, 64, true);
    AddInt(spec, "drag_gap", "Gap", "Drag", s.drag_gap, 0, 40, true);
    Add(spec, "drag_side", "Side", "Drag", PropertyEditorKind::Choice, SideName(s.drag_side), true).Choice("Left", "Left").Choice("Right", "Right");
    Add(spec, "drag_marker", "Marker Colour", "Drag", PropertyEditorKind::Color, s.drag_marker);
}

static bool AffectsLayout(const String& id)
{
    return id == "frame_enabled" || id == "frame_width" || id.StartsWith("font_") ||
           id.StartsWith("margin_") || id == "align_h" || id == "align_v" ||
           id == "content_gap" || id == "show_indicator" || id == "indicator_side" ||
           id == "indicator_size" || id.StartsWith("popup_") || id == "item_spacing" ||
           id == "show_selection_badge" || id == "show_drag_handle" ||
           id == "drag_size" || id == "drag_gap" || id == "drag_side" ||
           id == "shadow_enabled" || id == "shadow_distance" || id == "shadow_x" ||
           id == "shadow_y" || id == "shadow_inset";
}

static Value ResolveFace(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                         const String& id, const UiDesignerTransientOverlay* overlay,
                         const UiDropdown::Style& base)
{
    int state = State(id, "face");
    if(state < 0) state = PopupItemState(id, "face");
    for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
        if(p.adapter_field_id != id) continue;
        return NormalizeFillValue(ResolveValue(node, overlay, p.id, AuthoredOrDefault(node, p)));
    }
    return state >= 0 ? (id.StartsWith("popup_item_") ? FillRecipe(base.popup_item_style.palette.face[state]).ToValue() : FillRecipe(base.palette.face[state]).ToValue()) : Value();
}

static void Emit(String& out, const String& var, const String& id, const Value& v)
{
    int state = State(id, "face");
    if(state >= 0) { out << "\t" << var << ".palette.face[" << StateCode(state) << "] = " << FillCode(v) << ";\n"; return; }
    state = State(id, "frame");
    if(state >= 0) { out << "\t" << var << ".palette.frame[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    state = State(id, "ink");
    if(state >= 0) { out << "\t" << var << ".palette.ink[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    state = PopupItemState(id, "face");
    if(state >= 0) { out << "\t" << var << ".popup_item_style.palette.face[" << StateCode(state) << "] = " << FillCode(v) << ";\n"; return; }
    state = PopupItemState(id, "frame");
    if(state >= 0) { out << "\t" << var << ".popup_item_style.palette.frame[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    state = PopupItemState(id, "ink");
    if(state >= 0) { out << "\t" << var << ".popup_item_style.palette.ink[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }

    if(id == "radius") out << "\t" << var << ".metrics.radius = " << (int)v << ";\n";
    else if(id == "transparent") out << "\t" << var << ".transparent = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "align_h") out << "\t" << var << ".align_h = " << (AsString(v) == "Right" ? "UiAlign::RIGHT" : AsString(v) == "Center" ? "UiAlign::CENTER" : "UiAlign::LEFT") << ";\n";
    else if(id == "align_v") out << "\t" << var << ".align_v = " << (AsString(v) == "Top" ? "UiAlign::TOP" : AsString(v) == "Bottom" ? "UiAlign::BOTTOM" : "UiAlign::CENTER") << ";\n";
    else if(id == "face_enabled") out << "\t" << var << ".metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_enabled") out << "\t" << var << ".metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_width") out << "\t" << var << ".metrics.frame_width = " << (int)v << ";\n";
    else if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "font_height") out << "\t" << var << ".font.Height(" << (int)v << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id.StartsWith("margin_")) out << "\t" << var << ".metrics.content_margin." << id.Mid(7) << " = " << (int)v << ";\n";
    else if(id == "content_gap" || id == "indicator_size" || id == "popup_max_height" || id == "popup_min_width" || id == "popup_item_height" || id == "item_spacing" || id == "popup_space" || id == "popup_max_items" || id == "popup_frame_width" || id == "popup_radius" || id == "selection_badge_radius" || id == "drag_size" || id == "drag_gap") out << "\t" << var << "." << id << " = " << (int)v << ";\n";
    else if(id == "show_indicator" || id == "popup_show_scrollbar" || id == "popup_use_main_skin" || id == "show_popup_selection_marker" || id == "show_selection_badge" || id == "show_drag_handle") out << "\t" << var << "." << id << " = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "indicator_side" || id == "popup_marker_side" || id == "drag_side") out << "\t" << var << "." << id << " = " << (AsString(v) == "Left" ? "UiAlign::LEFT" : AsString(v) == "Top" ? "UiAlign::TOP" : AsString(v) == "Bottom" ? "UiAlign::BOTTOM" : "UiAlign::RIGHT") << ";\n";
    else if(id == "popup_marker_render_mode") out << "\t" << var << ".popup_marker_render_mode = " << (AsString(v) == "Auto" ? "UiIconRenderMode::Auto" : AsString(v) == "PreserveColor" ? "UiIconRenderMode::PreserveColor" : "UiIconRenderMode::MonoTint") << ";\n";
    else if(id == "popup_frame_color" || id == "popup_background_color" || id == "selection_badge_face" || id == "selection_badge_ink" || id == "drag_marker") out << "\t" << var << "." << id << " = " << EmitValue(v) << ";\n";
    else if(id == "popup_item_transparent") out << "\t" << var << ".popup_item_style.transparent = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "popup_item_face_enabled") out << "\t" << var << ".popup_item_style.metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "popup_item_frame_enabled") out << "\t" << var << ".popup_item_style.metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "popup_item_frame_width") out << "\t" << var << ".popup_item_style.metrics.frame_width = " << (int)v << ";\n";
    else if(id == "popup_item_radius") out << "\t" << var << ".popup_item_style.metrics.radius = " << (int)v << ";\n";
    else if(id == "popup_item_font_face") out << "\t" << var << ".popup_item_style.font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "popup_item_font_height") out << "\t" << var << ".popup_item_style.font.Height(" << (int)v << ");\n";
    else if(id == "popup_item_font_bold") out << "\t" << var << ".popup_item_style.font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id == "popup_item_font_italic") out << "\t" << var << ".popup_item_style.font.Italic(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id.StartsWith("popup_item_margin_")) out << "\t" << var << ".popup_item_style.metrics.content_margin." << id.Mid(18) << " = " << (int)v << ";\n";
    else if(id == "focus_enabled") out << "\t" << var << ".metrics.focus_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "focus_margin") out << "\t" << var << ".metrics.focus_margin = " << (int)v << ";\n";
    else if(id == "focus_alpha") out << "\t" << var << ".metrics.focus_alpha = " << (int)v << ";\n";
    else if(id == "focus_color") out << "\t" << var << ".metrics.focus_color = " << EmitValue(v) << ";\n";
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
    else if(id == "highlight_style") out << "\t" << var << ".metrics.highlight.style = " << (AsString(v) == "Dashed" ? "DASHED" : AsString(v) == "Dotted" ? "DOTTED" : "SOLID") << ";\n";
}

class Adapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "dropdown"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == UiDesignerRuntimeKind::UiDropdown; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override { AddOverrides(spec); }
    bool HasField(const String& id) const override { return IsField(id); }
    bool FieldAffectsLayout(const String& id) const override { return AffectsLayout(id); }

    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        UiDropdown::Style style = Base(node);
        if(State(id, "face") >= 0 || PopupItemState(id, "face") >= 0)
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
        UiDropdown *dropdown = dynamic_cast<UiDropdown *>(&ctrl);
        if(!dropdown) return;
        UiDropdown::Style style = Base(node);
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            if(!HasValue(node, overlay, p.id)) continue;
            authored = true;
            Apply(style, p.adapter_field_id,
                  ResolveValue(node, overlay, p.id, AuthoredOrDefault(node, p)));
        }
        if(authored || Role(node.GetProperty("role", "Standard")) != UiRole::Standard)
            dropdown->SetCustomStyle(style);
        else
            dropdown->ClearCustomStyle();
    }

    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
            authored |= node.theme_overrides.Find(p.id) >= 0;
        const Value role = node.GetProperty("role", "Standard");
        if(!authored && Role(role) == UiRole::Standard) return;
        const String var = member + "_style";
        out << "\tUiDropdown::Style " << var
            << " = UiTheme::ResolveDropdown(UiTheme::GetContext(), "
            << RoleExpr(role) << ");\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0) Emit(out, var, p.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

} // namespace

const UiDesignerThemeAdapter& UiDesignerDropdownThemeAdapterInstance()
{
    static Adapter adapter;
    return adapter;
}

} // namespace Upp
