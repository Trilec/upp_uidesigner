#include "UiDesignerPreview.h"
#include <Ui/UiIcons.h>
#include <Ui/UiProgressRing.h>
#include <Ui/UiRangeSliderEdit.h>

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

static Image ResolveAdvancedCatalogIcon(const String& name)
{
    if(name.IsEmpty() || name == "None")
        return Image();
    for(const UiIconCatalogEntry& entry : UiIconCatalog())
        if(entry.name == name && entry.factory)
            return entry.factory();
    return Image();
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

static UiDesignerApplyResult ApplyDesignerRangeSliderEdit(
    Ctrl& ctrl, const UiDesignerControlSpec&, const String& property, const Value& value)
{
    auto* range = dynamic_cast<UiRangeSliderEdit*>(&ctrl);
    if(!range) return UiDesignerApplyResult::Rejected;
    if(property == "range") {
        Vector<double> pair = PropertyEditorReadVector(value, 2);
        range->SetRange(pair[0], pair[1]);
        return UiDesignerApplyResult::RequiresSubtreeRebuild;
    }
    if(property == "step") {
        range->SetStep((double)value);
        return UiDesignerApplyResult::RequiresSubtreeRebuild;
    }
    if(property == "value") { range->SetData(value); return UiDesignerApplyResult::AppliedControlState; }
    if(property == "direction") range->SetDirection(AsString(value) == "V" ? UiDirection::V : UiDirection::H);
    else if(property == "field_width") range->SetFieldWidth(DPI((int)value));
    else if(property == "gap") range->SetGap(DPI((int)value));
    else if(property == "inset") range->SetInset(DPI((int)value));
    else if(property == "precision") range->SetPrecision((int)value);
    else return ApplyAdvancedCommon(ctrl, property, value);
    return UiDesignerApplyResult::AppliedLocalLayout;
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

static UiDesignerApplyResult ApplyDesignerProgressRing(
    Ctrl& ctrl, const UiDesignerControlSpec&, const String& property,
    const Value& value)
{
    UiProgressRing* ring = dynamic_cast<UiProgressRing*>(&ctrl);
    if(!ring)
        return UiDesignerApplyResult::Rejected;
    if(property == "total") {
        ring->SetTotal((int)value);
        return UiDesignerApplyResult::RequiresSubtreeRebuild;
    }
    else if(property == "value") ring->SetData(value);
    else if(property == "show_percent") ring->Percent((bool)value);
    else if(property == "center_text") {
        if(AsString(value).IsEmpty()) ring->ClearText();
        else ring->SetText(AsString(value));
    }
    else if(property == "animate_on_show") ring->AnimateOnShow((bool)value);
    else if(property == "intro_duration") ring->SetIntroDuration((int)value);
    else if(property == "indeterminate_duration") ring->SetIndeterminateDuration((int)value);
    else if(property == "role") {
        const String role = AsString(value);
        ring->SetRole(role == "Accent" ? UiRole::Accent :
                      role == "Alert" ? UiRole::Alert :
                      role == "Subtle" ? UiRole::Subtle : UiRole::Standard);
    }
    else return ApplyAdvancedCommon(ctrl, property, value);
    return UiDesignerApplyResult::AppliedControlState;
}

static void RegisterAdvancedPreviewAdapters()
{
    UiDesignerPreviewAdapterRegistry& registry =
        UiDesignerPreviewAdapterRegistry::Global();

    // Extend the normal GroupPanel runtime adapter without cloning its existing
    // title/role/visibility/theme behaviour. Designer-only metadata delegates
    // every unaffected property back to the established runtime adapter.
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
            if(UiGroupPanel *panel = dynamic_cast<UiGroupPanel *>(&ctrl)) {
                if(property == "subtitle") {
                    panel->SetSubTitle(AsString(value));
                    ctrl.RefreshLayout();
                    return UiDesignerApplyResult::AppliedLocalLayout;
                }
                if(property == "icon") {
                    const Image icon = ResolveAdvancedCatalogIcon(AsString(value));
                    if(IsNull(icon))
                        panel->ClearIcon();
                    else
                        panel->SetIcon(icon);
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

    // UiDoc's Designer scalar is deliberately the same property used by the
    // Inspector and code generator. Preview therefore applies it through the
    // runtime SetData contract rather than maintaining a second document copy.
    UiDesignerControlSpec doc_spec;
    doc_spec.type_id = "UiDoc";
    doc_spec.display_name = "Document";
    doc_spec.preview_adapter_id = "runtime:UiDoc";
    doc_spec.runtime_kind = UiDesignerRuntimeKind::UiDoc;
    const UiDesignerPreviewAdapter *doc_base =
        UiDesignerPreviewFactory::Adapter(doc_spec);
    if(doc_base) {
        UiDesignerPreviewAdapter doc;
        doc.id = doc_spec.preview_adapter_id;
        doc.semantic = false;
        doc.create = doc_base->create;
        doc.initialize = doc_base->initialize;
        Function<UiDesignerApplyResult(
            Ctrl&, const UiDesignerControlSpec&, const String&, const Value&)>
            base_apply = doc_base->apply;
        doc.apply = [base_apply](Ctrl& ctrl,
                                const UiDesignerControlSpec& spec,
                                const String& property,
                                const Value& value) {
            if(property == "value") {
                if(UiDoc *doc = dynamic_cast<UiDoc *>(&ctrl)) {
                    doc->SetData(value);
                    return UiDesignerApplyResult::AppliedControlState;
                }
            }
            return base_apply
                ? base_apply(ctrl, spec, property, value)
                : UiDesignerApplyResult::Rejected;
        };
        registry.Register(pick(doc));
    }

    UiDesignerPreviewAdapter ring;
    ring.id = "runtime:UiProgressRing";
    ring.create = [] { return One<Ctrl>(new UiProgressRing); };
    ring.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) { ctrl.Tip(spec.help); };
    ring.apply = ApplyDesignerProgressRing;
    registry.Register(pick(ring));

    UiDesignerPreviewAdapter range;
    range.id = "runtime:UiRangeSlider";
    range.create = CreateDesignerRangeSlider;
    range.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) {
        ctrl.Tip(spec.help);
    };
    range.apply = ApplyDesignerRangeSlider;
    registry.Register(pick(range));

    UiDesignerPreviewAdapter range_edit;
    range_edit.id = "runtime:UiRangeSliderEdit";
    range_edit.create = [] { return One<Ctrl>(new UiRangeSliderEdit); };
    range_edit.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) { ctrl.Tip(spec.help); };
    range_edit.apply = ApplyDesignerRangeSliderEdit;
    registry.Register(pick(range_edit));

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
