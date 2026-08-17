#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiList.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;

static const char *const kListStates[] = { "normal", "hot", "pressed", "disabled" };
static const char *const kStateLabels[] = { "Normal", "Hot", "Pressed", "Disabled" };

static UiList::Style ListBase()
{
    return UiTheme::ResolveList();
}

static bool IsListField(const String& id)
{
    static const char *fields[] = {
        "radius", "high_contrast", "face_enabled", "frame_enabled", "frame_width",
        "icon.normal", "font_face", "font_height", "font_bold", "font_italic",
        "margin_left", "margin_top", "margin_right", "margin_bottom",
        "focus_enabled", "focus_margin", "focus_alpha", "focus_color",
        "shadow_enabled", "shadow_distance", "shadow_x", "shadow_y", "shadow_alpha",
        "shadow_color", "shadow_inset", "shadow_mode",
        "highlight_enabled", "highlight_thickness", "highlight_x", "highlight_y",
        "highlight_alpha", "highlight_color", "highlight_style",
        "row_height", "item_spacing", "icon_size", "check_size", "content_gap",
        "h_padding", "v_padding", "row_radius", "metadata_size", "metadata_gap",
        "right_gap", "show_icons", "show_checks", "show_metadata_marker",
        "ink", "disabled_ink", "muted_ink", "hot_face", "hot_frame", "hot_ink",
        "selected_face", "selected_frame", "selected_ink", "hot_as_underline",
        "selected_as_underline", "state_underline_thickness", "striped_rows",
        "row_even_face", "row_odd_face", "show_row_separator", "separator_color",
        "row_state_frame_enabled", "right_text_as_badge", "badge_face", "badge_frame",
        "badge_ink", "badge_radius", "badge_h_padding", "metadata_default",
        "check_frame", "check_fill", "show_drag_handle", "drag_size", "drag_gap",
        "drag_side", "drag_marker"
    };
    if(DotState(id, "face") >= 0 || DotState(id, "frame") >= 0)
        return true;
    for(const char *field : fields)
        if(id == field)
            return true;
    return false;
}

static Value ListFieldValue(const UiList::Style& s, const String& id)
{
    int state = DotState(id, "face");
    if(state >= 0) return FillRecipe(s.palette.face[state]).ToValue();
    state = DotState(id, "frame");
    if(state >= 0) return s.palette.frame[state];
    if(id == "icon.normal") return s.palette.icon[ST_NORMAL];
    if(id == "radius") return s.metrics.radius;
    if(id == "high_contrast") return s.metrics.high_contrast;
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
    if(id == "row_height") return s.row_height;
    if(id == "item_spacing") return s.item_spacing;
    if(id == "icon_size") return s.icon_size;
    if(id == "check_size") return s.check_size;
    if(id == "content_gap") return s.content_gap;
    if(id == "h_padding") return s.h_padding;
    if(id == "v_padding") return s.v_padding;
    if(id == "row_radius") return s.row_radius;
    if(id == "metadata_size") return s.metadata_size;
    if(id == "metadata_gap") return s.metadata_gap;
    if(id == "right_gap") return s.right_gap;
    if(id == "show_icons") return s.show_icons;
    if(id == "show_checks") return s.show_checks;
    if(id == "show_metadata_marker") return s.show_metadata_marker;
    if(id == "ink") return s.ink;
    if(id == "disabled_ink") return s.disabled_ink;
    if(id == "muted_ink") return s.muted_ink;
    if(id == "hot_face") return s.hot_face;
    if(id == "hot_frame") return s.hot_frame;
    if(id == "hot_ink") return s.hot_ink;
    if(id == "selected_face") return s.selected_face;
    if(id == "selected_frame") return s.selected_frame;
    if(id == "selected_ink") return s.selected_ink;
    if(id == "hot_as_underline") return s.hot_as_underline;
    if(id == "selected_as_underline") return s.selected_as_underline;
    if(id == "state_underline_thickness") return s.state_underline_thickness;
    if(id == "striped_rows") return s.striped_rows;
    if(id == "row_even_face") return s.row_even_face;
    if(id == "row_odd_face") return s.row_odd_face;
    if(id == "show_row_separator") return s.show_row_separator;
    if(id == "separator_color") return s.separator_color;
    if(id == "row_state_frame_enabled") return s.row_state_frame_enabled;
    if(id == "right_text_as_badge") return s.right_text_as_badge;
    if(id == "badge_face") return s.badge_face;
    if(id == "badge_frame") return s.badge_frame;
    if(id == "badge_ink") return s.badge_ink;
    if(id == "badge_radius") return s.badge_radius;
    if(id == "badge_h_padding") return s.badge_h_padding;
    if(id == "metadata_default") return s.metadata_default;
    if(id == "check_frame") return s.check_frame;
    if(id == "check_fill") return s.check_fill;
    if(id == "show_drag_handle") return s.show_drag_handle;
    if(id == "drag_size") return s.drag_size;
    if(id == "drag_gap") return s.drag_gap;
    if(id == "drag_side") return s.drag_side == UiAlign::LEFT ? "Left" : "Right";
    if(id == "drag_marker") return s.drag_marker;
    return Value();
}

