#include "UiDesignerStyledThemeCommon.h"
#include <Ui/UiPanel.h>
#include <Ui/UiGroupPanel.h>
#include <Ui/UiScrollPanel.h>
#include <Ui/UiTab.h>
#include <Ui/UiTheme.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;
using namespace UiDesignerStyledTheme;

static UiRole ContainerRole(const UiDesignerNode& node)
{
    return Role(node.GetProperty("role", "Standard"));
}

static bool HasAuthored(const UiDesignerNode& node,
                        const UiDesignerControlSpec& spec)
{
    for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
        if(node.theme_overrides.Find(p.id) >= 0)
            return true;
    return false;
}

static Value Effective(const UiDesignerNode& node,
                       const UiDesignerThemeOverrideSpec& p,
                       const UiDesignerTransientOverlay* overlay,
                       bool& active)
{
    const int q = node.theme_overrides.Find(p.id);
    active = q >= 0 || HasValue(node, overlay, p.id);
    if(!active)
        return Value();
    return ResolveValue(node, overlay, p.id,
                        q >= 0 ? node.theme_overrides.GetValue(q)
                               : p.default_value);
}

static void RenameOverride(UiDesignerControlSpec& spec,
                           const String& from, const String& to)
{
    for(UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
        if(p.id == from) {
            p.id = to;
            p.adapter_field_id = to;
            return;
        }
}

static void RemoveOverride(UiDesignerControlSpec& spec, const String& id)
{
    for(int i = spec.theme_overrides.GetCount() - 1; i >= 0; --i)
        if(spec.theme_overrides[i].id == id)
            spec.theme_overrides.Remove(i);
}

static String PanelCommonId(const String& id)
{
    if(id == "face.normal") return "face_normal";
    if(id == "face.hot") return "face_hot";
    if(id == "face.pressed") return "face_pressed";
    if(id == "face.disabled") return "face_disabled";
    if(id == "frame.normal") return "frame_normal";
    if(id == "frame.hot") return "frame_hot";
    if(id == "frame.pressed") return "frame_pressed";
    if(id == "frame.disabled") return "frame_disabled";
    if(id == "frame.width") return "frame_width";
    return id;
}

static String PanelFrameStyleName(const StyledMetrics& metrics)
{
    if(!metrics.frame_enabled)
        return "None";
    if(!metrics.dashed)
        return "Solid";
    return metrics.dash_pattern == "1,3" ? "Dotted" : "Dashed";
}

static void ApplyPanelFrameStyle(StyledMetrics& metrics, const Value& value)
{
    const String style = AsString(value);
    metrics.frame_enabled = style != "None";
    metrics.dashed = style == "Dashed" || style == "Dotted";
    if(style == "Dotted")
        metrics.dash_pattern = "1,3";
    else if(style == "Dashed")
        metrics.dash_pattern = "5,5";
}

static void AddPanelCommon(UiDesignerControlSpec& spec,
                           const StyledPalette& palette,
                           const StyledMetrics& metrics,
                           bool transparent)
{
    AddPaletteMetrics(spec, "", "", palette, metrics,
                      false, false, true, true, true, true);

    // Preserve the durable dotted panel recipe IDs used by existing project
    // and standalone Theme JSON documents.
    RenameOverride(spec, "face_normal", "face.normal");
    RenameOverride(spec, "face_hot", "face.hot");
    RenameOverride(spec, "face_pressed", "face.pressed");
    RenameOverride(spec, "face_disabled", "face.disabled");
    RenameOverride(spec, "frame_normal", "frame.normal");
    RenameOverride(spec, "frame_hot", "frame.hot");
    RenameOverride(spec, "frame_pressed", "frame.pressed");
    RenameOverride(spec, "frame_disabled", "frame.disabled");
    RenameOverride(spec, "frame_width", "frame.width");

    // Panel documents historically represent the frame on/off/dash contract
    // as one stable field. Keep that contract instead of serializing three
    // competing switches for the same visual decision.
    RemoveOverride(spec, "frame_enabled");
    RemoveOverride(spec, "frame_dashed");
    RemoveOverride(spec, "frame_dash_pattern");
    Add(spec, "frame.style", "Style", "Frame", PropertyEditorKind::Choice,
        PanelFrameStyleName(metrics), true)
        .Choice("None", "None")
        .Choice("Solid", "Solid")
        .Choice("Dashed", "Dashed")
        .Choice("Dotted", "Dotted");

    Add(spec, "transparent", "Transparent", "Appearance",
        PropertyEditorKind::Boolean, transparent);
}

static bool IsPanelCommonField(const String& id)
{
    return id == "frame.style" || id == "transparent" ||
           IsPaletteMetricsField("", PanelCommonId(id));
}

static Value PanelCommonValue(const StyledPalette& palette,
                              const StyledMetrics& metrics,
                              bool transparent, const String& id)
{
    if(id == "frame.style") return PanelFrameStyleName(metrics);
    if(id == "transparent") return transparent;
    return PaletteMetricsValue(palette, metrics, "", PanelCommonId(id));
}

static bool ApplyPanelCommon(StyledPalette& palette, StyledMetrics& metrics,
                             bool& transparent, const String& id,
                             const Value& value)
{
    if(id == "frame.style") {
        ApplyPanelFrameStyle(metrics, value);
        return true;
    }
    if(id == "transparent") {
        transparent = (bool)value;
        return true;
    }
    return ApplyPaletteMetrics(palette, metrics, "", PanelCommonId(id), value);
}

static bool PanelCommonAffectsLayout(const String& id)
{
    return id == "frame.style" ||
           PaletteMetricsAffectsLayout("", PanelCommonId(id));
}

static bool EmitPanelCommon(String& out, const String& var,
                            const String& id, const Value& value)
{
    if(id == "frame.style") {
        const String style = AsString(value);
        out << "\t" << var << ".metrics.frame_enabled = "
            << (style == "None" ? "false" : "true") << ";\n";
        out << "\t" << var << ".metrics.dashed = "
            << (style == "Dashed" || style == "Dotted" ? "true" : "false")
            << ";\n";
        if(style == "Dotted")
            out << "\t" << var << ".metrics.dash_pattern = \"1,3\";\n";
        else if(style == "Dashed")
            out << "\t" << var << ".metrics.dash_pattern = \"5,5\";\n";
        return true;
    }
    if(id == "transparent") {
        out << "\t" << var << ".transparent = " << AsString((bool)value) << ";\n";
        return true;
    }
    return EmitPaletteMetrics(out, var + ".palette", var + ".metrics", "",
                              PanelCommonId(id), value);
}

// -----------------------------------------------------------------------------
// UiPanel

class PanelThemeAdapterV2 final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "panel"; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return kind == UiDesignerRuntimeKind::UiPanel;
    }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        const UiPanel::Style s = UiTheme::ResolvePanel(UiRole::Standard);
        AddPanelCommon(spec, s.palette, s.metrics, s.transparent);
    }
    bool HasField(const String& id) const override { return IsPanelCommonField(id); }
    bool FieldAffectsLayout(const String& id) const override
    {
        return PanelCommonAffectsLayout(id);
    }
    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiPanel::Style s = UiTheme::ResolvePanel(ContainerRole(node));
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) ApplyPanelCommon(s.palette, s.metrics, s.transparent,
                                        p.adapter_field_id, v);
        }
        return PanelCommonValue(s.palette, s.metrics, s.transparent, id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiPanel *c = dynamic_cast<UiPanel *>(&ctrl);
        if(!c) return;
        UiPanel::Style s = UiTheme::ResolvePanel(ContainerRole(node));
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) {
                authored = true;
                ApplyPanelCommon(s.palette, s.metrics, s.transparent,
                                 p.adapter_field_id, v);
            }
        }
        if(authored || ContainerRole(node) != UiRole::Standard)
            c->SetCustomStyle(s);
        else
            c->ClearCustomStyle();
    }
    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        if(!HasAuthored(node, spec) && ContainerRole(node) == UiRole::Standard)
            return;
        const String var = member + "_style";
        out << "\tUiPanel::Style " << var << " = UiTheme::ResolvePanel("
            << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0)
                EmitPanelCommon(out, var, p.adapter_field_id,
                                node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

// -----------------------------------------------------------------------------
// UiScrollPanel

class ScrollPanelThemeAdapterV2 final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "scroll_panel"; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return kind == UiDesignerRuntimeKind::UiScrollPanel;
    }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        const UiScrollPanel::Style s = UiTheme::ResolveScrollPanel(UiRole::Standard);
        AddPanelCommon(spec, s.palette, s.metrics, s.transparent);
    }
    bool HasField(const String& id) const override { return IsPanelCommonField(id); }
    bool FieldAffectsLayout(const String& id) const override
    {
        return PanelCommonAffectsLayout(id);
    }
    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiScrollPanel::Style s = UiTheme::ResolveScrollPanel(ContainerRole(node));
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) ApplyPanelCommon(s.palette, s.metrics, s.transparent,
                                        p.adapter_field_id, v);
        }
        return PanelCommonValue(s.palette, s.metrics, s.transparent, id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiScrollPanel *c = dynamic_cast<UiScrollPanel *>(&ctrl);
        if(!c) return;
        UiScrollPanel::Style s = UiTheme::ResolveScrollPanel(ContainerRole(node));
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) {
                authored = true;
                ApplyPanelCommon(s.palette, s.metrics, s.transparent,
                                 p.adapter_field_id, v);
            }
        }
        if(authored || ContainerRole(node) != UiRole::Standard)
            c->SetCustomStyle(s);
        else
            c->ClearCustomStyle();
    }
    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        if(!HasAuthored(node, spec) && ContainerRole(node) == UiRole::Standard)
            return;
        const String var = member + "_style";
        out << "\tUiScrollPanel::Style " << var << " = UiTheme::ResolveScrollPanel("
            << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0)
                EmitPanelCommon(out, var, p.adapter_field_id,
                                node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

// -----------------------------------------------------------------------------
// UiGroupPanel

static String HeaderModeName(UiGroupPanel::HeaderMode mode)
{
    if(mode == UiGroupPanel::Outside) return "Outside";
    if(mode == UiGroupPanel::Center) return "Center";
    return "Inside";
}

static UiGroupPanel::HeaderMode HeaderModeValue(const Value& value)
{
    const String s = AsString(value);
    if(s == "Outside") return UiGroupPanel::Outside;
    if(s == "Center") return UiGroupPanel::Center;
    return UiGroupPanel::Inside;
}

static void AddGroupPanelOverrides(UiDesignerControlSpec& spec)
{
    const UiGroupPanel::Style s = UiTheme::ResolveGroupPanel(UiRole::Standard);
    AddPanelCommon(spec, s.palette, s.metrics, s.transparent);

    Add(spec, "title_color", "Title colour", "Header / Ink",
        PropertyEditorKind::Color, s.title_color);
    Add(spec, "subtitle_color", "Subtitle colour", "Header / Ink",
        PropertyEditorKind::Color, s.subtitle_color);

    Add(spec, "title_font_face", "Title font face", "Header / Typography",
        PropertyEditorKind::Text, s.title_font.GetFaceName(), true).Editor("property.font");
    AddNumeric(spec, "title_font_height", "Title font height", "Header / Typography",
               max(1, s.title_font.GetHeight()), 6, 128, true);
    Add(spec, "title_font_bold", "Title bold", "Header / Typography",
        PropertyEditorKind::Boolean, s.title_font.IsBold(), true);
    Add(spec, "title_font_italic", "Title italic", "Header / Typography",
        PropertyEditorKind::Boolean, s.title_font.IsItalic(), true);

    Add(spec, "subtitle_font_face", "Subtitle font face", "Header / Typography",
        PropertyEditorKind::Text, s.subtitle_font.GetFaceName(), true).Editor("property.font");
    AddNumeric(spec, "subtitle_font_height", "Subtitle font height", "Header / Typography",
               max(1, s.subtitle_font.GetHeight()), 6, 128, true);
    Add(spec, "subtitle_font_bold", "Subtitle bold", "Header / Typography",
        PropertyEditorKind::Boolean, s.subtitle_font.IsBold(), true);
    Add(spec, "subtitle_font_italic", "Subtitle italic", "Header / Typography",
        PropertyEditorKind::Boolean, s.subtitle_font.IsItalic(), true);

    AddAlignChoice(spec, "header_placement", "Header placement", "Header / Layout",
                   s.header_placement, true);
    AddAlignChoice(spec, "title_align_h", "Title horizontal align", "Header / Layout",
                   s.title_align_h);
    AddAlignChoice(spec, "title_align_v", "Title vertical align", "Header / Layout",
                   s.title_align_v);
    Add(spec, "header_mode", "Header mode", "Header / Layout",
        PropertyEditorKind::Choice, HeaderModeName(s.header_mode), true)
        .Choice("Outside", "Outside").Choice("Center", "Center").Choice("Inside", "Inside");

    AddNumeric(spec, "inset_left", "Left", "Body Inset", s.inset.left, 0, 256, true);
    AddNumeric(spec, "inset_top", "Top", "Body Inset", s.inset.top, 0, 256, true);
    AddNumeric(spec, "inset_right", "Right", "Body Inset", s.inset.right, 0, 256, true);
    AddNumeric(spec, "inset_bottom", "Bottom", "Body Inset", s.inset.bottom, 0, 256, true);
    AddNumeric(spec, "header_inset_left", "Left", "Header / Inset", s.header_inset.left, 0, 256, true);
    AddNumeric(spec, "header_inset_top", "Top", "Header / Inset", s.header_inset.top, 0, 256, true);
    AddNumeric(spec, "header_inset_right", "Right", "Header / Inset", s.header_inset.right, 0, 256, true);
    AddNumeric(spec, "header_inset_bottom", "Bottom", "Header / Inset", s.header_inset.bottom, 0, 256, true);

    AddNumeric(spec, "header_gap", "Header gap", "Header / Spacing", s.header_gap, 0, 128, true);
    AddNumeric(spec, "icon_size", "Icon size", "Header / Spacing", s.icon_size, 0, 256, true);
    AddNumeric(spec, "icon_gap", "Icon gap", "Header / Spacing", s.icon_gap, 0, 128, true);
    AddNumeric(spec, "title_subtitle_gap", "Title/subtitle gap", "Header / Spacing",
               s.title_subtitle_gap, 0, 128, true);
    AddNumeric(spec, "separator_thickness", "Separator thickness", "Header / Spacing",
               s.separator_thickness, 0, 32, true);
    Add(spec, "line_enabled", "Separator line", "Header / Appearance",
        PropertyEditorKind::Boolean, s.line_enabled, true);
    Add(spec, "header_band_enabled", "Header band", "Header / Appearance",
        PropertyEditorKind::Boolean, s.header_band_enabled, true);
}

static bool IsGroupExtra(const String& id)
{
    static const char *ids[] = {
        "title_color", "subtitle_color", "title_font_face", "title_font_height",
        "title_font_bold", "title_font_italic", "subtitle_font_face",
        "subtitle_font_height", "subtitle_font_bold", "subtitle_font_italic",
        "header_placement", "title_align_h", "title_align_v", "header_mode",
        "inset_left", "inset_top", "inset_right", "inset_bottom",
        "header_inset_left", "header_inset_top", "header_inset_right",
        "header_inset_bottom", "header_gap", "icon_size", "icon_gap",
        "title_subtitle_gap", "separator_thickness", "line_enabled",
        "header_band_enabled"
    };
    for(const char *candidate : ids)
        if(id == candidate) return true;
    return false;
}

static bool ApplyGroupField(UiGroupPanel::Style& s, const String& id, const Value& v)
{
    if(ApplyPanelCommon(s.palette, s.metrics, s.transparent, id, v)) return true;
    if(id == "title_color") s.title_color = (Color)v;
    else if(id == "subtitle_color") s.subtitle_color = (Color)v;
    else if(id == "title_font_face") s.title_font.FaceName(AsString(v));
    else if(id == "title_font_height") s.title_font.Height(max(1, (int)v));
    else if(id == "title_font_bold") s.title_font.Bold((bool)v);
    else if(id == "title_font_italic") s.title_font.Italic((bool)v);
    else if(id == "subtitle_font_face") s.subtitle_font.FaceName(AsString(v));
    else if(id == "subtitle_font_height") s.subtitle_font.Height(max(1, (int)v));
    else if(id == "subtitle_font_bold") s.subtitle_font.Bold((bool)v);
    else if(id == "subtitle_font_italic") s.subtitle_font.Italic((bool)v);
    else if(id == "header_placement") s.header_placement = AlignFromValue(v, s.header_placement);
    else if(id == "title_align_h") s.title_align_h = AlignFromValue(v, s.title_align_h);
    else if(id == "title_align_v") s.title_align_v = AlignFromValue(v, s.title_align_v);
    else if(id == "header_mode") s.header_mode = HeaderModeValue(v);
    else if(id == "inset_left") s.inset.left = max(0, (int)v);
    else if(id == "inset_top") s.inset.top = max(0, (int)v);
    else if(id == "inset_right") s.inset.right = max(0, (int)v);
    else if(id == "inset_bottom") s.inset.bottom = max(0, (int)v);
    else if(id == "header_inset_left") s.header_inset.left = max(0, (int)v);
    else if(id == "header_inset_top") s.header_inset.top = max(0, (int)v);
    else if(id == "header_inset_right") s.header_inset.right = max(0, (int)v);
    else if(id == "header_inset_bottom") s.header_inset.bottom = max(0, (int)v);
    else if(id == "header_gap") s.header_gap = max(0, (int)v);
    else if(id == "icon_size") s.icon_size = max(0, (int)v);
    else if(id == "icon_gap") s.icon_gap = max(0, (int)v);
    else if(id == "title_subtitle_gap") s.title_subtitle_gap = max(0, (int)v);
    else if(id == "separator_thickness") s.separator_thickness = max(0, (int)v);
    else if(id == "line_enabled") s.line_enabled = (bool)v;
    else if(id == "header_band_enabled") s.header_band_enabled = (bool)v;
    else return false;
    return true;
}

static Value GroupFieldValue(const UiGroupPanel::Style& s, const String& id)
{
    if(IsPanelCommonField(id)) return PanelCommonValue(s.palette, s.metrics, s.transparent, id);
    if(id == "title_color") return s.title_color;
    if(id == "subtitle_color") return s.subtitle_color;
    if(id == "title_font_face") return s.title_font.GetFaceName();
    if(id == "title_font_height") return s.title_font.GetHeight();
    if(id == "title_font_bold") return s.title_font.IsBold();
    if(id == "title_font_italic") return s.title_font.IsItalic();
    if(id == "subtitle_font_face") return s.subtitle_font.GetFaceName();
    if(id == "subtitle_font_height") return s.subtitle_font.GetHeight();
    if(id == "subtitle_font_bold") return s.subtitle_font.IsBold();
    if(id == "subtitle_font_italic") return s.subtitle_font.IsItalic();
    if(id == "header_placement") return AlignName(s.header_placement);
    if(id == "title_align_h") return AlignName(s.title_align_h);
    if(id == "title_align_v") return AlignName(s.title_align_v);
    if(id == "header_mode") return HeaderModeName(s.header_mode);
    if(id == "inset_left") return s.inset.left;
    if(id == "inset_top") return s.inset.top;
    if(id == "inset_right") return s.inset.right;
    if(id == "inset_bottom") return s.inset.bottom;
    if(id == "header_inset_left") return s.header_inset.left;
    if(id == "header_inset_top") return s.header_inset.top;
    if(id == "header_inset_right") return s.header_inset.right;
    if(id == "header_inset_bottom") return s.header_inset.bottom;
    if(id == "header_gap") return s.header_gap;
    if(id == "icon_size") return s.icon_size;
    if(id == "icon_gap") return s.icon_gap;
    if(id == "title_subtitle_gap") return s.title_subtitle_gap;
    if(id == "separator_thickness") return s.separator_thickness;
    if(id == "line_enabled") return s.line_enabled;
    if(id == "header_band_enabled") return s.header_band_enabled;
    return Value();
}

static bool EmitGroupField(String& out, const String& var,
                           const String& id, const Value& v)
{
    if(EmitPanelCommon(out, var, id, v)) return true;
    if(id == "title_color") out << "\t" << var << ".title_color = " << EmitValue(v) << ";\n";
    else if(id == "subtitle_color") out << "\t" << var << ".subtitle_color = " << EmitValue(v) << ";\n";
    else if(id == "title_font_face") out << "\t" << var << ".title_font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "title_font_height") out << "\t" << var << ".title_font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "title_font_bold") out << "\t" << var << ".title_font.Bold(" << AsString((bool)v) << ");\n";
    else if(id == "title_font_italic") out << "\t" << var << ".title_font.Italic(" << AsString((bool)v) << ");\n";
    else if(id == "subtitle_font_face") out << "\t" << var << ".subtitle_font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "subtitle_font_height") out << "\t" << var << ".subtitle_font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "subtitle_font_bold") out << "\t" << var << ".subtitle_font.Bold(" << AsString((bool)v) << ");\n";
    else if(id == "subtitle_font_italic") out << "\t" << var << ".subtitle_font.Italic(" << AsString((bool)v) << ");\n";
    else if(id == "header_placement") out << "\t" << var << ".header_placement = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "title_align_h") out << "\t" << var << ".title_align_h = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "title_align_v") out << "\t" << var << ".title_align_v = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "header_mode") out << "\t" << var << ".header_mode = UiGroupPanel::" << AsString(v) << ";\n";
    else if(id == "inset_left") out << "\t" << var << ".inset.left = " << max(0, (int)v) << ";\n";
    else if(id == "inset_top") out << "\t" << var << ".inset.top = " << max(0, (int)v) << ";\n";
    else if(id == "inset_right") out << "\t" << var << ".inset.right = " << max(0, (int)v) << ";\n";
    else if(id == "inset_bottom") out << "\t" << var << ".inset.bottom = " << max(0, (int)v) << ";\n";
    else if(id == "header_inset_left") out << "\t" << var << ".header_inset.left = " << max(0, (int)v) << ";\n";
    else if(id == "header_inset_top") out << "\t" << var << ".header_inset.top = " << max(0, (int)v) << ";\n";
    else if(id == "header_inset_right") out << "\t" << var << ".header_inset.right = " << max(0, (int)v) << ";\n";
    else if(id == "header_inset_bottom") out << "\t" << var << ".header_inset.bottom = " << max(0, (int)v) << ";\n";
    else if(id == "header_gap") out << "\t" << var << ".header_gap = " << max(0, (int)v) << ";\n";
    else if(id == "icon_size") out << "\t" << var << ".icon_size = " << max(0, (int)v) << ";\n";
    else if(id == "icon_gap") out << "\t" << var << ".icon_gap = " << max(0, (int)v) << ";\n";
    else if(id == "title_subtitle_gap") out << "\t" << var << ".title_subtitle_gap = " << max(0, (int)v) << ";\n";
    else if(id == "separator_thickness") out << "\t" << var << ".separator_thickness = " << max(0, (int)v) << ";\n";
    else if(id == "line_enabled") out << "\t" << var << ".line_enabled = " << AsString((bool)v) << ";\n";
    else if(id == "header_band_enabled") out << "\t" << var << ".header_band_enabled = " << AsString((bool)v) << ";\n";
    else return false;
    return true;
}

