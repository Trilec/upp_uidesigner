#include "UiDesignerStyledThemeCommon.h"
#include <Ui/UiCheckBox.h>
#include <Ui/UiRadioButton.h>
#include <Ui/UiToggle.h>
#include <Ui/UiProgressBar.h>
#include <Ui/UiSlider.h>
#include <Ui/UiScrollBar.h>
#include <Ui/UiTheme.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;
using namespace UiDesignerStyledTheme;

static void RemoveOverride(UiDesignerControlSpec& spec, const String& id)
{
    for(int i = spec.theme_overrides.GetCount() - 1; i >= 0; --i)
        if(spec.theme_overrides[i].id == id)
            spec.theme_overrides.Remove(i);
}

static bool HasAuthored(const UiDesignerNode& node,
                        const UiDesignerControlSpec& spec)
{
    for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides)
        if(node.theme_overrides.Find(p.id) >= 0)
            return true;
    return false;
}

static UiRole CoreRole(const UiDesignerNode& node)
{
    return Role(node.GetProperty("role", "Standard"));
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

// -----------------------------------------------------------------------------
// CheckBox

static void AddCheckOverrides(UiDesignerControlSpec& spec)
{
    const UiCheckBox::Style s = UiTheme::ResolveCheckBox(UiRole::Standard,
                                                         UICHECKVIS_CLASSIC);
    AddPaletteMetrics(spec, "", "", s.palette, s.metrics,
                      true, false, true, true, true, true);
    AddPaletteMetrics(spec, "indicator", "Indicator", s.indicator_palette,
                      s.indicator_metrics, true, true, true, true, true, true);

    Add(spec, "font_face", "Font face", "Typography", PropertyEditorKind::Text,
        s.font.GetFaceName(), true).Editor("property.font");
    AddNumeric(spec, "font_height", "Font height", "Typography",
               max(1, s.font.GetHeight()), 6, 128, true);
    Add(spec, "font_bold", "Bold", "Typography", PropertyEditorKind::Boolean,
        s.font.IsBold(), true);
    Add(spec, "font_italic", "Italic", "Typography", PropertyEditorKind::Boolean,
        s.font.IsItalic(), true);

    AddAlignChoice(spec, "align_h", "Horizontal align", "Appearance", s.align_h);
    AddAlignChoice(spec, "align_v", "Vertical align", "Appearance", s.align_v);
    AddAlignChoice(spec, "indicator_side", "Indicator side", "Appearance",
                   s.indicator_side, true);
    AddNumeric(spec, "indicator_size", "Indicator size", "Appearance",
               s.indicator_size, 0, 256, true);
    AddNumeric(spec, "indicator_width", "Indicator width", "Appearance",
               s.indicator_extent.cx, 0, 256, true);
    AddNumeric(spec, "indicator_height", "Indicator height", "Appearance",
               s.indicator_extent.cy, 0, 256, true);
    AddNumeric(spec, "indicator_gap", "Indicator gap", "Appearance",
               s.indicator_gap, 0, 128, true);
    AddNumeric(spec, "mark_thickness", "Mark thickness", "Appearance",
               s.mark_thickness, 1, 32, true);
    AddIconMode(spec, "marker_render_mode", "Marker render mode", "Appearance",
                s.marker_render_mode);
    // checked_icon / tri_state_icon are intentionally deferred until theme
    // adapters share the Designer document resource resolver.
}

static bool ApplyCheckField(UiCheckBox::Style& s, const String& id, const Value& v)
{
    if(ApplyPaletteMetrics(s.palette, s.metrics, "", id, v)) return true;
    if(ApplyPaletteMetrics(s.indicator_palette, s.indicator_metrics,
                           "indicator", id, v)) return true;
    if(id == "font_face") s.font.FaceName(AsString(v));
    else if(id == "font_height") s.font.Height(max(1, (int)v));
    else if(id == "font_bold") s.font.Bold((bool)v);
    else if(id == "font_italic") s.font.Italic((bool)v);
    else if(id == "align_h") s.align_h = AlignFromValue(v, s.align_h);
    else if(id == "align_v") s.align_v = AlignFromValue(v, s.align_v);
    else if(id == "indicator_side") s.indicator_side = AlignFromValue(v, s.indicator_side);
    else if(id == "indicator_size") s.indicator_size = max(0, (int)v);
    else if(id == "indicator_width") s.indicator_extent.cx = max(0, (int)v);
    else if(id == "indicator_height") s.indicator_extent.cy = max(0, (int)v);
    else if(id == "indicator_gap") s.indicator_gap = max(0, (int)v);
    else if(id == "mark_thickness") s.mark_thickness = max(1, (int)v);
    else if(id == "marker_render_mode") s.marker_render_mode = IconModeFromValue(v);
    else return false;
    return true;
}

static Value CheckFieldValue(const UiCheckBox::Style& s, const String& id)
{
    if(IsPaletteMetricsField("", id)) return PaletteMetricsValue(s.palette, s.metrics, "", id);
    if(IsPaletteMetricsField("indicator", id)) return PaletteMetricsValue(s.indicator_palette, s.indicator_metrics, "indicator", id);
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_height") return s.font.GetHeight();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    if(id == "align_h") return AlignName(s.align_h);
    if(id == "align_v") return AlignName(s.align_v);
    if(id == "indicator_side") return AlignName(s.indicator_side);
    if(id == "indicator_size") return s.indicator_size;
    if(id == "indicator_width") return s.indicator_extent.cx;
    if(id == "indicator_height") return s.indicator_extent.cy;
    if(id == "indicator_gap") return s.indicator_gap;
    if(id == "mark_thickness") return s.mark_thickness;
    if(id == "marker_render_mode") return IconModeName(s.marker_render_mode);
    return Value();
}

static bool EmitCheckField(String& out, const String& var,
                           const String& id, const Value& v)
{
    if(EmitPaletteMetrics(out, var + ".palette", var + ".metrics", "", id, v)) return true;
    if(EmitPaletteMetrics(out, var + ".indicator_palette", var + ".indicator_metrics", "indicator", id, v)) return true;
    if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "font_height") out << "\t" << var << ".font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << AsString((bool)v) << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << AsString((bool)v) << ");\n";
    else if(id == "align_h") out << "\t" << var << ".align_h = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "align_v") out << "\t" << var << ".align_v = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "indicator_side") out << "\t" << var << ".indicator_side = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "indicator_size") out << "\t" << var << ".indicator_size = " << max(0, (int)v) << ";\n";
    else if(id == "indicator_width") out << "\t" << var << ".indicator_extent.cx = " << max(0, (int)v) << ";\n";
    else if(id == "indicator_height") out << "\t" << var << ".indicator_extent.cy = " << max(0, (int)v) << ";\n";
    else if(id == "indicator_gap") out << "\t" << var << ".indicator_gap = " << max(0, (int)v) << ";\n";
    else if(id == "mark_thickness") out << "\t" << var << ".mark_thickness = " << max(1, (int)v) << ";\n";
    else if(id == "marker_render_mode") out << "\t" << var << ".marker_render_mode = " << IconModeCode(IconModeFromValue(v)) << ";\n";
    else return false;
    return true;
}

class CheckThemeAdapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "check"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == UiDesignerRuntimeKind::UiCheckBox; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override { AddCheckOverrides(spec); }
    bool HasField(const String& id) const override
    {
        UiCheckBox::Style s;
        return IsPaletteMetricsField("", id) || IsPaletteMetricsField("indicator", id) || !IsNull(CheckFieldValue(s, id));
    }
    bool FieldAffectsLayout(const String& id) const override
    {
        return PaletteMetricsAffectsLayout("", id) || PaletteMetricsAffectsLayout("indicator", id) ||
               id == "font_face" || id == "font_height" || id == "font_bold" || id == "font_italic" ||
               id == "align_h" || id == "align_v" || id == "indicator_side" ||
               id == "indicator_size" || id == "indicator_width" || id == "indicator_height" ||
               id == "indicator_gap" || id == "mark_thickness";
    }
    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        UiCheckBox::Style s = UiTheme::ResolveCheckBox(CoreRole(node), UICHECKVIS_CLASSIC);
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false; Value v = Effective(node, p, overlay, active);
            if(active) ApplyCheckField(s, p.adapter_field_id, v);
        }
        return CheckFieldValue(s, id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        UiCheckBox *c = dynamic_cast<UiCheckBox *>(&ctrl); if(!c) return;
        UiCheckBox::Style s = UiTheme::ResolveCheckBox(CoreRole(node), UICHECKVIS_CLASSIC);
        bool authored = false;
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            bool active = false; Value v = Effective(node, p, overlay, active);
            if(active) { authored = true; ApplyCheckField(s, p.adapter_field_id, v); }
        }
        if(authored || CoreRole(node) != UiRole::Standard) c->SetCustomStyle(s); else c->ClearCustomStyle();
    }
    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        if(!HasAuthored(node, spec) && CoreRole(node) == UiRole::Standard) return;
        const String var = member + "_style";
        out << "\tUiCheckBox::Style " << var << " = UiTheme::ResolveCheckBox("
            << RoleExpr(node.GetProperty("role", "Standard")) << ", UICHECKVIS_CLASSIC);\n";
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) {
            int q = node.theme_overrides.Find(p.id); if(q < 0) continue;
            EmitCheckField(out, var, p.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};

// -----------------------------------------------------------------------------
// RadioButton

static void AddRadioOverrides(UiDesignerControlSpec& spec)
{
    const UiRadioButton::Style s = UiTheme::ResolveRadioButton(UiRole::Standard,
                                                               UIRADIOVIS_CLASSIC);
    AddPaletteMetrics(spec, "", "", s.palette, s.metrics, true, false, true, true, true, true);
    AddPaletteMetrics(spec, "indicator", "Indicator", s.indicator_palette,
                      s.indicator_metrics, true, true, true, true, true, true);
    Add(spec, "font_face", "Font face", "Typography", PropertyEditorKind::Text,
        s.font.GetFaceName(), true).Editor("property.font");
    AddNumeric(spec, "font_height", "Font height", "Typography", max(1, s.font.GetHeight()), 6, 128, true);
    Add(spec, "font_bold", "Bold", "Typography", PropertyEditorKind::Boolean, s.font.IsBold(), true);
    Add(spec, "font_italic", "Italic", "Typography", PropertyEditorKind::Boolean, s.font.IsItalic(), true);
    AddAlignChoice(spec, "indicator_side", "Indicator side", "Appearance", s.indicator_side, true);
    AddNumeric(spec, "indicator_size", "Indicator size", "Appearance", s.indicator_size, 0, 256, true);
    AddNumeric(spec, "indicator_gap", "Indicator gap", "Appearance", s.indicator_gap, 0, 128, true);
}

static bool ApplyRadioField(UiRadioButton::Style& s, const String& id, const Value& v)
{
    if(ApplyPaletteMetrics(s.palette, s.metrics, "", id, v)) return true;
    if(ApplyPaletteMetrics(s.indicator_palette, s.indicator_metrics, "indicator", id, v)) return true;
    if(id == "font_face") s.font.FaceName(AsString(v));
    else if(id == "font_height") s.font.Height(max(1, (int)v));
    else if(id == "font_bold") s.font.Bold((bool)v);
    else if(id == "font_italic") s.font.Italic((bool)v);
    else if(id == "indicator_side") s.indicator_side = AlignFromValue(v, s.indicator_side);
    else if(id == "indicator_size") s.indicator_size = max(0, (int)v);
    else if(id == "indicator_gap") s.indicator_gap = max(0, (int)v);
    else return false;
    return true;
}

static Value RadioFieldValue(const UiRadioButton::Style& s, const String& id)
{
    if(IsPaletteMetricsField("", id)) return PaletteMetricsValue(s.palette, s.metrics, "", id);
    if(IsPaletteMetricsField("indicator", id)) return PaletteMetricsValue(s.indicator_palette, s.indicator_metrics, "indicator", id);
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_height") return s.font.GetHeight();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    if(id == "indicator_side") return AlignName(s.indicator_side);
    if(id == "indicator_size") return s.indicator_size;
    if(id == "indicator_gap") return s.indicator_gap;
    return Value();
}

static bool EmitRadioField(String& out, const String& var, const String& id, const Value& v)
{
    if(EmitPaletteMetrics(out, var + ".palette", var + ".metrics", "", id, v)) return true;
    if(EmitPaletteMetrics(out, var + ".indicator_palette", var + ".indicator_metrics", "indicator", id, v)) return true;
    if(id == "font_face") out << "\t" << var << ".font.FaceName(" << CppString(AsString(v)) << ");\n";
    else if(id == "font_height") out << "\t" << var << ".font.Height(" << max(1, (int)v) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << AsString((bool)v) << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << AsString((bool)v) << ");\n";
    else if(id == "indicator_side") out << "\t" << var << ".indicator_side = " << AlignCode(AlignFromValue(v)) << ";\n";
    else if(id == "indicator_size") out << "\t" << var << ".indicator_size = " << max(0, (int)v) << ";\n";
    else if(id == "indicator_gap") out << "\t" << var << ".indicator_gap = " << max(0, (int)v) << ";\n";
    else return false;
    return true;
}

class RadioThemeAdapter final : public UiDesignerThemeAdapter {
public:
    const char *Id() const override { return "radio"; }
    bool Supports(UiDesignerRuntimeKind kind) const override { return kind == UiDesignerRuntimeKind::UiRadioButton; }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override { AddRadioOverrides(spec); }
    bool HasField(const String& id) const override { UiRadioButton::Style s; return IsPaletteMetricsField("", id) || IsPaletteMetricsField("indicator", id) || !IsNull(RadioFieldValue(s, id)); }
    bool FieldAffectsLayout(const String& id) const override { return PaletteMetricsAffectsLayout("", id) || PaletteMetricsAffectsLayout("indicator", id) || id.StartsWith("font_") || id == "indicator_side" || id == "indicator_size" || id == "indicator_gap"; }
    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec, const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        UiRadioButton::Style s = UiTheme::ResolveRadioButton(CoreRole(node), UIRADIOVIS_CLASSIC);
        for(const UiDesignerThemeOverrideSpec& p : spec.theme_overrides) { bool active=false; Value v=Effective(node,p,overlay,active); if(active) ApplyRadioField(s,p.adapter_field_id,v); }
        return RadioFieldValue(s,id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node, const UiDesignerControlSpec& spec, const UiDesignerTransientOverlay* overlay) const override
    {
        UiRadioButton *c=dynamic_cast<UiRadioButton*>(&ctrl); if(!c) return; UiRadioButton::Style s=UiTheme::ResolveRadioButton(CoreRole(node),UIRADIOVIS_CLASSIC); bool authored=false;
        for(const UiDesignerThemeOverrideSpec& p:spec.theme_overrides){bool active=false;Value v=Effective(node,p,overlay,active);if(active){authored=true;ApplyRadioField(s,p.adapter_field_id,v);}}
        if(authored||CoreRole(node)!=UiRole::Standard)c->SetCustomStyle(s);else c->ClearCustomStyle();
    }
    void EmitSetup(String& out,const String& member,const UiDesignerNode& node,const UiDesignerControlSpec& spec) const override
    {
        if(!HasAuthored(node,spec)&&CoreRole(node)==UiRole::Standard)return;String var=member+"_style";out<<"\tUiRadioButton::Style "<<var<<" = UiTheme::ResolveRadioButton("<<RoleExpr(node.GetProperty("role","Standard"))<<", UIRADIOVIS_CLASSIC);\n";
        for(const UiDesignerThemeOverrideSpec& p:spec.theme_overrides){int q=node.theme_overrides.Find(p.id);if(q>=0)EmitRadioField(out,var,p.adapter_field_id,node.theme_overrides.GetValue(q));}out<<"\t"<<member<<".SetCustomStyle("<<var<<");\n";
    }
};

// -----------------------------------------------------------------------------
// Toggle

static String DirectionName(UiDirection d) { return d == UiDirection::V ? "Vertical" : "Horizontal"; }
static UiDirection DirectionValue(const Value& v) { return AsString(v) == "Vertical" ? UiDirection::V : UiDirection::H; }
static String DirectionCode(UiDirection d) { return d == UiDirection::V ? "UiDirection::V" : "UiDirection::H"; }

static void AddToggleOverrides(UiDesignerControlSpec& spec)
{
    const UiToggle::Style s = UiTheme::ResolveToggle();
    AddPaletteMetrics(spec,"","",s.palette,s.metrics,true,false,true,true,true,true);
    AddPaletteMetrics(spec,"track","Track",s.track_palette,s.track_metrics,true,false,true,true,true,true);
    AddPaletteMetrics(spec,"thumb","Thumb",s.thumb_palette,s.thumb_metrics,true,false,true,true,true,true);
    Add(spec,"direction","Direction","Appearance",PropertyEditorKind::Choice,DirectionName(s.direction),true).Choice("Horizontal","Horizontal").Choice("Vertical","Vertical");
    AddAlignChoice(spec,"align_h","Horizontal align","Appearance",s.align_h);
    AddAlignChoice(spec,"align_v","Vertical align","Appearance",s.align_v);
    AddAlignChoice(spec,"track_side","Track side","Appearance",s.track_side,true);
    AddNumeric(spec,"track_width","Track width","Appearance",s.track_size.cx,1,512,true);
    AddNumeric(spec,"track_height","Track height","Appearance",s.track_size.cy,1,512,true);
    AddNumeric(spec,"thumb_width","Thumb width","Appearance",s.thumb_size.cx,1,512,true);
    AddNumeric(spec,"thumb_height","Thumb height","Appearance",s.thumb_size.cy,1,512,true);
    AddNumeric(spec,"thumb_inset","Thumb inset","Appearance",s.thumb_inset,0,128,true);
    Add(spec,"animate","Enabled","Animation",PropertyEditorKind::Boolean,s.animate);
    AddNumeric(spec,"animation_ms","Duration (ms)","Animation",s.animation_ms,0,5000);
}

static bool ApplyToggleField(UiToggle::Style& s,const String& id,const Value& v)
{
    if(ApplyPaletteMetrics(s.palette,s.metrics,"",id,v))return true;
    if(ApplyPaletteMetrics(s.track_palette,s.track_metrics,"track",id,v))return true;
    if(ApplyPaletteMetrics(s.thumb_palette,s.thumb_metrics,"thumb",id,v))return true;
    if(id=="direction")s.direction=DirectionValue(v);else if(id=="align_h")s.align_h=AlignFromValue(v,s.align_h);else if(id=="align_v")s.align_v=AlignFromValue(v,s.align_v);else if(id=="track_side")s.track_side=AlignFromValue(v,s.track_side);else if(id=="track_width")s.track_size.cx=max(1,(int)v);else if(id=="track_height")s.track_size.cy=max(1,(int)v);else if(id=="thumb_width")s.thumb_size.cx=max(1,(int)v);else if(id=="thumb_height")s.thumb_size.cy=max(1,(int)v);else if(id=="thumb_inset")s.thumb_inset=max(0,(int)v);else if(id=="animate")s.animate=(bool)v;else if(id=="animation_ms")s.animation_ms=max(0,(int)v);else return false;return true;
}

static Value ToggleFieldValue(const UiToggle::Style& s,const String& id)
{
    if(IsPaletteMetricsField("",id))return PaletteMetricsValue(s.palette,s.metrics,"",id);if(IsPaletteMetricsField("track",id))return PaletteMetricsValue(s.track_palette,s.track_metrics,"track",id);if(IsPaletteMetricsField("thumb",id))return PaletteMetricsValue(s.thumb_palette,s.thumb_metrics,"thumb",id);
    if(id=="direction")return DirectionName(s.direction);if(id=="align_h")return AlignName(s.align_h);if(id=="align_v")return AlignName(s.align_v);if(id=="track_side")return AlignName(s.track_side);if(id=="track_width")return s.track_size.cx;if(id=="track_height")return s.track_size.cy;if(id=="thumb_width")return s.thumb_size.cx;if(id=="thumb_height")return s.thumb_size.cy;if(id=="thumb_inset")return s.thumb_inset;if(id=="animate")return s.animate;if(id=="animation_ms")return s.animation_ms;return Value();
}

static bool EmitToggleField(String& out,const String& var,const String& id,const Value& v)
{
    if(EmitPaletteMetrics(out,var+".palette",var+".metrics","",id,v))return true;if(EmitPaletteMetrics(out,var+".track_palette",var+".track_metrics","track",id,v))return true;if(EmitPaletteMetrics(out,var+".thumb_palette",var+".thumb_metrics","thumb",id,v))return true;
    if(id=="direction")out<<"\t"<<var<<".direction = "<<DirectionCode(DirectionValue(v))<<";\n";else if(id=="align_h")out<<"\t"<<var<<".align_h = "<<AlignCode(AlignFromValue(v))<<";\n";else if(id=="align_v")out<<"\t"<<var<<".align_v = "<<AlignCode(AlignFromValue(v))<<";\n";else if(id=="track_side")out<<"\t"<<var<<".track_side = "<<AlignCode(AlignFromValue(v))<<";\n";else if(id=="track_width")out<<"\t"<<var<<".track_size.cx = "<<max(1,(int)v)<<";\n";else if(id=="track_height")out<<"\t"<<var<<".track_size.cy = "<<max(1,(int)v)<<";\n";else if(id=="thumb_width")out<<"\t"<<var<<".thumb_size.cx = "<<max(1,(int)v)<<";\n";else if(id=="thumb_height")out<<"\t"<<var<<".thumb_size.cy = "<<max(1,(int)v)<<";\n";else if(id=="thumb_inset")out<<"\t"<<var<<".thumb_inset = "<<max(0,(int)v)<<";\n";else if(id=="animate")out<<"\t"<<var<<".animate = "<<AsString((bool)v)<<";\n";else if(id=="animation_ms")out<<"\t"<<var<<".animation_ms = "<<max(0,(int)v)<<";\n";else return false;return true;
}

class ToggleThemeAdapter final:public UiDesignerThemeAdapter{
public:const char*Id()const override{return"toggle";}bool Supports(UiDesignerRuntimeKind k)const override{return k==UiDesignerRuntimeKind::UiToggle;}void AddThemeOverrides(UiDesignerControlSpec&s)const override{AddToggleOverrides(s);}bool HasField(const String&id)const override{UiToggle::Style s;return IsPaletteMetricsField("",id)||IsPaletteMetricsField("track",id)||IsPaletteMetricsField("thumb",id)||!IsNull(ToggleFieldValue(s,id));}bool FieldAffectsLayout(const String&id)const override{return PaletteMetricsAffectsLayout("",id)||PaletteMetricsAffectsLayout("track",id)||PaletteMetricsAffectsLayout("thumb",id)||id=="direction"||id=="align_h"||id=="align_v"||id=="track_side"||id=="track_width"||id=="track_height"||id=="thumb_width"||id=="thumb_height"||id=="thumb_inset";}
Value ResolveFieldValue(const UiDesignerNode&n,const UiDesignerControlSpec&sp,const String&id,const UiDesignerTransientOverlay*o)const override{UiToggle::Style s=UiTheme::ResolveToggle();for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a)ApplyToggleField(s,p.adapter_field_id,v);}return ToggleFieldValue(s,id);}void ApplyPreviewStyle(Ctrl&ctrl,const UiDesignerNode&n,const UiDesignerControlSpec&sp,const UiDesignerTransientOverlay*o)const override{auto*c=dynamic_cast<UiToggle*>(&ctrl);if(!c)return;UiToggle::Style s=UiTheme::ResolveToggle();bool authored=false;for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a){authored=true;ApplyToggleField(s,p.adapter_field_id,v);}}if(authored)c->SetCustomStyle(s);else c->ClearCustomStyle();}void EmitSetup(String&out,const String&m,const UiDesignerNode&n,const UiDesignerControlSpec&sp)const override{if(!HasAuthored(n,sp))return;String v=m+"_style";out<<"\tUiToggle::Style "<<v<<" = UiTheme::ResolveToggle();\n";for(const auto&p:sp.theme_overrides){int q=n.theme_overrides.Find(p.id);if(q>=0)EmitToggleField(out,v,p.adapter_field_id,n.theme_overrides.GetValue(q));}out<<"\t"<<m<<".SetCustomStyle("<<v<<");\n";}};