static void ApplyListField(UiList::Style& s, const String& id, const Value& v)
{
    int state = DotState(id, "face");
    if(state >= 0) { ApplyFill(s.palette.face[state], v); return; }
    state = DotState(id, "frame");
    if(state >= 0) { s.palette.frame[state] = (Color)v; return; }
    if(id == "icon.normal") s.palette.icon[ST_NORMAL] = (Color)v;
    else if(id == "radius") s.metrics.radius = max(0, (int)v);
    else if(id == "high_contrast") s.metrics.high_contrast = (bool)v;
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
    else if(id == "highlight_style") { String q = AsString(v); s.metrics.highlight.style = q == "Dashed" ? DASHED : q == "Dotted" ? DOTTED : SOLID; }
    else if(id == "row_height") s.row_height = max(18, (int)v);
    else if(id == "item_spacing") s.item_spacing = max(0, (int)v);
    else if(id == "icon_size") s.icon_size = max(1, (int)v);
    else if(id == "check_size") s.check_size = max(1, (int)v);
    else if(id == "content_gap") s.content_gap = max(0, (int)v);
    else if(id == "h_padding") s.h_padding = max(0, (int)v);
    else if(id == "v_padding") s.v_padding = max(0, (int)v);
    else if(id == "row_radius") s.row_radius = max(0, (int)v);
    else if(id == "metadata_size") s.metadata_size = max(1, (int)v);
    else if(id == "metadata_gap") s.metadata_gap = max(0, (int)v);
    else if(id == "right_gap") s.right_gap = max(0, (int)v);
    else if(id == "show_icons") s.show_icons = (bool)v;
    else if(id == "show_checks") s.show_checks = (bool)v;
    else if(id == "show_metadata_marker") s.show_metadata_marker = (bool)v;
    else if(id == "ink") s.ink = (Color)v;
    else if(id == "disabled_ink") s.disabled_ink = (Color)v;
    else if(id == "muted_ink") s.muted_ink = (Color)v;
    else if(id == "hot_face") s.hot_face = (Color)v;
    else if(id == "hot_frame") s.hot_frame = (Color)v;
    else if(id == "hot_ink") s.hot_ink = (Color)v;
    else if(id == "selected_face") s.selected_face = (Color)v;
    else if(id == "selected_frame") s.selected_frame = (Color)v;
    else if(id == "selected_ink") s.selected_ink = (Color)v;
    else if(id == "hot_as_underline") s.hot_as_underline = (bool)v;
    else if(id == "selected_as_underline") s.selected_as_underline = (bool)v;
    else if(id == "state_underline_thickness") s.state_underline_thickness = max(1, (int)v);
    else if(id == "striped_rows") s.striped_rows = (bool)v;
    else if(id == "row_even_face") s.row_even_face = (Color)v;
    else if(id == "row_odd_face") s.row_odd_face = (Color)v;
    else if(id == "show_row_separator") s.show_row_separator = (bool)v;
    else if(id == "separator_color") s.separator_color = (Color)v;
    else if(id == "row_state_frame_enabled") s.row_state_frame_enabled = (bool)v;
    else if(id == "right_text_as_badge") s.right_text_as_badge = (bool)v;
    else if(id == "badge_face") s.badge_face = (Color)v;
    else if(id == "badge_frame") s.badge_frame = (Color)v;
    else if(id == "badge_ink") s.badge_ink = (Color)v;
    else if(id == "badge_radius") s.badge_radius = max(0, (int)v);
    else if(id == "badge_h_padding") s.badge_h_padding = max(0, (int)v);
    else if(id == "metadata_default") s.metadata_default = (Color)v;
    else if(id == "check_frame") s.check_frame = (Color)v;
    else if(id == "check_fill") s.check_fill = (Color)v;
    else if(id == "show_drag_handle") s.show_drag_handle = (bool)v;
    else if(id == "drag_size") s.drag_size = max(1, (int)v);
    else if(id == "drag_gap") s.drag_gap = max(0, (int)v);
    else if(id == "drag_side") s.drag_side = AsString(v) == "Left" ? UiAlign::LEFT : UiAlign::RIGHT;
    else if(id == "drag_marker") s.drag_marker = (Color)v;
}