class GroupPanelThemeAdapterV2 final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "group_panel"; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return kind == UiDesignerRuntimeKind::UiGroupPanel;
    }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        AddGroupPanelOverrides(spec);
    }
    bool HasField(const String& id) const override
    {
        return IsPanelCommonField(id) || IsGroupExtra(id);
    }
    bool FieldAffectsLayout(const String& id) const override
    {
        return PanelCommonAffectsLayout(id) || IsGroupExtra(id);
    }
    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiGroupPanel::Style s = UiTheme::ResolveGroupPanel(ContainerRole(node));
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) ApplyGroupField(s, p.adapter_field_id, v);
        }
        return GroupFieldValue(s, id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiGroupPanel *c = dynamic_cast<UiGroupPanel *>(&ctrl);
        if(!c) return;
        UiGroupPanel::Style s = UiTheme::ResolveGroupPanel(ContainerRole(node));
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) {
                authored = true;
                ApplyGroupField(s, p.adapter_field_id, v);
            }
        }
        if(authored || ContainerRole(node) != UiRole::Standard)
            c->SetCustomStyle(s);
        else
            c->ClearCustomStyle();
    }
    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        if(!HasAuthored(node, spec) && ContainerRole(node) == UiRole::Standard)
            return;
        const String var = member + "_style";
        out << "\tUiGroupPanel::Style " << var << " = UiTheme::ResolveGroupPanel("
            << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0)
                EmitGroupField(out, var, p.adapter_field_id,
                               node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

// -----------------------------------------------------------------------------
// UiTab

static String SpanName(UiSpan span)
{
    if(span == SMALL) return "Small";
    if(span == MEDIUM) return "Medium";
    if(span == LARGE) return "Large";
    return "None";
}

static UiSpan SpanValue(const Value& value)
{
    const String s = AsString(value);
    if(s == "Small") return SMALL;
    if(s == "Medium") return MEDIUM;
    if(s == "Large") return LARGE;
    return NONE;
}

static String TabVisualName(UiTabVisual visual)
{
    switch(visual) {
    case UITAB_UNDERLINE: return "Underline";
    case UITAB_SEGMENTED: return "Segmented";
    case UITAB_RAIL: return "Rail";
    case UITAB_DOCUMENT: return "Document";
    default: return "Classic";
    }
}

static UiTabVisual TabVisualValue(const Value& value)
{
    const String s = AsString(value);
    if(s == "Underline") return UITAB_UNDERLINE;
    if(s == "Segmented") return UITAB_SEGMENTED;
    if(s == "Rail") return UITAB_RAIL;
    if(s == "Document") return UITAB_DOCUMENT;
    return UITAB_CLASSIC;
}

static void AddTabOverrides(UiDesignerControlSpec& spec)
{
    const UiTab::Style s = UiTheme::ResolveTab(UiRole::Standard, UITAB_CLASSIC);
    AddPaletteMetrics(spec, "", "Body", s.palette, s.metrics,
                      false, false, true, true, true, true);
    AddPaletteMetrics(spec, "tab", "Tab", s.tab_palette, s.tab_metrics,
                      false, false, true, true, true, true);

    Add(spec, "tab_font_face", "Font face", "Tab / Typography",
        PropertyEditorKind::Text, s.tab_font.GetFaceName(), true).Editor("property.font");
    AddNumeric(spec, "tab_font_height", "Font height", "Tab / Typography",
               max(1, s.tab_font.GetHeight()), 6, 128, true);
    Add(spec, "tab_font_bold", "Bold", "Tab / Typography",
        PropertyEditorKind::Boolean, s.tab_font.IsBold(), true);
    Add(spec, "tab_font_italic", "Italic", "Tab / Typography",
        PropertyEditorKind::Boolean, s.tab_font.IsItalic(), true);

    auto add_legacy_int = [&](const char *id, const char *field, const char *label,
                              const char *group, int value, int lo, int hi) {
        UiDesignerThemeOverrideSpec& p = AddNumeric(spec, id, label, group,
                                                    value, lo, hi, true);
        p.adapter_field_id = field;
    };
    auto add_legacy_bool = [&](const char *id, const char *field,
                               const char *label, const char *group, bool value) {
        UiDesignerThemeOverrideSpec& p = Add(spec, id, label, group,
            PropertyEditorKind::Boolean, value, true);
        p.adapter_field_id = field;
    };

    add_legacy_int("style_tab_extent", "tab_extent", "Tab extent", "Layout",
                   s.tab_extent, 1, 512);
    add_legacy_int("style_item_spacing", "item_spacing", "Item spacing", "Layout",
                   s.item_spacing, 0, 128);
    add_legacy_int("style_body_gap", "body_gap", "Body gap", "Layout",
                   s.body_gap, 0, 128);
    add_legacy_int("style_content_gap", "content_gap", "Content gap", "Layout",
                   s.content_gap, 0, 128);
    add_legacy_bool("style_expand_tabs", "expand_tabs", "Expand tabs", "Layout",
                    s.expand_tabs || s.fill_tabs);

    AddNumeric(spec, "tab_padding_left", "Left", "Tab / Padding", s.tab_padding.left, 0, 256, true);
    AddNumeric(spec, "tab_padding_top", "Top", "Tab / Padding", s.tab_padding.top, 0, 256, true);
    AddNumeric(spec, "tab_padding_right", "Right", "Tab / Padding", s.tab_padding.right, 0, 256, true);
    AddNumeric(spec, "tab_padding_bottom", "Bottom", "Tab / Padding", s.tab_padding.bottom, 0, 256, true);
    AddNumeric(spec, "strip_inset_left", "Left", "Strip Inset", s.strip_inset.left, 0, 256, true);
    AddNumeric(spec, "strip_inset_top", "Top", "Strip Inset", s.strip_inset.top, 0, 256, true);
    AddNumeric(spec, "strip_inset_right", "Right", "Strip Inset", s.strip_inset.right, 0, 256, true);
    AddNumeric(spec, "strip_inset_bottom", "Bottom", "Strip Inset", s.strip_inset.bottom, 0, 256, true);
    AddNumeric(spec, "icon_size", "Icon size", "Tab / Content", s.icon_size, 0, 256, true);
    AddAlignChoice(spec, "icon_side", "Icon side", "Tab / Content", s.icon_side, true);
    AddNumeric(spec, "affordance_gap", "Affordance gap", "Tab / Content", s.affordance_gap, 0, 128, true);
    AddNumeric(spec, "min_tab_main", "Minimum tab length", "Tab / Content", s.min_tab_main, 0, 512, true);

    Add(spec, "indicator_color", "Active indicator", "Indicator",
        PropertyEditorKind::Color, s.active_frame_color);
    AddNumeric(spec, "indicator_thickness", "Indicator thickness", "Indicator",
               s.indicator_thickness, 0, 64, true);
    AddNumeric(spec, "active_frame_width", "Active frame width", "Indicator",
               s.active_frame_width, 0, 64, true);
    AddNumeric(spec, "open_corner_radius", "Open-corner radius", "Indicator",
               s.open_corner_radius, 0, 128, true);
    Add(spec, "indicator_span", "Indicator span", "Indicator",
        PropertyEditorKind::Choice, SpanName(s.indicator_span), true)
        .Choice("None", "None").Choice("Small", "Small")
        .Choice("Medium", "Medium").Choice("Large", "Large");
    add_legacy_bool("style_active_tab_uses_body_face", "active_tab_uses_body_face",
                    "Active tab uses body face", "Indicator",
                    s.active_tab_uses_body_face);

    Add(spec, "visual", "Visual", "Appearance", PropertyEditorKind::Choice,
        TabVisualName(s.visual), true)
        .Choice("Classic", "Classic").Choice("Underline", "Underline")
        .Choice("Segmented", "Segmented").Choice("Rail", "Rail")
        .Choice("Document", "Document");
}

static String TabCanonicalId(const String& id)
{
    if(id == "style_tab_extent") return "tab_extent";
    if(id == "style_item_spacing") return "item_spacing";
    if(id == "style_body_gap") return "body_gap";
    if(id == "style_content_gap") return "content_gap";
    if(id == "style_expand_tabs") return "expand_tabs";
    if(id == "style_active_tab_uses_body_face") return "active_tab_uses_body_face";
    return id;
}

static bool IsTabExtra(const String& id)
{
    static const char *ids[] = {
        "tab_font_face", "tab_font_height", "tab_font_bold", "tab_font_italic",
        "tab_extent", "item_spacing", "body_gap", "content_gap", "expand_tabs",
        "tab_padding_left", "tab_padding_top", "tab_padding_right", "tab_padding_bottom",
        "strip_inset_left", "strip_inset_top", "strip_inset_right", "strip_inset_bottom",
        "icon_size", "icon_side", "affordance_gap", "min_tab_main",
        "indicator_color", "indicator_thickness", "active_frame_width",
        "open_corner_radius", "indicator_span", "active_tab_uses_body_face", "visual"
    };
    for(const char *candidate : ids)
        if(id == candidate) return true;
    return false;
}

static bool ApplyTabField(UiTab::Style& s, const String& raw_id, const Value& v)
{
    const String id = TabCanonicalId(raw_id);
    if(ApplyPaletteMetrics(s.palette, s.metrics, "", id, v)) return true;
    if(ApplyPaletteMetrics(s.tab_palette, s.tab_metrics, "tab", id, v)) return true;
    if(id == "tab_font_face") s.tab_font.FaceName(AsString(v));
    else if(id == "tab_font_height") s.tab_font.Height(max(1, (int)v));
    else if(id == "tab_font_bold") s.tab_font.Bold((bool)v);
    else if(id == "tab_font_italic") s.tab_font.Italic((bool)v);
    else if(id == "tab_extent") s.tab_extent = max(1, (int)v);
    else if(id == "item_spacing") s.item_spacing = max(0, (int)v);
    else if(id == "body_gap") s.body_gap = max(0, (int)v);
    else if(id == "content_gap") s.content_gap = max(0, (int)v);
    else if(id == "expand_tabs") s.expand_tabs = s.fill_tabs = (bool)v;
    else if(id == "tab_padding_left") s.tab_padding.left = max(0, (int)v);
    else if(id == "tab_padding_top") s.tab_padding.top = max(0, (int)v);
    else if(id == "tab_padding_right") s.tab_padding.right = max(0, (int)v);
    else if(id == "tab_padding_bottom") s.tab_padding.bottom = max(0, (int)v);
    else if(id == "strip_inset_left") s.strip_inset.left = max(0, (int)v);
    else if(id == "strip_inset_top") s.strip_inset.top = max(0, (int)v);
    else if(id == "strip_inset_right") s.strip_inset.right = max(0, (int)v);
    else if(id == "strip_inset_bottom") s.strip_inset.bottom = max(0, (int)v);
    else if(id == "icon_size") s.icon_size = max(0, (int)v);
    else if(id == "icon_side") s.icon_side = AlignFromValue(v, s.icon_side);
    else if(id == "affordance_gap") s.affordance_gap = max(0, (int)v);
    else if(id == "min_tab_main") s.min_tab_main = max(0, (int)v);
    else if(id == "indicator_color") s.active_frame_color = (Color)v;
    else if(id == "indicator_thickness") s.indicator_thickness = max(0, (int)v);
    else if(id == "active_frame_width") s.active_frame_width = max(0, (int)v);
    else if(id == "open_corner_radius") s.open_corner_radius = max(0, (int)v);
    else if(id == "indicator_span") s.indicator_span = SpanValue(v);
    else if(id == "active_tab_uses_body_face") s.active_tab_uses_body_face = (bool)v;
    else if(id == "visual") s.visual = TabVisualValue(v);
    else return false;
    return true;
}

static Value TabFieldValue(const UiTab::Style& s, const String& raw_id)
{
    const String id = TabCanonicalId(raw_id);
    if(IsPaletteMetricsField("", id)) return PaletteMetricsValue(s.palette, s.metrics, "", id);
    if(IsPaletteMetricsField("tab", id)) return PaletteMetricsValue(s.tab_palette, s.tab_metrics, "tab", id);
    if(id == "tab_font_face") return s.tab_font.GetFaceName();
    if(id == "tab_font_height") return s.tab_font.GetHeight();
    if(id == "tab_font_bold") return s.tab_font.IsBold();
    if(id == "tab_font_italic") return s.tab_font.IsItalic();
    if(id == "tab_extent") return s.tab_extent;
    if(id == "item_spacing") return s.item_spacing;
    if(id == "body_gap") return s.body_gap;
    if(id == "content_gap") return s.content_gap;
    if(id == "expand_tabs") return s.expand_tabs || s.fill_tabs;
    if(id == "tab_padding_left") return s.tab_padding.left;
    if(id == "tab_padding_top") return s.tab_padding.top;
    if(id == "tab_padding_right") return s.tab_padding.right;
    if(id == "tab_padding_bottom") return s.tab_padding.bottom;
    if(id == "strip_inset_left") return s.strip_inset.left;
    if(id == "strip_inset_top") return s.strip_inset.top;
    if(id == "strip_inset_right") return s.strip_inset.right;
    if(id == "strip_inset_bottom") return s.strip_inset.bottom;
    if(id == "icon_size") return s.icon_size;
    if(id == "icon_side") return AlignName(s.icon_side);
    if(id == "affordance_gap") return s.affordance_gap;
    if(id == "min_tab_main") return s.min_tab_main;
    if(id == "indicator_color") return s.active_frame_color;
    if(id == "indicator_thickness") return s.indicator_thickness;
    if(id == "active_frame_width") return s.active_frame_width;
    if(id == "open_corner_radius") return s.open_corner_radius;
    if(id == "indicator_span") return SpanName(s.indicator_span);
    if(id == "active_tab_uses_body_face") return s.active_tab_uses_body_face;
    if(id == "visual") return TabVisualName(s.visual);
    return Value();
}

static bool EmitTabField(String& out, const String& var,
                         const String& raw_id, const Value& v)
{
    const String id = TabCanonicalId(raw_id);
    if(EmitPaletteMetrics(out, var + ".palette", var + ".metrics", "", id, v)) return true;
    if(EmitPaletteMetrics(out, var + ".tab_palette", var + ".tab_metrics", "tab", id, v)) return true;
    if(id == "tab_font_face") out << "\t" << var << ".tab_font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "tab_font_height") out << "\t" << var << ".tab_font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "tab_font_bold") out << "\t" << var << ".tab_font.Bold(" << AsString((bool)v) << ");\n";
    else if(id == "tab_font_italic") out << "\t" << var << ".tab_font.Italic(" << AsString((bool)v) << ");\n";
    else if(id == "tab_extent") out << "\t" << var << ".tab_extent = " << max(1, (int)v) << ";\n";
    else if(id == "item_spacing") out << "\t" << var << ".item_spacing = " << max(0, (int)v) << ";\n";
    else if(id == "body_gap") out << "\t" << var << ".body_gap = " << max(0, (int)v) << ";\n";
    else if(id == "content_gap") out << "\t" << var << ".content_gap = " << max(0, (int)v) << ";\n";
    else if(id == "expand_tabs") out << "\t" << var << ".expand_tabs = " << AsString((bool)v) << "; " << var << ".fill_tabs = " << AsString((bool)v) << ";\n";
    else if(id == "tab_padding_left") out << "\t" << var << ".tab_padding.left = " << max(0, (int)v) << ";\n";
    else if(id == "tab_padding_top") out << "\t" << var << ".tab_padding.top = " << max(0, (int)v) << ";\n";
    else if(id == "tab_padding_right") out << "\t" << var << ".tab_padding.right = " << max(0, (int)v) << ";\n";
    else if(id == "tab_padding_bottom") out << "\t" << var << ".tab_padding.bottom = " << max(0, (int)v) << ";\n";
    else if(id == "strip_inset_left") out << "\t" << var << ".strip_inset.left = " << max(0, (int)v) << ";\n";
    else if(id == "strip_inset_top") out << "\t" << var << ".strip_inset.top = " << max(0, (int)v) << ";\n";
    else if(id == "strip_inset_right") out << "\t" << var << ".strip_inset.right = " << max(0, (int)v) << ";\n";
    else if(id == "strip_inset_bottom") out << "\t" << var << ".strip_inset.bottom = " << max(0, (int)v) << ";\n";
    else if(id == "icon_size") out << "\t" << var << ".icon_size = " << max(0, (int)v) << ";\n";
    else if(id == "icon_side") out << "\t" << var << ".icon_side = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "affordance_gap") out << "\t" << var << ".affordance_gap = " << max(0, (int)v) << ";\n";
    else if(id == "min_tab_main") out << "\t" << var << ".min_tab_main = " << max(0, (int)v) << ";\n";
    else if(id == "indicator_color") out << "\t" << var << ".active_frame_color = " << EmitValue(v) << ";\n";
    else if(id == "indicator_thickness") out << "\t" << var << ".indicator_thickness = " << max(0, (int)v) << ";\n";
    else if(id == "active_frame_width") out << "\t" << var << ".active_frame_width = " << max(0, (int)v) << ";\n";
    else if(id == "open_corner_radius") out << "\t" << var << ".open_corner_radius = " << max(0, (int)v) << ";\n";
    else if(id == "indicator_span") out << "\t" << var << ".indicator_span = " << (AsString(v) == "Small" ? "SMALL" : AsString(v) == "Medium" ? "MEDIUM" : AsString(v) == "Large" ? "LARGE" : "NONE") << ";\n";
    else if(id == "active_tab_uses_body_face") out << "\t" << var << ".active_tab_uses_body_face = " << AsString((bool)v) << ";\n";
    else if(id == "visual") out << "\t" << var << ".visual = " << (AsString(v) == "Underline" ? "UITAB_UNDERLINE" : AsString(v) == "Segmented" ? "UITAB_SEGMENTED" : AsString(v) == "Rail" ? "UITAB_RAIL" : AsString(v) == "Document" ? "UITAB_DOCUMENT" : "UITAB_CLASSIC") << ";\n";
    else return false;
    return true;
}

