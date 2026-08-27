#include "UiDesignerPreview.h"

namespace Upp {

static UiDesignerApplyResult ApplyAdvancedCommon(
    Ctrl& ctrl, const String& property, const Value& value)
{
    if(property == "visible") {
        ctrl.Show((bool)value);
        return UiDesignerApplyResult::AppliedControlState;
    }
    if(property == "enabled") {
        ctrl.Enable((bool)value);
        return UiDesignerApplyResult::AppliedControlState;
    }
    if(property == "role") {
        ctrl.Refresh();
        return UiDesignerApplyResult::AppliedPaint;
    }
    return UiDesignerApplyResult::Rejected;
}

static One<Ctrl> CreateDesignerRangeSlider()
{
    UiRangeSlider *range = new UiRangeSlider;
    range->SetRange(0, 100).SetValues(25, 75);
    return One<Ctrl>(range);
}

static UiDesignerApplyResult ApplyDesignerRangeSlider(
    Ctrl& ctrl, const UiDesignerControlSpec&, const String& property,
    const Value& value)
{
    if(UiRangeSlider *range = dynamic_cast<UiRangeSlider *>(&ctrl)) {
        if(property == "value") {
            range->SetData(value);
            return UiDesignerApplyResult::AppliedControlState;
        }
    }
    return ApplyAdvancedCommon(ctrl, property, value);
}

static One<Ctrl> CreateDesignerNodeGraph()
{
    UiNodeGraph *graph = new UiNodeGraph;
    graph->UseInternalModel()
         .SetEditable(false)
         .EnableInternalMutation(false)
         .SetAutoFitOnFirstPaint(true);
    graph->Model().AddNode("Input", Pointf(20, 36), Sizef(116, 66));
    graph->Model().AddNode("Process", Pointf(176, 82), Sizef(132, 72));
    graph->Model().AddNode("Output", Pointf(348, 42), Sizef(116, 66));
    return One<Ctrl>(graph);
}

static UiDesignerApplyResult ApplyDesignerNodeGraph(
    Ctrl& ctrl, const UiDesignerControlSpec&, const String& property,
    const Value& value)
{
    return ApplyAdvancedCommon(ctrl, property, value);
}

static void RegisterAdvancedPreviewAdapters()
{
    UiDesignerPreviewAdapterRegistry& registry =
        UiDesignerPreviewAdapterRegistry::Global();

    // Extend the normal GroupPanel runtime adapter without cloning its existing
    // title/role/visibility/theme behaviour.
    UiDesignerControlSpec group_spec;
    group_spec.type_id = "UiGroupPanel";
    group_spec.display_name = "Group Panel";
    group_spec.preview_adapter_id = "runtime:UiGroupPanel";
    group_spec.runtime_kind = UiDesignerRuntimeKind::UiGroupPanel;
    const UiDesignerPreviewAdapter *group_base =
        UiDesignerPreviewFactory::Adapter(group_spec);
    if(group_base) {
        UiDesignerPreviewAdapter group;
        group.id = group_spec.preview_adapter_id;
        group.semantic = false;
        group.create = group_base->create;
        group.initialize = group_base->initialize;
        Function<UiDesignerApplyResult(
            Ctrl&, const UiDesignerControlSpec&, const String&, const Value&)>
            base_apply = group_base->apply;
        group.apply = [base_apply](Ctrl& ctrl,
                                   const UiDesignerControlSpec& spec,
                                   const String& property,
                                   const Value& value) {
            if(property == "subtitle") {
                if(UiGroupPanel *panel = dynamic_cast<UiGroupPanel *>(&ctrl)) {
                    panel->SetSubTitle(AsString(value));
                    ctrl.RefreshLayout();
                    return UiDesignerApplyResult::AppliedLocalLayout;
                }
            }
            return base_apply
                ? base_apply(ctrl, spec, property, value)
                : UiDesignerApplyResult::Rejected;
        };
        registry.Register(pick(group));
    }

    UiDesignerPreviewAdapter range;
    range.id = "runtime:UiRangeSlider";
    range.create = CreateDesignerRangeSlider;
    range.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) {
        ctrl.Tip(spec.help);
    };
    range.apply = ApplyDesignerRangeSlider;
    registry.Register(pick(range));

    UiDesignerPreviewAdapter graph;
    graph.id = "runtime:UiNodeGraph";
    graph.create = CreateDesignerNodeGraph;
    graph.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) {
        ctrl.Tip(spec.help);
    };
    graph.apply = ApplyDesignerNodeGraph;
    registry.Register(pick(graph));
}

namespace {
struct UiDesignerAdvancedPreviewRegistration {
    UiDesignerAdvancedPreviewRegistration()
    {
        RegisterAdvancedPreviewAdapters();
    }
};
UiDesignerAdvancedPreviewRegistration s_advanced_preview_registration;
}

}