static void AddListOverrides(UiDesignerControlSpec& spec)
{
    const UiList::Style s = ListBase();
    AddInt(spec, "radius", "Radius", "General", s.metrics.radius, 0, 96);
    Add(spec, "high_contrast", "High contrast", "General", PropertyEditorKind::Boolean, s.metrics.high_contrast);
    Add(spec, "face_enabled", "Enabled", "Face", PropertyEditorKind::Boolean, s.metrics.face_enabled);
    for(int i = 0; i < 4; i++) Add(spec, "face." + String(kListStates[i]), kStateLabels[i], "Face", PropertyEditorKind::FillRecipe, FillRecipe(s.palette.face[i]).ToValue());
    Add(spec, "frame_enabled", "Enabled", "Frame", PropertyEditorKind::Boolean, s.metrics.frame_enabled, true);
    AddInt(spec, "frame_width", "Width", "Frame", s.metrics.frame_width, 0, 24, true);
    for(int i = 0; i < 4; i++) Add(spec, "frame." + String(kListStates[i]), kStateLabels[i], "Frame", PropertyEditorKind::Color, s.palette.frame[i]);
    Add(spec, "ink", "Normal", "Ink", PropertyEditorKind::Color, s.ink);
    Add(spec, "disabled_ink", "Disabled", "Ink", PropertyEditorKind::Color, s.disabled_ink);
    Add(spec, "muted_ink", "Muted", "Ink", PropertyEditorKind::Color, s.muted_ink);
    Add(spec, "icon.normal", "Normal", "Icon", PropertyEditorKind::Color, s.palette.icon[ST_NORMAL]);
    Add(spec, "font_face", "Font face", "Typography", PropertyEditorKind::Text, s.font.GetFaceName(), true).Editor("property.font");
    AddInt(spec, "font_height", "Font height", "Typography", max(1, s.font.GetHeight()), 6, 96, true);
    Add(spec, "font_bold", "Bold", "Typography", PropertyEditorKind::Boolean, s.font.IsBold(), true);
    Add(spec, "font_italic", "Italic", "Typography", PropertyEditorKind::Boolean, s.font.IsItalic(), true);
    AddInt(spec, "margin_left", "Left", "Content Margin", s.metrics.content_margin.left, 0, 80, true);
    AddInt(spec, "margin_top", "Top", "Content Margin", s.metrics.content_margin.top, 0, 80, true);
    AddInt(spec, "margin_right", "Right", "Content Margin", s.metrics.content_margin.right, 0, 80, true);
    AddInt(spec, "margin_bottom", "Bottom", "Content Margin", s.metrics.content_margin.bottom, 0, 80, true);
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

    AddInt(spec, "row_height", "Height", "Rows/Layout", s.row_height, 18, 160, true);
    AddInt(spec, "item_spacing", "Spacing", "Rows/Layout", s.item_spacing, 0, 40, true);
    AddInt(spec, "h_padding", "Horizontal padding", "Rows/Layout", s.h_padding, 0, 80, true);
    AddInt(spec, "v_padding", "Vertical padding", "Rows/Layout", s.v_padding, 0, 40, true);
    AddInt(spec, "row_radius", "Radius", "Rows/Layout", s.row_radius, 0, 80);
    AddInt(spec, "icon_size", "Icon size", "Rows/Layout", s.icon_size, 1, 96, true);
    AddInt(spec, "check_size", "Check size", "Rows/Layout", s.check_size, 1, 96, true);
    AddInt(spec, "content_gap", "Content gap", "Rows/Layout", s.content_gap, 0, 40, true);
    AddInt(spec, "metadata_size", "Metadata size", "Rows/Layout", s.metadata_size, 1, 48, true);
    AddInt(spec, "metadata_gap", "Metadata gap", "Rows/Layout", s.metadata_gap, 0, 40, true);
    AddInt(spec, "right_gap", "Right gap", "Rows/Layout", s.right_gap, 0, 80, true);

    Add(spec, "hot_face", "Hot Face", "Rows/State", PropertyEditorKind::Color, s.hot_face);
    Add(spec, "hot_frame", "Hot Frame", "Rows/State", PropertyEditorKind::Color, s.hot_frame);
    Add(spec, "hot_ink", "Hot Ink", "Rows/State", PropertyEditorKind::Color, s.hot_ink);
    Add(spec, "selected_face", "Selected Face", "Rows/State", PropertyEditorKind::Color, s.selected_face);
    Add(spec, "selected_frame", "Selected Frame", "Rows/State", PropertyEditorKind::Color, s.selected_frame);
    Add(spec, "selected_ink", "Selected Ink", "Rows/State", PropertyEditorKind::Color, s.selected_ink);
    Add(spec, "row_state_frame_enabled", "State Frame", "Rows/State", PropertyEditorKind::Boolean, s.row_state_frame_enabled);
    Add(spec, "hot_as_underline", "Hot as Underline", "Rows/State", PropertyEditorKind::Boolean, s.hot_as_underline);
    Add(spec, "selected_as_underline", "Selected as Underline", "Rows/State", PropertyEditorKind::Boolean, s.selected_as_underline);
    AddInt(spec, "state_underline_thickness", "Underline Thickness", "Rows/State", s.state_underline_thickness, 1, 12);
    Add(spec, "striped_rows", "Striped Rows", "Rows/State", PropertyEditorKind::Boolean, s.striped_rows);
    Add(spec, "row_even_face", "Even Face", "Rows/State", PropertyEditorKind::Color, s.row_even_face);
    Add(spec, "row_odd_face", "Odd Face", "Rows/State", PropertyEditorKind::Color, s.row_odd_face);
    Add(spec, "show_row_separator", "Separator", "Rows/State", PropertyEditorKind::Boolean, s.show_row_separator);
    Add(spec, "separator_color", "Separator Colour", "Rows/State", PropertyEditorKind::Color, s.separator_color);

    Add(spec, "show_icons", "Show Icons", "Content", PropertyEditorKind::Boolean, s.show_icons, true);
    Add(spec, "show_checks", "Show Checks", "Content", PropertyEditorKind::Boolean, s.show_checks, true);
    Add(spec, "show_metadata_marker", "Show Metadata", "Content", PropertyEditorKind::Boolean, s.show_metadata_marker, true);
    Add(spec, "metadata_default", "Metadata Colour", "Content", PropertyEditorKind::Color, s.metadata_default);
    Add(spec, "check_frame", "Check Frame", "Content", PropertyEditorKind::Color, s.check_frame);
    Add(spec, "check_fill", "Check Fill", "Content", PropertyEditorKind::Color, s.check_fill);

    Add(spec, "right_text_as_badge", "Enabled", "Badge", PropertyEditorKind::Boolean, s.right_text_as_badge, true);
    Add(spec, "badge_face", "Face", "Badge", PropertyEditorKind::Color, s.badge_face);
    Add(spec, "badge_frame", "Frame", "Badge", PropertyEditorKind::Color, s.badge_frame);
    Add(spec, "badge_ink", "Ink", "Badge", PropertyEditorKind::Color, s.badge_ink);
    AddInt(spec, "badge_radius", "Radius", "Badge", s.badge_radius, 0, 999);
    AddInt(spec, "badge_h_padding", "Horizontal Padding", "Badge", s.badge_h_padding, 0, 40, true);

    Add(spec, "show_drag_handle", "Show Handle", "Drag", PropertyEditorKind::Boolean, s.show_drag_handle, true);
    AddInt(spec, "drag_size", "Size", "Drag", s.drag_size, 1, 64, true);
    AddInt(spec, "drag_gap", "Gap", "Drag", s.drag_gap, 0, 40, true);
    Add(spec, "drag_side", "Side", "Drag", PropertyEditorKind::Choice, s.drag_side == UiAlign::LEFT ? "Left" : "Right", true).Choice("Left", "Left").Choice("Right", "Right");
    Add(spec, "drag_marker", "Marker Colour", "Drag", PropertyEditorKind::Color, s.drag_marker);
}