// -----------------------------------------------------------------------------
// ProgressBar

static void AddProgressOverrides(UiDesignerControlSpec& spec)
{
    const UiProgressBar::Style s=UiTheme::ResolveProgressBar(UiRole::Standard);
    AddPaletteMetrics(spec,"","Track",s.track_palette,s.track_metrics,false,false,true,true,true,true);
    AddPaletteMetrics(spec,"fill","Fill",s.fill_palette,s.fill_metrics,false,false,true,true,true,true);
    Add(spec,"ink_normal","Normal","Fill / Text Ink",PropertyEditorKind::Color,s.fill_palette.ink[ST_NORMAL]);
    Add(spec,"fill_ink_hot","Hot","Fill / Text Ink",PropertyEditorKind::Color,s.fill_palette.ink[ST_HOT]);
    Add(spec,"fill_ink_pressed","Pressed","Fill / Text Ink",PropertyEditorKind::Color,s.fill_palette.ink[ST_PRESSED]);
    Add(spec,"fill_ink_disabled","Disabled","Fill / Text Ink",PropertyEditorKind::Color,s.fill_palette.ink[ST_DISABLED]);
    Add(spec,"font_face","Font face","Typography",PropertyEditorKind::Text,s.font.GetFaceName(),true).Editor("property.font");
    AddNumeric(spec,"font_height","Font height","Typography",max(1,s.font.GetHeight()),6,128,true);
    Add(spec,"font_bold","Bold","Typography",PropertyEditorKind::Boolean,s.font.IsBold(),true);
    Add(spec,"font_italic","Italic","Typography",PropertyEditorKind::Boolean,s.font.IsItalic(),true);
}