class TabThemeAdapterV2 final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "tab"; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return kind == UiDesignerRuntimeKind::UiTab;
    }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        AddTabOverrides(spec);
    }
    bool HasField(const String& raw_id) const override
    {
        const String id = TabCanonicalId(raw_id);
        return IsPaletteMetricsField("", id) || IsPaletteMetricsField("tab", id) ||
               IsTabExtra(id);
    }
    bool FieldAffectsLayout(const String& raw_id) const override
    {
        const String id = TabCanonicalId(raw_id);
        return PaletteMetricsAffectsLayout("", id) ||
               PaletteMetricsAffectsLayout("tab", id) || IsTabExtra(id);
    }
    Value ResolveFieldValue(const UiDesignerNode& node,
                            const UiDesignerControlSpec& spec,
                            const String& raw_id,
                            const UiDesignerTransientOverlay* overlay) const override
    {
        UiTab::Style s = UiTheme::ResolveTab(ContainerRole(node), UITAB_CLASSIC);
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) ApplyTabField(s, p.adapter_field_id, v);
        }
        return TabFieldValue(s, raw_id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node,
                           const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiTab *c = dynamic_cast<UiTab *>(&ctrl);
        if(!c) return;
        UiTab::Style s = UiTheme::ResolveTab(ContainerRole(node), c->GetVisual());
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false;
            Value v = Effective(node, p, overlay, active);
            if(active) {
                authored = true;
                ApplyTabField(s, p.adapter_field_id, v);
            }
        }
        if(authored || ContainerRole(node) != UiRole::Standard)
            c->SetCustomStyle(s);
        else
            c->ClearCustomStyle();
    }
    void EmitSetup(String& out, const String& member,
                   const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        if(!HasAuthored(node, spec) && ContainerRole(node) == UiRole::Standard)
            return;
        const String var = member + "_style";
        out << "\tUiTab::Style " << var << " = UiTheme::ResolveTab("
            << RoleExpr(node.GetProperty("role", "Standard"))
            << ", UITAB_CLASSIC);\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(p.id);
            if(q >= 0)
                EmitTabField(out, var, p.adapter_field_id,
                             node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

PanelThemeAdapterV2 s_panel_adapter;
GroupPanelThemeAdapterV2 s_group_panel_adapter;
ScrollPanelThemeAdapterV2 s_scroll_panel_adapter;
TabThemeAdapterV2 s_tab_adapter;

} // namespace

const UiDesignerThemeAdapter& UiDesignerPanelThemeAdapterInstance()
{
    return s_panel_adapter;
}

const UiDesignerThemeAdapter& UiDesignerGroupPanelThemeAdapterInstance()
{
    return s_group_panel_adapter;
}

const UiDesignerThemeAdapter& UiDesignerScrollPanelThemeAdapterInstance()
{
    return s_scroll_panel_adapter;
}

const UiDesignerThemeAdapter& UiDesignerTabThemeAdapterInstance()
{
    return s_tab_adapter;
}

} // namespace Upp
