#include "UiDesignerPreview.h"
#include <UiDesigner/Core/UiDesignerChartRingData.h>
#include <Ui/UiChartRing.h>
#include <Ui/UiTheme.h>

namespace Upp {
namespace {

UiDesignerApplyResult ApplyChartRing(
    Ctrl& ctrl, const UiDesignerControlSpec&, const String& property, const Value& value)
{
    UiChartRing* chart = dynamic_cast<UiChartRing*>(&ctrl);
    if(!chart)
        return UiDesignerApplyResult::Rejected;
    if(property == "segments") {
        Vector<UiDesignerChartRingSegment> decoded;
        String error;
        if(!UiDesignerReadChartRingSegments(value, decoded, error))
            return UiDesignerApplyResult::Rejected;
        Vector<UiChartRingSegment> segments;
        for(const auto& row : decoded)
            segments.Add(UiChartRingSegment(row.value, row.label, row.color));
        chart->SetSegments(segments);
    }
    else if(property == "explicit_total") {
        double total;
        if(!UiDesignerChartRingNumber(value, total))
            return UiDesignerApplyResult::Rejected;
        chart->SetTotal(total);
    }
    else if(property == "center_text") {
        if(!value.Is<String>())
            return UiDesignerApplyResult::Rejected;
        chart->SetCenterText((String)value);
    }
    else if(property == "role") {
        const String role = AsString(value);
        chart->SetRole(role == "Accent" ? UiRole::Accent :
                       role == "Alert" ? UiRole::Alert :
                       role == "Subtle" ? UiRole::Subtle : UiRole::Standard);
    }
    else if(property == "visible") chart->Show((bool)value);
    else if(property == "enabled") chart->Enable((bool)value);
    else return UiDesignerApplyResult::Rejected;
    return UiDesignerApplyResult::AppliedControlState;
}

struct ChartRingPreviewRegistration {
    ChartRingPreviewRegistration()
    {
        UiDesignerPreviewAdapter adapter;
        adapter.id = "runtime:UiChartRing";
        adapter.create = [] { return One<Ctrl>(new UiChartRing); };
        adapter.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) {
            ctrl.Tip(spec.help);
            // Factory-only samples use the same catalog defaults as new nodes.
            for(const char* id : {"segments", "explicit_total", "center_text"})
                if(const auto* property = spec.FindProperty(id))
                    ApplyChartRing(ctrl, spec, id, property->default_value);
        };
        adapter.apply = ApplyChartRing;
        UiDesignerPreviewAdapterRegistry::Global().Register(pick(adapter));
    }
};
ChartRingPreviewRegistration s_chart_ring_preview_registration;

}
}
