#include "UiDesignerNormalizedThemeCommon.h"
#include <Ui/UiChartRing.h>

namespace Upp {
namespace {
using namespace UiDesignerNormalizedTheme;

int ChartSeriesIndex(const String& id)
{
    for(int i = 0; i < UiChartRing::MAX_SERIES_COLORS; ++i)
        if(id == "series." + AsString(i))
            return i;
    return -1;
}

UiChartRing::Style ChartBase(const UiDesignerNode& node)
{
    UiChartRing chart;
    chart.SetRole(Role(node.GetProperty("role", "Standard")));
    return chart.GetStyle();
}

Value ChartField(const UiChartRing::Style& s, const String& id)
{
    int index = DotState(id, "track");
    if(index >= 0) return s.track_palette.face[index].color;
    index = DotState(id, "text");
    if(index >= 0) return s.text_palette.ink[index];
    index = ChartSeriesIndex(id);
    if(index >= 0) return s.series[index];
    if(id == "series_count") return s.series_count;
    if(id == "thickness") return s.thickness;
    if(id == "cap_roundness") return s.cap_roundness;
    if(id == "ring_inset") return s.ring_inset;
    if(id == "segment_gap") return s.segment_gap;
    if(id == "min_text_height") return s.min_text_height;
    if(id == "font_height") return s.font.GetHeight();
    if(id == "font_face") return s.font.GetFaceName();
    if(id == "font_bold") return s.font.IsBold();
    if(id == "font_italic") return s.font.IsItalic();
    return Value();
}

int ChartMetric(const String& id, const Value& value)
{
    const int n = (int)value;
    if(id == "series_count") return clamp(n, 1, UiChartRing::MAX_SERIES_COLORS);
    if(id == "cap_roundness") return clamp(n, 0, 100);
    return max(id == "ring_inset" || id == "segment_gap" ? 0 : 1, n);
}

void SetChartField(UiChartRing::Style& s, const String& id, const Value& value)
{
    int index = DotState(id, "track");
    if(index >= 0) { s.track_palette.face[index] = UiFill::Solid((Color)value); return; }
    index = DotState(id, "text");
    if(index >= 0) { s.text_palette.ink[index] = (Color)value; return; }
    index = ChartSeriesIndex(id);
    if(index >= 0) { s.series[index] = (Color)value; return; }
    if(id == "font_face") { s.font.FaceName(AsString(value)); return; }
    if(id == "font_bold") { s.font.Bold((bool)value); return; }
    if(id == "font_italic") { s.font.Italic((bool)value); return; }
    const int n = ChartMetric(id, value);
    if(id == "series_count") s.series_count = n;
    else if(id == "thickness") s.thickness = n;
    else if(id == "cap_roundness") s.cap_roundness = n;
    else if(id == "ring_inset") s.ring_inset = n;
    else if(id == "segment_gap") s.segment_gap = n;
    else if(id == "min_text_height") s.min_text_height = n;
    else if(id == "font_height") s.font.Height(n);
}

void EmitChartField(String& out, const String& var, const String& id, const Value& value)
{
    int index = DotState(id, "track");
    if(index >= 0) {
        out << "\t" << var << ".track_palette.face[" << StateCode(index)
            << "] = UiFill::Solid(" << EmitValue(value) << ");\n";
        return;
    }
    index = DotState(id, "text");
    if(index >= 0) {
        out << "\t" << var << ".text_palette.ink[" << StateCode(index)
            << "] = " << EmitValue(value) << ";\n";
        return;
    }
    index = ChartSeriesIndex(id);
    if(index >= 0) {
        out << "\t" << var << ".series[" << index << "] = " << EmitValue(value) << ";\n";
        return;
    }
    if(id == "font_face") out << "\t" << var << ".font.FaceName(" << EmitValue(value) << ");\n";
    else if(id == "font_bold") out << "\t" << var << ".font.Bold(" << EmitValue(value) << ");\n";
    else if(id == "font_italic") out << "\t" << var << ".font.Italic(" << EmitValue(value) << ");\n";
    else if(id == "font_height") out << "\t" << var << ".font.Height(" << ChartMetric(id, value) << ");\n";
    else out << "\t" << var << "." << id << " = " << ChartMetric(id, value) << ";\n";
}

class ChartRingThemeAdapter final : public UiDesignerThemeAdapter {
public:
    const char* Id() const override { return "chart_ring"; }
    bool Supports(UiDesignerRuntimeKind kind) const override
    {
        return kind == UiDesignerRuntimeKind::UiChartRing;
    }
    bool HasField(const String& id) const override
    {
        return DotState(id, "track") >= 0 || DotState(id, "text") >= 0 ||
               ChartSeriesIndex(id) >= 0 || id == "series_count" ||
               id == "thickness" || id == "cap_roundness" ||
               id == "ring_inset" || id == "segment_gap" ||
               id == "min_text_height" || id == "font_height" ||
               id == "font_face" || id == "font_bold" || id == "font_italic";
    }
    bool FieldAffectsLayout(const String& id) const override
    {
        return id == "thickness" || id == "ring_inset" || id == "segment_gap" ||
               id == "min_text_height" || id.StartsWith("font_");
    }
    void AddThemeOverrides(UiDesignerControlSpec& spec) const override
    {
        const auto s = ChartBase(UiDesignerNode());
        for(const char* field : {"track", "text"})
            for(const char* state : {"normal", "hot", "pressed", "disabled"}) {
                const String id = String(field) + "." + state;
                Add(spec, id, state, field, PropertyEditorKind::Color, ChartField(s, id));
            }
        for(int i = 0; i < UiChartRing::MAX_SERIES_COLORS; ++i)
            Add(spec, "series." + AsString(i), "Colour " + AsString(i + 1),
                "Series", PropertyEditorKind::Color, s.series[i]);
        AddInt(spec, "series_count", "Series colours", "Series", s.series_count,
               1, UiChartRing::MAX_SERIES_COLORS);
        AddInt(spec, "thickness", "Thickness", "Ring", s.thickness, 1, 256, true);
        AddInt(spec, "cap_roundness", "Cap roundness (%)", "Ring", s.cap_roundness, 0, 100);
        AddInt(spec, "ring_inset", "Inset", "Ring", s.ring_inset, 0, 256, true);
        AddInt(spec, "segment_gap", "Segment gap", "Ring", s.segment_gap, 0, 256, true);
        AddInt(spec, "min_text_height", "Minimum text height", "Typography", s.min_text_height, 1, 128, true);
        AddInt(spec, "font_height", "Font height", "Typography", s.font.GetHeight(), 1, 128, true);
        Add(spec, "font_face", "Font face", "Typography", PropertyEditorKind::Text, s.font.GetFaceName(), true);
        Add(spec, "font_bold", "Bold", "Typography", PropertyEditorKind::Boolean, s.font.IsBold(), true);
        Add(spec, "font_italic", "Italic", "Typography", PropertyEditorKind::Boolean, s.font.IsItalic(), true);
    }
    UiChartRing::Style Resolve(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                              const UiDesignerTransientOverlay* overlay) const
    {
        auto style = ChartBase(node);
        for(const auto& property : spec.theme_overrides)
            if((node.IsThemeOverrideActive(property.id) && node.theme_overrides.Find(property.id) >= 0) ||
               (overlay && overlay->Has(node.id, UiDesignerTransientValueKind::ThemeOverride, property.id)))
                SetChartField(style, property.adapter_field_id,
                    ResolveValue(node, overlay, property.id, AuthoredOrDefault(node, property)));
        return style;
    }
    Value ResolveFieldValue(const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                            const String& id, const UiDesignerTransientOverlay* overlay) const override
    {
        return ChartField(Resolve(node, spec, overlay), id);
    }
    void ApplyPreviewStyle(Ctrl& ctrl, const UiDesignerNode& node, const UiDesignerControlSpec& spec,
                           const UiDesignerTransientOverlay* overlay) const override
    {
        if(auto* chart = dynamic_cast<UiChartRing*>(&ctrl)) {
            chart->SetRole(Role(node.GetProperty("role", "Standard")));
            // SetCustomStyle touches appearance only; it must not re-author data.
            chart->SetCustomStyle(Resolve(node, spec, overlay));
        }
    }
    void EmitSetup(String& out, const String& member, const UiDesignerNode& node,
                   const UiDesignerControlSpec& spec) const override
    {
        out << "\t" << member << ".ClearCustomStyle().SetRole("
            << RoleExpr(node.GetProperty("role", "Standard")) << ");\n";
        bool authored = false;
        for(const auto& property : spec.theme_overrides)
            authored |= node.IsThemeOverrideActive(property.id) &&
                        node.theme_overrides.Find(property.id) >= 0;
        if(!authored) return;
        const String var = member + "_style";
        out << "\tUiChartRing::Style " << var << " = " << member << ".GetStyle();\n";
        for(const auto& property : spec.theme_overrides) {
            const int q = node.theme_overrides.Find(property.id);
            if(q >= 0 && node.IsThemeOverrideActive(property.id))
                EmitChartField(out, var, property.adapter_field_id, node.theme_overrides.GetValue(q));
        }
        out << "\t" << member << ".SetCustomStyle(" << var << ");\n";
    }
};
}

const UiDesignerThemeAdapter& UiDesignerChartRingThemeAdapterInstance()
{
    static ChartRingThemeAdapter adapter;
    return adapter;
}
}