static bool ApplyProgressField(UiProgressBar::Style&s,const String&id,const Value&v){if(ApplyPaletteMetrics(s.track_palette,s.track_metrics,"",id,v))return true;if(ApplyPaletteMetrics(s.fill_palette,s.fill_metrics,"fill",id,v))return true;if(id=="ink_normal")s.fill_palette.ink[ST_NORMAL]=(Color)v;else if(id=="fill_ink_hot")s.fill_palette.ink[ST_HOT]=(Color)v;else if(id=="fill_ink_pressed")s.fill_palette.ink[ST_PRESSED]=(Color)v;else if(id=="fill_ink_disabled")s.fill_palette.ink[ST_DISABLED]=(Color)v;else if(id=="font_face")s.font.FaceName(AsString(v));else if(id=="font_height")s.font.Height(max(1,(int)v));else if(id=="font_bold")s.font.Bold((bool)v);else if(id=="font_italic")s.font.Italic((bool)v);else return false;return true;}
static Value ProgressFieldValue(const UiProgressBar::Style&s,const String&id){if(IsPaletteMetricsField("",id))return PaletteMetricsValue(s.track_palette,s.track_metrics,"",id);if(IsPaletteMetricsField("fill",id))return PaletteMetricsValue(s.fill_palette,s.fill_metrics,"fill",id);if(id=="ink_normal")return s.fill_palette.ink[ST_NORMAL];if(id=="fill_ink_hot")return s.fill_palette.ink[ST_HOT];if(id=="fill_ink_pressed")return s.fill_palette.ink[ST_PRESSED];if(id=="fill_ink_disabled")return s.fill_palette.ink[ST_DISABLED];if(id=="font_face")return s.font.GetFaceName();if(id=="font_height")return s.font.GetHeight();if(id=="font_bold")return s.font.IsBold();if(id=="font_italic")return s.font.IsItalic();return Value();}
static bool EmitProgressField(String&out,const String&var,const String&id,const Value&v){if(EmitPaletteMetrics(out,var+".track_palette",var+".track_metrics","",id,v))return true;if(EmitPaletteMetrics(out,var+".fill_palette",var+".fill_metrics","fill",id,v))return true;if(id=="ink_normal")out<<"\t"<<var<<".fill_palette.ink[ST_NORMAL] = "<<EmitValue(v)<<";\n";else if(id=="fill_ink_hot")out<<"\t"<<var<<".fill_palette.ink[ST_HOT] = "<<EmitValue(v)<<";\n";else if(id=="fill_ink_pressed")out<<"\t"<<var<<".fill_palette.ink[ST_PRESSED] = "<<EmitValue(v)<<";\n";else if(id=="fill_ink_disabled")out<<"\t"<<var<<".fill_palette.ink[ST_DISABLED] = "<<EmitValue(v)<<";\n";else if(id=="font_face")out<<"\t"<<var<<".font.FaceName("<<CppString(AsString(v))<<");\n";else if(id=="font_height")out<<"\t"<<var<<".font.Height("<<max(1,(int)v)<<");\n";else if(id=="font_bold")out<<"\t"<<var<<".font.Bold("<<AsString((bool)v)<<");\n";else if(id=="font_italic")out<<"\t"<<var<<".font.Italic("<<AsString((bool)v)<<");\n";else return false;return true;}
class ProgressThemeAdapter final:public UiDesignerThemeAdapter{public:const char*Id()const override{return"progress";}bool Supports(UiDesignerRuntimeKind k)const override{return k==UiDesignerRuntimeKind::UiProgressBar;}void AddThemeOverrides(UiDesignerControlSpec&s)const override{AddProgressOverrides(s);}bool HasField(const String&id)const override{UiProgressBar::Style s;return IsPaletteMetricsField("",id)||IsPaletteMetricsField("fill",id)||!IsNull(ProgressFieldValue(s,id));}bool FieldAffectsLayout(const String&id)const override{return PaletteMetricsAffectsLayout("",id)||PaletteMetricsAffectsLayout("fill",id)||id.StartsWith("font_");}Value ResolveFieldValue(const UiDesignerNode&n,const UiDesignerControlSpec&sp,const String&id,const UiDesignerTransientOverlay*o)const override{UiProgressBar::Style s=UiTheme::ResolveProgressBar(CoreRole(n));for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a)ApplyProgressField(s,p.adapter_field_id,v);}return ProgressFieldValue(s,id);}void ApplyPreviewStyle(Ctrl&ctrl,const UiDesignerNode&n,const UiDesignerControlSpec&sp,const UiDesignerTransientOverlay*o)const override{auto*c=dynamic_cast<UiProgressBar*>(&ctrl);if(!c)return;UiProgressBar::Style s=UiTheme::ResolveProgressBar(CoreRole(n));bool authored=false;for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a){authored=true;ApplyProgressField(s,p.adapter_field_id,v);}}if(authored||CoreRole(n)!=UiRole::Standard)c->SetCustomStyle(s);else c->ClearCustomStyle();}void EmitSetup(String&out,const String&m,const UiDesignerNode&n,const UiDesignerControlSpec&sp)const override{if(!HasAuthored(n,sp)&&CoreRole(n)==UiRole::Standard)return;String var=m+"_style";out<<"\tUiProgressBar::Style "<<var<<" = UiTheme::ResolveProgressBar("<<RoleExpr(n.GetProperty("role","Standard"))<<");\n";for(const auto&p:sp.theme_overrides){int q=n.theme_overrides.Find(p.id);if(q>=0)EmitProgressField(out,var,p.adapter_field_id,n.theme_overrides.GetValue(q));}out<<"\t"<<m<<".SetCustomStyle("<<var<<");\n";}};

// -----------------------------------------------------------------------------
// Slider