static bool ListFieldAffectsLayout(const String& id)
{
    return id == "frame_enabled" || id == "frame_width" || id.StartsWith("margin_") ||
           id.StartsWith("font_") || id == "row_height" || id == "item_spacing" ||
           id == "h_padding" || id == "v_padding" || id == "icon_size" ||
           id == "check_size" || id == "content_gap" || id == "metadata_size" ||
           id == "metadata_gap" || id == "right_gap" || id == "show_icons" ||
           id == "show_checks" || id == "show_metadata_marker" ||
           id == "right_text_as_badge" || id == "badge_h_padding" ||
           id == "show_drag_handle" || id == "drag_size" || id == "drag_gap" ||
           id == "drag_side" || id == "shadow_enabled" || id == "shadow_distance" ||
           id == "shadow_x" || id == "shadow_y" || id == "shadow_inset";
}

static Value ResolveListFace(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                             const String& id, const UiDesignerTransientOverlay* overlay,
                             const UiList::Style& base)
{
    const int state = DotState(id, "face");
    if(state < 0) return Value();
    const Value inherited = FillRecipe(base.palette.face[state]).ToValue();
    for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
        if(property.adapter_field_id != id) continue;
        if(!HasValue(node, overlay, property.id)) return inherited;
        return ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property));
    }
    return inherited;
}