static void AddSliderOverrides(UiDesignerControlSpec&spec){const UiSlider::Style s=UiTheme::ResolveSlider();AddPaletteMetrics(spec,"","Track",s.track_palette,s.track_metrics,false,false,true,true,true,true);AddPaletteMetrics(spec,"thumb","Thumb",s.thumb_palette,s.thumb_metrics,false,false,true,true,true,true);RemoveOverride(spec,"thumb_face_normal");Add(spec,"ink_normal","Normal","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_NORMAL]).ToValue());Add(spec,"thumb_face_hot","Hot","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_HOT]).ToValue());Add(spec,"thumb_face_pressed","Pressed","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_PRESSED]).ToValue());Add(spec,"thumb_face_disabled","Disabled","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_DISABLED]).ToValue());Add(spec,"show_ticks","Show ticks","Ticks",PropertyEditorKind::Boolean,s.show_ticks,true);AddNumeric(spec,"major_ticks","Major ticks","Ticks",s.major_ticks,0,100,true);AddNumeric(spec,"minor_ticks_per_major","Minor per major","Ticks",s.minor_ticks_per_major,0,20,true);AddNumeric(spec,"tick_len_major","Major length","Ticks",s.tick_len_major,0,64,true);AddNumeric(spec,"tick_len_minor","Minor length","Ticks",s.tick_len_minor,0,64,true);AddNumeric(spec,"tick_gap","Gap","Ticks",s.tick_gap,0,64,true);Add(spec,"tick_color","Colour","Ticks",PropertyEditorKind::Color,s.tick_color);AddAlignChoice(spec,"tick_side","Side","Ticks",s.tick_side,true);AddNumeric(spec,"track_width","Track major length","Appearance",s.track_size.cx,1,2048,true);AddNumeric(spec,"track_height","Track thickness","Appearance",s.track_size.cy,1,256,true);AddNumeric(spec,"thumb_width","Thumb width","Appearance",s.thumb_size.cx,1,512,true);AddNumeric(spec,"thumb_height","Thumb height","Appearance",s.thumb_size.cy,1,512,true);Add(spec,"thumb_inner_ring","Inner ring","Thumb / Additional",PropertyEditorKind::Boolean,s.thumb_inner_ring);AddNumeric(spec,"thumb_inner_ring_width","Ring width","Thumb / Additional",s.thumb_inner_ring_width,0,32);Add(spec,"thumb_inner_ring_color","Ring colour","Thumb / Additional",PropertyEditorKind::Color,s.thumb_inner_ring_color);}
static bool ApplySliderField(UiSlider::Style&s,const String&id,const Value&v){if(ApplyPaletteMetrics(s.track_palette,s.track_metrics,"",id,v))return true;if(ApplyPaletteMetrics(s.thumb_palette,s.thumb_metrics,"thumb",id,v))return true;if(id=="ink_normal")ApplyFill(s.thumb_palette.face[ST_NORMAL],v);else if(id=="show_ticks")s.show_ticks=(bool)v;else if(id=="major_ticks")s.major_ticks=max(0,(int)v);else if(id=="minor_ticks_per_major")s.minor_ticks_per_major=max(0,(int)v);else if(id=="tick_len_major")s.tick_len_major=max(0,(int)v);else if(id=="tick_len_minor")s.tick_len_minor=max(0,(int)v);else if(id=="tick_gap")s.tick_gap=max(0,(int)v);else if(id=="tick_color")s.tick_color=(Color)v;else if(id=="tick_side")s.tick_side=AlignFromValue(v,s.tick_side);else if(id=="track_width")s.track_size.cx=max(1,(int)v);else if(id=="track_height")s.track_size.cy=max(1,(int)v);else if(id=="thumb_width")s.thumb_size.cx=max(1,(int)v);else if(id=="thumb_height")s.thumb_size.cy=max(1,(int)v);else if(id=="thumb_inner_ring")s.thumb_inner_ring=(bool)v;else if(id=="thumb_inner_ring_width")s.thumb_inner_ring_width=max(0,(int)v);else if(id=="thumb_inner_ring_color")s.thumb_inner_ring_color=(Color)v;else return false;return true;}
static Value SliderFieldValue(const UiSlider::Style&s,const String&id){if(IsPaletteMetricsField("",id))return PaletteMetricsValue(s.track_palette,s.track_metrics,"",id);if(IsPaletteMetricsField("thumb",id))return PaletteMetricsValue(s.thumb_palette,s.thumb_metrics,"thumb",id);if(id=="ink_normal")return FillRecipe(s.thumb_palette.face[ST_NORMAL]).ToValue();if(id=="show_ticks")return s.show_ticks;if(id=="major_ticks")return s.major_ticks;if(id=="minor_ticks_per_major")return s.minor_ticks_per_major;if(id=="tick_len_major")return s.tick_len_major;if(id=="tick_len_minor")return s.tick_len_minor;if(id=="tick_gap")return s.tick_gap;if(id=="tick_color")return s.tick_color;if(id=="tick_side")return AlignName(s.tick_side);if(id=="track_width")return s.track_size.cx;if(id=="track_height")return s.track_size.cy;if(id=="thumb_width")return s.thumb_size.cx;if(id=="thumb_height")return s.thumb_size.cy;if(id=="thumb_inner_ring")return s.thumb_inner_ring;if(id=="thumb_inner_ring_width")return s.thumb_inner_ring_width;if(id=="thumb_inner_ring_color")return s.thumb_inner_ring_color;return Value();}
static bool EmitSliderField(String&out,const String&var,const String&id,const Value&v){if(EmitPaletteMetrics(out,var+".track_palette",var+".track_metrics","",id,v))return true;if(EmitPaletteMetrics(out,var+".thumb_palette",var+".thumb_metrics","thumb",id,v))return true;if(id=="ink_normal")out<<"\t"<<var<<".thumb_palette.face[ST_NORMAL] = "<<FillCode(v)<<";\n";else if(id=="show_ticks")out<<"\t"<<var<<".show_ticks = "<<AsString((bool)v)<<";\n";else if(id=="major_ticks")out<<"\t"<<var<<".major_ticks = "<<max(0,(int)v)<<";\n";else if(id=="minor_ticks_per_major")out<<"\t"<<var<<".minor_ticks_per_major = "<<max(0,(int)v)<<";\n";else if(id=="tick_len_major")out<<"\t"<<var<<".tick_len_major = "<<max(0,(int)v)<<";\n";else if(id=="tick_len_minor")out<<"\t"<<var<<".tick_len_minor = "<<max(0,(int)v)<<";\n";else if(id=="tick_gap")out<<"\t"<<var<<".tick_gap = "<<max(0,(int)v)<<";\n";else if(id=="tick_color")out<<"\t"<<var<<".tick_color = "<<EmitValue(v)<<";\n";else if(id=="tick_side")out<<"\t"<<var<<".tick_side = "<<AlignCode(AlignFromValue(v))<<";\n";else if(id=="track_width")out<<"\t"<<var<<".track_size.cx = "<<max(1,(int)v)<<";\n";else if(id=="track_height")out<<"\t"<<var<<".track_size.cy = "<<max(1,(int)v)<<";\n";else if(id=="thumb_width")out<<"\t"<<var<<".thumb_size.cx = "<<max(1,(int)v)<<";\n";else if(id=="thumb_height")out<<"\t"<<var<<".thumb_size.cy = "<<max(1,(int)v)<<";\n";else if(id=="thumb_inner_ring")out<<"\t"<<var<<".thumb_inner_ring = "<<AsString((bool)v)<<";\n";else if(id=="thumb_inner_ring_width")out<<"\t"<<var<<".thumb_inner_ring_width = "<<max(0,(int)v)<<";\n";else if(id=="thumb_inner_ring_color")out<<"\t"<<var<<".thumb_inner_ring_color = "<<EmitValue(v)<<";\n";else return false;return true;}
class SliderThemeAdapter final:public UiDesignerThemeAdapter{public:const char*Id()const override{return"slider";}bool Supports(UiDesignerRuntimeKind k)const override{return k==UiDesignerRuntimeKind::UiSlider;}void AddThemeOverrides(UiDesignerControlSpec&s)const override{AddSliderOverrides(s);}bool HasField(const String&id)const override{UiSlider::Style s;return IsPaletteMetricsField("",id)||IsPaletteMetricsField("thumb",id)||!IsNull(SliderFieldValue(s,id));}bool FieldAffectsLayout(const String&id)const override{return PaletteMetricsAffectsLayout("",id)||PaletteMetricsAffectsLayout("thumb",id)||id.StartsWith("tick_")||id=="show_ticks"||id=="major_ticks"||id=="minor_ticks_per_major"||id=="track_width"||id=="track_height"||id=="thumb_width"||id=="thumb_height";}Value ResolveFieldValue(const UiDesignerNode&n,const UiDesignerControlSpec&sp,const String&id,const UiDesignerTransientOverlay*o)const override{UiSlider::Style s=UiTheme::ResolveSlider();for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a)ApplySliderField(s,p.adapter_field_id,v);}return SliderFieldValue(s,id);}void ApplyPreviewStyle(Ctrl&ctrl,const UiDesignerNode&n,const UiDesignerControlSpec&sp,const UiDesignerTransientOverlay*o)const override{auto*c=dynamic_cast<UiSlider*>(&ctrl);if(!c)return;UiSlider::Style s=UiTheme::ResolveSlider();bool authored=false;for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a){authored=true;ApplySliderField(s,p.adapter_field_id,v);}}if(authored)c->SetCustomStyle(s);else c->ClearCustomStyle();}void EmitSetup(String&out,const String&m,const UiDesignerNode&n,const UiDesignerControlSpec&sp)const override{if(!HasAuthored(n,sp))return;String var=m+"_style";out<<"\tUiSlider::Style "<<var<<" = UiTheme::ResolveSlider();\n";for(const auto&p:sp.theme_overrides){int q=n.theme_overrides.Find(p.id);if(q>=0)EmitSliderField(out,var,p.adapter_field_id,n.theme_overrides.GetValue(q));}out<<"\t"<<m<<".SetCustomStyle("<<var<<");\n";}};