static void EmitListField(String& out, const String& var, const String& id, const Value& v)
{
    int state = DotState(id, "face");
    if(state >= 0) { out << "\t" << var << ".palette.face[" << StateCode(state) << "] = " << FillCode(v) << ";\n"; return; }
    state = DotState(id, "frame");
    if(state >= 0) { out << "\t" << var << ".palette.frame[" << StateCode(state) << "] = " << EmitValue(v) << ";\n"; return; }
    if(id == "icon.normal") out << "\t" << var << ".palette.icon[ST_NORMAL] = " << EmitValue(v) << ";\n";
    else if(id == "radius") out << "\t" << var << ".metrics.radius = " << max(0, (int)v) << ";\n";
    else if(id == "high_contrast") out << "\t" << var << ".metrics.high_contrast = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "face_enabled") out << "\t" << var << ".metrics.face_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_enabled") out << "\t" << var << ".metrics.frame_enabled = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "frame_width") out << "\t" << var << ".metrics.frame_width = " << max(0, (int)v) << ";\n";
    else if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "font_height") out << "\t" << var << ".font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << ((bool)v ? "true" : "false") << ");\n";
    else if(id.StartsWith("margin_")) out << "\t" << var << ".metrics.content_margin." << id.Mid(7) << " = " << max(0, (int)v) << ";\n";
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
    else if(id == "drag_side") out << "\t" << var << ".drag_side = " << (AsString(v) == "Left" ? "UiAlign::LEFT" : "UiAlign::RIGHT") << ";\n";
    else if(id == "row_height" || id == "item_spacing" || id == "icon_size" || id == "check_size" || id == "content_gap" || id == "h_padding" || id == "v_padding" || id == "row_radius" || id == "metadata_size" || id == "metadata_gap" || id == "right_gap" || id == "state_underline_thickness" || id == "badge_radius" || id == "badge_h_padding" || id == "drag_size" || id == "drag_gap") out << "\t" << var << "." << id << " = " << (int)v << ";\n";
    else if(id == "show_icons" || id == "show_checks" || id == "show_metadata_marker" || id == "hot_as_underline" || id == "selected_as_underline" || id == "striped_rows" || id == "show_row_separator" || id == "row_state_frame_enabled" || id == "right_text_as_badge" || id == "show_drag_handle") out << "\t" << var << "." << id << " = " << ((bool)v ? "true" : "false") << ";\n";
    else if(id == "ink" || id == "disabled_ink" || id == "muted_ink" || id == "hot_face" || id == "hot_frame" || id == "hot_ink" || id == "selected_face" || id == "selected_frame" || id == "selected_ink" || id == "row_even_face" || id == "row_odd_face" || id == "separator_color" || id == "badge_face" || id == "badge_frame" || id == "badge_ink" || id == "metadata_default" || id == "check_frame" || id == "check_fill" || id == "drag_marker") out << "\t" << var << "." << id << " = " << EmitValue(v) << ";\n";
}

class NormalizedListAdapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "list"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == UiDesignerRuntimeKind::UiList; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override { AddListOverrides(spec); }
    bool HasField(const String& id) const override { return IsListField(id); }
    bool FieldAffectsLayout(const String& id) const override { return ListFieldAffectsLayout(id); }

    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        UiList::Style style = ListBase();
        if(DotState(id, "face") >= 0)
            return ResolveListFace(node, spec, id, overlay, style);
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            if(!HasValue(node, overlay, property.id)) continue;
            ApplyListField(style, property.adapter_field_id,
                           ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property)));
        }
        return ListFieldValue(style, id);
    }

    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiList *list = dynamic_cast<UiList *>(&ctrl);
        if(!list) return;
        UiList::Style style = ListBase();
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            if(!HasValue(node, overlay, property.id)) continue;
            authored = true;
            ApplyListField(style, property.adapter_field_id,
                           ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property)));
        }
        if(authored) list->SetCustomStyle(style); else list->ClearCustomStyle();
    }

    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides)
            authored |= node.theme_overrides.Find(property.id) >= 0;
        if(!authored) return;
        const String var = member + "_style";
        out << "\tUiList::Style " << var << " = UiTheme::ResolveList();\n";
        for(const UiDesignerThemeOverrideSpec& property : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(property.id);
            if(q >= 0) EmitListField(out, var, property.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

} // namespace

const UiDesignerThemeAdapter& UiDesignerListThemeAdapterInstance()
{
    static NormalizedListAdapter adapter;
    return adapter;
}

} // namespace Upp