// -----------------------------------------------------------------------------
// ScrollBar

static String ArrowsLayoutName(UiScrollArrowsLayout v){switch(v){case UIARROWS_NONE:return"None";case UIARROWS_GROUP_START:return"GroupStart";case UIARROWS_GROUP_END:return"GroupEnd";default:return"Split";}}
static UiScrollArrowsLayout ArrowsLayoutValue(const Value&v){String s=AsString(v);if(s=="None")return UIARROWS_NONE;if(s=="GroupStart")return UIARROWS_GROUP_START;if(s=="GroupEnd")return UIARROWS_GROUP_END;return UIARROWS_SPLIT;}
static String ArrowsLayoutCode(UiScrollArrowsLayout v){switch(v){case UIARROWS_NONE:return"UIARROWS_NONE";case UIARROWS_GROUP_START:return"UIARROWS_GROUP_START";case UIARROWS_GROUP_END:return"UIARROWS_GROUP_END";default:return"UIARROWS_SPLIT";}}
static String ArrowCrossName(UiScrollArrowCross v){return v==UIARROWCROSS_FILL?"Fill":"Square";}static UiScrollArrowCross ArrowCrossValue(const Value&v){return AsString(v)=="Fill"?UIARROWCROSS_FILL:UIARROWCROSS_SQUARE;}
static String ThumbLenName(UiScrollThumbLenMode v){return v==UITHUMB_FIXED?"Fixed":"Proportional";}static UiScrollThumbLenMode ThumbLenValue(const Value&v){return AsString(v)=="Fixed"?UITHUMB_FIXED:UITHUMB_PROPORTIONAL;}
static String GripName(UiScrollGrip v){switch(v){case UIGRIP_LINES:return"Lines";case UIGRIP_DOTS:return"Dots";case UIGRIP_SLOT:return"Slot";case UIGRIP_IMAGE:return"Image";default:return"None";}}static UiScrollGrip GripValue(const Value&v){String s=AsString(v);if(s=="Lines")return UIGRIP_LINES;if(s=="Dots")return UIGRIP_DOTS;if(s=="Slot")return UIGRIP_SLOT;if(s=="Image")return UIGRIP_IMAGE;return UIGRIP_NONE;}
static String GripCode(UiScrollGrip v){switch(v){case UIGRIP_LINES:return"UIGRIP_LINES";case UIGRIP_DOTS:return"UIGRIP_DOTS";case UIGRIP_SLOT:return"UIGRIP_SLOT";case UIGRIP_IMAGE:return"UIGRIP_IMAGE";default:return"UIGRIP_NONE";}}

static void AddScrollBarOverrides(UiDesignerControlSpec&spec){const UiScrollBar::Style s=UiTheme::ResolveScrollBar();AddPaletteMetrics(spec,"","Track",s.track_palette,s.track_metrics,false,false,true,true,true,true);AddPaletteMetrics(spec,"thumb","Thumb",s.thumb_palette,s.thumb_metrics,false,false,true,true,true,true);RemoveOverride(spec,"thumb_face_normal");Add(spec,"ink_normal","Normal","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_NORMAL]).ToValue());Add(spec,"thumb_face_hot","Hot","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_HOT]).ToValue());Add(spec,"thumb_face_pressed","Pressed","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_PRESSED]).ToValue());Add(spec,"thumb_face_disabled","Disabled","Thumb / Face",PropertyEditorKind::FillRecipe,FillRecipe(s.thumb_palette.face[ST_DISABLED]).ToValue());AddPaletteMetrics(spec,"arrow","Arrow",s.arrow_palette,s.arrow_metrics,true,true,true,true,true,true);
Add(spec,"arrow_icons","Use arrow icons","Arrows",PropertyEditorKind::Boolean,s.arrow_icons);Add(spec,"arrow_icon_scale","Scale arrow icons","Arrows",PropertyEditorKind::Boolean,s.arrow_icon_scale);AddIconMode(spec,"arrow_icon_render_mode","Icon render mode","Arrows",s.arrow_icon_render_mode);Add(spec,"show_arrows","Show arrows","Arrows",PropertyEditorKind::Boolean,s.show_arrows,true);Add(spec,"arrows_layout","Layout","Arrows",PropertyEditorKind::Choice,ArrowsLayoutName(s.arrows_layout),true).Choice("None","None").Choice("Split","Split").Choice("GroupStart","Group start").Choice("GroupEnd","Group end");Add(spec,"arrow_cross","Cross-axis","Arrows",PropertyEditorKind::Choice,ArrowCrossName(s.arrow_cross),true).Choice("Fill","Fill").Choice("Square","Square");AddNumeric(spec,"arrow_size","Arrow size","Arrows",s.arrow_size,0,256,true);
AddNumeric(spec,"thumb_min_size","Minimum thumb","Thumb / Layout",s.thumb_min_size,1,512,true);Add(spec,"thumb_len_mode","Length mode","Thumb / Layout",PropertyEditorKind::Choice,ThumbLenName(s.thumb_len_mode),true).Choice("Proportional","Proportional").Choice("Fixed","Fixed");AddNumeric(spec,"fixed_thumb_len_px","Fixed length","Thumb / Layout",s.fixed_thumb_len_px,1,2048,true);Add(spec,"paint_track_under_arrows","Track under arrows","Track / Additional",PropertyEditorKind::Boolean,s.paint_track_under_arrows);Add(spec,"auto_hide","Auto hide","Behaviour",PropertyEditorKind::Boolean,s.auto_hide);Add(spec,"thin_idle","Thin when idle","Expansion",PropertyEditorKind::Boolean,s.thin_idle);AddNumeric(spec,"thin_px","Thin size","Expansion",s.thin_px,1,128,true);AddNumeric(spec,"thick_px","Thick size","Expansion",s.thick_px,1,128,true);AddNumeric(spec,"track_paint_px_idle","Track idle size","Expansion",s.track_paint_px_idle,0,128,true);AddNumeric(spec,"track_paint_px_hot","Track hot size","Expansion",s.track_paint_px_hot,0,128,true);AddNumeric(spec,"thumb_paint_px_idle","Thumb idle size","Expansion",s.thumb_paint_px_idle,0,128,true);AddNumeric(spec,"thumb_paint_px_hot","Thumb hot size","Expansion",s.thumb_paint_px_hot,0,128,true);Add(spec,"animate_expand","Animate expansion","Expansion",PropertyEditorKind::Boolean,s.animate_expand);AddNumeric(spec,"expand_ms","Expand duration (ms)","Expansion",s.expand_ms,0,5000);AddNumeric(spec,"collapse_ms","Collapse delay (ms)","Expansion",s.collapse_ms,0,10000);Add(spec,"fade_idle","Fade when idle","Fade",PropertyEditorKind::Boolean,s.fade_idle);AddNumeric(spec,"fade_ms","Fade duration (ms)","Fade",s.fade_ms,0,5000);AddNumeric(spec,"idle_fade_pct","Idle opacity (%)","Fade",s.idle_fade_pct,0,100);Add(spec,"grip","Grip","Grip",PropertyEditorKind::Choice,GripName(s.grip)).Choice("None","None").Choice("Lines","Lines").Choice("Dots","Dots").Choice("Slot","Slot");Add(spec,"grip_color","Grip colour","Grip",PropertyEditorKind::Color,s.grip_color);AddNumeric(spec,"track_inset_left","Left","Track / Inset",s.track_inset.left,0,128,true);AddNumeric(spec,"track_inset_top","Top","Track / Inset",s.track_inset.top,0,128,true);AddNumeric(spec,"track_inset_right","Right","Track / Inset",s.track_inset.right,0,128,true);AddNumeric(spec,"track_inset_bottom","Bottom","Track / Inset",s.track_inset.bottom,0,128,true);AddNumeric(spec,"thumb_inset_left","Left","Thumb / Inset",s.thumb_inset.left,0,128,true);AddNumeric(spec,"thumb_inset_top","Top","Thumb / Inset",s.thumb_inset.top,0,128,true);AddNumeric(spec,"thumb_inset_right","Right","Thumb / Inset",s.thumb_inset.right,0,128,true);AddNumeric(spec,"thumb_inset_bottom","Bottom","Thumb / Inset",s.thumb_inset.bottom,0,128,true);
// Arrow image fields, grip_image and easing function objects are deliberately
// deferred: they have no portable ThemeDocument resource/function contract.
}

static bool ApplyScrollBarField(UiScrollBar::Style&s,const String&id,const Value&v){if(ApplyPaletteMetrics(s.track_palette,s.track_metrics,"",id,v))return true;if(ApplyPaletteMetrics(s.thumb_palette,s.thumb_metrics,"thumb",id,v))return true;if(ApplyPaletteMetrics(s.arrow_palette,s.arrow_metrics,"arrow",id,v))return true;if(id=="ink_normal")ApplyFill(s.thumb_palette.face[ST_NORMAL],v);else if(id=="arrow_icons")s.arrow_icons=(bool)v;else if(id=="arrow_icon_scale")s.arrow_icon_scale=(bool)v;else if(id=="arrow_icon_render_mode")s.arrow_icon_render_mode=IconModeFromValue(v);else if(id=="show_arrows")s.show_arrows=(bool)v;else if(id=="arrows_layout")s.arrows_layout=ArrowsLayoutValue(v);else if(id=="arrow_cross")s.arrow_cross=ArrowCrossValue(v);else if(id=="arrow_size")s.arrow_size=max(0,(int)v);else if(id=="thumb_min_size")s.thumb_min_size=max(1,(int)v);else if(id=="thumb_len_mode")s.thumb_len_mode=ThumbLenValue(v);else if(id=="fixed_thumb_len_px")s.fixed_thumb_len_px=max(1,(int)v);else if(id=="paint_track_under_arrows")s.paint_track_under_arrows=(bool)v;else if(id=="auto_hide")s.auto_hide=(bool)v;else if(id=="thin_idle")s.thin_idle=(bool)v;else if(id=="thin_px")s.thin_px=max(1,(int)v);else if(id=="thick_px")s.thick_px=max(1,(int)v);else if(id=="track_paint_px_idle")s.track_paint_px_idle=max(0,(int)v);else if(id=="track_paint_px_hot")s.track_paint_px_hot=max(0,(int)v);else if(id=="thumb_paint_px_idle")s.thumb_paint_px_idle=max(0,(int)v);else if(id=="thumb_paint_px_hot")s.thumb_paint_px_hot=max(0,(int)v);else if(id=="animate_expand")s.animate_expand=(bool)v;else if(id=="expand_ms")s.expand_ms=max(0,(int)v);else if(id=="collapse_ms")s.collapse_ms=max(0,(int)v);else if(id=="fade_idle")s.fade_idle=(bool)v;else if(id=="fade_ms")s.fade_ms=max(0,(int)v);else if(id=="idle_fade_pct")s.idle_fade_pct=minmax((int)v,0,100);else if(id=="grip")s.grip=GripValue(v);else if(id=="grip_color")s.grip_color=(Color)v;else if(id=="track_inset_left")s.track_inset.left=max(0,(int)v);else if(id=="track_inset_top")s.track_inset.top=max(0,(int)v);else if(id=="track_inset_right")s.track_inset.right=max(0,(int)v);else if(id=="track_inset_bottom")s.track_inset.bottom=max(0,(int)v);else if(id=="thumb_inset_left")s.thumb_inset.left=max(0,(int)v);else if(id=="thumb_inset_top")s.thumb_inset.top=max(0,(int)v);else if(id=="thumb_inset_right")s.thumb_inset.right=max(0,(int)v);else if(id=="thumb_inset_bottom")s.thumb_inset.bottom=max(0,(int)v);else return false;return true;}
static Value ScrollBarFieldValue(const UiScrollBar::Style&s,const String&id){if(IsPaletteMetricsField("",id))return PaletteMetricsValue(s.track_palette,s.track_metrics,"",id);if(IsPaletteMetricsField("thumb",id))return PaletteMetricsValue(s.thumb_palette,s.thumb_metrics,"thumb",id);if(IsPaletteMetricsField("arrow",id))return PaletteMetricsValue(s.arrow_palette,s.arrow_metrics,"arrow",id);if(id=="ink_normal")return FillRecipe(s.thumb_palette.face[ST_NORMAL]).ToValue();if(id=="arrow_icons")return s.arrow_icons;if(id=="arrow_icon_scale")return s.arrow_icon_scale;if(id=="arrow_icon_render_mode")return IconModeName(s.arrow_icon_render_mode);if(id=="show_arrows")return s.show_arrows;if(id=="arrows_layout")return ArrowsLayoutName(s.arrows_layout);if(id=="arrow_cross")return ArrowCrossName(s.arrow_cross);if(id=="arrow_size")return s.arrow_size;if(id=="thumb_min_size")return s.thumb_min_size;if(id=="thumb_len_mode")return ThumbLenName(s.thumb_len_mode);if(id=="fixed_thumb_len_px")return s.fixed_thumb_len_px;if(id=="paint_track_under_arrows")return s.paint_track_under_arrows;if(id=="auto_hide")return s.auto_hide;if(id=="thin_idle")return s.thin_idle;if(id=="thin_px")return s.thin_px;if(id=="thick_px")return s.thick_px;if(id=="track_paint_px_idle")return s.track_paint_px_idle;if(id=="track_paint_px_hot")return s.track_paint_px_hot;if(id=="thumb_paint_px_idle")return s.thumb_paint_px_idle;if(id=="thumb_paint_px_hot")return s.thumb_paint_px_hot;if(id=="animate_expand")return s.animate_expand;if(id=="expand_ms")return s.expand_ms;if(id=="collapse_ms")return s.collapse_ms;if(id=="fade_idle")return s.fade_idle;if(id=="fade_ms")return s.fade_ms;if(id=="idle_fade_pct")return s.idle_fade_pct;if(id=="grip")return GripName(s.grip);if(id=="grip_color")return s.grip_color;if(id=="track_inset_left")return s.track_inset.left;if(id=="track_inset_top")return s.track_inset.top;if(id=="track_inset_right")return s.track_inset.right;if(id=="track_inset_bottom")return s.track_inset.bottom;if(id=="thumb_inset_left")return s.thumb_inset.left;if(id=="thumb_inset_top")return s.thumb_inset.top;if(id=="thumb_inset_right")return s.thumb_inset.right;if(id=="thumb_inset_bottom")return s.thumb_inset.bottom;return Value();}
static bool EmitScrollBarField(String&out,const String&var,const String&id,const Value&v){if(EmitPaletteMetrics(out,var+".track_palette",var+".track_metrics","",id,v))return true;if(EmitPaletteMetrics(out,var+".thumb_palette",var+".thumb_metrics","thumb",id,v))return true;if(EmitPaletteMetrics(out,var+".arrow_palette",var+".arrow_metrics","arrow",id,v))return true;if(id=="ink_normal")out<<"\t"<<var<<".thumb_palette.face[ST_NORMAL] = "<<FillCode(v)<<";\n";else if(id=="arrow_icons")out<<"\t"<<var<<".arrow_icons = "<<AsString((bool)v)<<";\n";else if(id=="arrow_icon_scale")out<<"\t"<<var<<".arrow_icon_scale = "<<AsString((bool)v)<<";\n";else if(id=="arrow_icon_render_mode")out<<"\t"<<var<<".arrow_icon_render_mode = "<<IconModeCode(IconModeFromValue(v))<<";\n";else if(id=="show_arrows")out<<"\t"<<var<<".show_arrows = "<<AsString((bool)v)<<";\n";else if(id=="arrows_layout")out<<"\t"<<var<<".arrows_layout = "<<ArrowsLayoutCode(ArrowsLayoutValue(v))<<";\n";else if(id=="arrow_cross")out<<"\t"<<var<<".arrow_cross = "<<(ArrowCrossValue(v)==UIARROWCROSS_FILL?"UIARROWCROSS_FILL":"UIARROWCROSS_SQUARE")<<";\n";else if(id=="arrow_size")out<<"\t"<<var<<".arrow_size = "<<max(0,(int)v)<<";\n";else if(id=="thumb_min_size")out<<"\t"<<var<<".thumb_min_size = "<<max(1,(int)v)<<";\n";else if(id=="thumb_len_mode")out<<"\t"<<var<<".thumb_len_mode = "<<(ThumbLenValue(v)==UITHUMB_FIXED?"UITHUMB_FIXED":"UITHUMB_PROPORTIONAL")<<";\n";else if(id=="fixed_thumb_len_px")out<<"\t"<<var<<".fixed_thumb_len_px = "<<max(1,(int)v)<<";\n";else if(id=="paint_track_under_arrows")out<<"\t"<<var<<".paint_track_under_arrows = "<<AsString((bool)v)<<";\n";else if(id=="auto_hide")out<<"\t"<<var<<".auto_hide = "<<AsString((bool)v)<<";\n";else if(id=="thin_idle")out<<"\t"<<var<<".thin_idle = "<<AsString((bool)v)<<";\n";else if(id=="thin_px")out<<"\t"<<var<<".thin_px = "<<max(1,(int)v)<<";\n";else if(id=="thick_px")out<<"\t"<<var<<".thick_px = "<<max(1,(int)v)<<";\n";else if(id=="track_paint_px_idle")out<<"\t"<<var<<".track_paint_px_idle = "<<max(0,(int)v)<<";\n";else if(id=="track_paint_px_hot")out<<"\t"<<var<<".track_paint_px_hot = "<<max(0,(int)v)<<";\n";else if(id=="thumb_paint_px_idle")out<<"\t"<<var<<".thumb_paint_px_idle = "<<max(0,(int)v)<<";\n";else if(id=="thumb_paint_px_hot")out<<"\t"<<var<<".thumb_paint_px_hot = "<<max(0,(int)v)<<";\n";else if(id=="animate_expand")out<<"\t"<<var<<".animate_expand = "<<AsString((bool)v)<<";\n";else if(id=="expand_ms")out<<"\t"<<var<<".expand_ms = "<<max(0,(int)v)<<";\n";else if(id=="collapse_ms")out<<"\t"<<var<<".collapse_ms = "<<max(0,(int)v)<<";\n";else if(id=="fade_idle")out<<"\t"<<var<<".fade_idle = "<<AsString((bool)v)<<";\n";else if(id=="fade_ms")out<<"\t"<<var<<".fade_ms = "<<max(0,(int)v)<<";\n";else if(id=="idle_fade_pct")out<<"\t"<<var<<".idle_fade_pct = "<<minmax((int)v,0,100)<<";\n";else if(id=="grip")out<<"\t"<<var<<".grip = "<<GripCode(GripValue(v))<<";\n";else if(id=="grip_color")out<<"\t"<<var<<".grip_color = "<<EmitValue(v)<<";\n";else if(id=="track_inset_left")out<<"\t"<<var<<".track_inset.left = "<<max(0,(int)v)<<";\n";else if(id=="track_inset_top")out<<"\t"<<var<<".track_inset.top = "<<max(0,(int)v)<<";\n";else if(id=="track_inset_right")out<<"\t"<<var<<".track_inset.right = "<<max(0,(int)v)<<";\n";else if(id=="track_inset_bottom")out<<"\t"<<var<<".track_inset.bottom = "<<max(0,(int)v)<<";\n";else if(id=="thumb_inset_left")out<<"\t"<<var<<".thumb_inset.left = "<<max(0,(int)v)<<";\n";else if(id=="thumb_inset_top")out<<"\t"<<var<<".thumb_inset.top = "<<max(0,(int)v)<<";\n";else if(id=="thumb_inset_right")out<<"\t"<<var<<".thumb_inset.right = "<<max(0,(int)v)<<";\n";else if(id=="thumb_inset_bottom")out<<"\t"<<var<<".thumb_inset.bottom = "<<max(0,(int)v)<<";\n";else return false;return true;}
class ScrollBarThemeAdapter final:public UiDesignerThemeAdapter{public:const char*Id()const override{return"scroll_bar";}bool Supports(UiDesignerRuntimeKind k)const override{return k==UiDesignerRuntimeKind::UiScrollBar;}void AddThemeOverrides(UiDesignerControlSpec&s)const override{AddScrollBarOverrides(s);}bool HasField(const String&id)const override{UiScrollBar::Style s;return IsPaletteMetricsField("",id)||IsPaletteMetricsField("thumb",id)||IsPaletteMetricsField("arrow",id)||!IsNull(ScrollBarFieldValue(s,id));}bool FieldAffectsLayout(const String&id)const override{return PaletteMetricsAffectsLayout("",id)||PaletteMetricsAffectsLayout("thumb",id)||PaletteMetricsAffectsLayout("arrow",id)||id=="show_arrows"||id=="arrows_layout"||id=="arrow_cross"||id=="arrow_size"||id=="thumb_min_size"||id=="thumb_len_mode"||id=="fixed_thumb_len_px"||id=="thin_idle"||id=="thin_px"||id=="thick_px"||id.StartsWith("track_inset_")||id.StartsWith("thumb_inset_");}Value ResolveFieldValue(const UiDesignerNode&n,const UiDesignerControlSpec&sp,const String&id,const UiDesignerTransientOverlay*o)const override{UiScrollBar::Style s=UiTheme::ResolveScrollBar();for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a)ApplyScrollBarField(s,p.adapter_field_id,v);}return ScrollBarFieldValue(s,id);}void ApplyPreviewStyle(Ctrl&ctrl,const UiDesignerNode&n,const UiDesignerControlSpec&sp,const UiDesignerTransientOverlay*o)const override{auto*c=dynamic_cast<UiScrollBar*>(&ctrl);if(!c)return;UiScrollBar::Style s=UiTheme::ResolveScrollBar();bool authored=false;for(const auto&p:sp.theme_overrides){bool a=false;Value v=Effective(n,p,o,a);if(a){authored=true;ApplyScrollBarField(s,p.adapter_field_id,v);}}if(authored)c->SetCustomStyle(s);else c->ClearCustomStyle();}void EmitSetup(String&out,const String&m,const UiDesignerNode&n,const UiDesignerControlSpec&sp)const override{if(!HasAuthored(n,sp))return;String var=m+"_style";out<<"\tUiScrollBar::Style "<<var<<" = UiTheme::ResolveScrollBar();\n";for(const auto&p:sp.theme_overrides){int q=n.theme_overrides.Find(p.id);if(q>=0)EmitScrollBarField(out,var,p.adapter_field_id,n.theme_overrides.GetValue(q));}out<<"\t"<<m<<".SetCustomStyle("<<var<<");\n";}};

CheckThemeAdapter check_adapter;
RadioThemeAdapter radio_adapter;
ToggleThemeAdapter toggle_adapter;
ProgressThemeAdapter progress_adapter;
SliderThemeAdapter slider_adapter;
ScrollBarThemeAdapter scroll_bar_adapter;

} // namespace

const UiDesignerThemeAdapter& UiDesignerCheckThemeAdapterInstance(){return check_adapter;}
const UiDesignerThemeAdapter& UiDesignerRadioThemeAdapterInstance(){return radio_adapter;}
const UiDesignerThemeAdapter& UiDesignerToggleThemeAdapterInstance(){return toggle_adapter;}
const UiDesignerThemeAdapter& UiDesignerProgressThemeAdapterInstance(){return progress_adapter;}
const UiDesignerThemeAdapter& UiDesignerSliderThemeAdapterInstance(){return slider_adapter;}
const UiDesignerThemeAdapter& UiDesignerScrollBarThemeAdapterInstance(){return scroll_bar_adapter;}

} // namespace Upp
