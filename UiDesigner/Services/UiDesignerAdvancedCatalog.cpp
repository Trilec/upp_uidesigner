#include "UiDesignerAdvancedCatalog.h"

namespace Upp {

static UiDesignerControlSpec *MutableSpec(UiDesignerCatalog& catalog,
                                         const String& type)
{
    return const_cast<UiDesignerControlSpec *>(catalog.Find(type));
}

static UiDesignerPropertySpec *MutableProperty(UiDesignerControlSpec& spec,
                                               const String& id)
{
    for(UiDesignerPropertySpec& property : spec.properties)
        if(property.id == id)
            return &property;
    return nullptr;
}

static void ApplyWorkingSizingEditors(UiDesignerControlSpec& spec)
{
    static const char *fields[] = {
        "fixed_width", "fixed_height", "min_width", "min_height",
        "max_width", "max_height"
    };
    for(const char *field : fields) {
        UiDesignerPropertySpec *property = MutableProperty(spec, field);
        if(!property)
            continue;
        property->Editor("property.numeric-int-working-range");
        property->editor_variant = "0:500";
        if(String(field).StartsWith("max_"))
            property->help = "0 = No limit. Type values above 500 directly; the slider is only a working range.";
        else
            property->help = "Type values above 500 directly; the slider is only a working range.";
    }
}

static void AddTextProperty(UiDesignerControlSpec& spec,
                            const String& default_text)
{
    if(spec.FindProperty("text"))
        return;
    UiDesignerPropertySpec property = UiDesignerTextProperty();
    property.default_value = default_text;
    spec.properties.Add(property);
    spec.defaults.Set("text", default_text);
}

static void AddGroupPanelMetadata(UiDesignerControlSpec& spec)
{
    if(!spec.FindProperty("subtitle")) {
        UiDesignerPropertySpec subtitle = UiDesignerTextProperty(
            "subtitle", "Subtitle");
        subtitle.group = "Content";
        subtitle.default_value = "Supporting information";
        subtitle.impact = PropertyImpactPaint |
                          PropertyImpactLocalLayout | PropertyImpactCode;
        spec.properties.Add(subtitle);
        spec.defaults.Set("subtitle", subtitle.default_value);
    }
    if(!spec.FindProperty("icon")) {
        UiDesignerPropertySpec icon;
        icon.id = "icon";
        icon.label = "Icon";
        icon.group = "Content";
        icon.help = "Optional GroupPanel header icon. Choose None for a text-only panel.";
        icon.kind = PropertyEditorKind::Custom;
        icon.custom_editor = "property.icon";
        icon.row_span = 1;
        icon.expanded_row_span = 6;
        icon.domain = PropertyEditorDomain::Content;
        icon.default_value = "None";
        icon.impact = PropertyImpactPaint |
                      PropertyImpactLocalLayout | PropertyImpactCode;
        spec.properties.Add(icon);
        spec.defaults.Set("icon", icon.default_value);
    }
}

static void AddUiDocScalarData(UiDesignerControlSpec& spec)
{
    spec.data_capability = UiDesignerDataCapability::Scalar;
    spec.data_adapter_id = "scalar";
    if(spec.FindProperty("value"))
        return;

    UiDesignerPropertySpec value = UiDesignerTextProperty("value", "Value");
    value.group = "Data";
    value.help = "Scalar UiDoc value. The Data pane, live Preview and generated SetData call share this authored value.";
    value.domain = PropertyEditorDomain::Content;
    value.default_value = "UiDoc sample";
    value.impact = PropertyImpactControlState |
                   PropertyImpactPaint | PropertyImpactCode;
    spec.properties.Add(value);
    spec.defaults.Set("value", value.default_value);
}

bool UiDesignerBuildScalarDataPropertyModel(
    const UiDesignerControlSpec& spec, const UiDesignerNode& node,
    PropertyEditorModel& model)
{
    if(spec.data_capability != UiDesignerDataCapability::Scalar)
        return false;
    const UiDesignerPropertySpec *value = spec.FindProperty("value");
    if(!value)
        return false;

    model.Clear();
    UiDesignerPropertySpec projected = *value;
    projected.group = "Scalar";
    projected.AddTo(model, node.GetProperty("value", value->default_value), false);
    model.SetGroupSubtitle(
        "Scalar", spec.display_name +
            " · one authored value shared by Data, Preview and code generation");
    model.StructureChanged();
    return model.Find("value") != nullptr;
}

static void FinalizeAdvancedLeaf(UiDesignerControlSpec& spec)
{
    spec.sizing_class = UiDesignerSizingClass::Leaf;
    spec.node_flags = UiDesignerNodeNone;
    spec.capabilities = UiDesignerCapabilityRuntimeCtrl;
    spec.preview_adapter_id = "runtime:" + spec.type_id;
    spec.codegen_adapter_id = "control";
    spec.child_adapter_id = "none";
    spec.content_host = UiDesignerContentHostKind::None;
    spec.max_direct_children = 0;
    spec.preview = true;
    spec.inspector = true;
    spec.codegen = true;
    spec.theme = false;
    AddUiDesignerCommonProperties(spec);
    spec.defaults.Set("visible", true);
    spec.defaults.Set("enabled", true);
    spec.defaults.Set("role", "Standard");
    spec.defaults.Set("width_mode", "Fit");
    spec.defaults.Set("height_mode", "Fit");
    ApplyWorkingSizingEditors(spec);
}

static UiDesignerControlSpec MakeRangeSliderSpec()
{
    UiDesignerControlSpec spec;
    spec.type_id = "UiRangeSlider";
    spec.display_name = "Range Slider";
    spec.category = "Advanced";
    spec.runtime_cpp_type = "UiRangeSlider";
    spec.default_base_name = "range_slider";
    spec.help = "Two-handle range selector using the reusable UiRangeSlider runtime.";
    spec.icon_key = "controls";
    spec.runtime_kind = UiDesignerRuntimeKind::Placeholder;
    spec.default_size = Size(220, 32);
    spec.minimum_size = Size(100, 24);
    FinalizeAdvancedLeaf(spec);

    ValueArray value;
    value.Add(25.0);
    value.Add(75.0);
    UiDesignerPropertySpec range;
    range.id = "value";
    range.label = "Range";
    range.group = "Value";
    range.kind = PropertyEditorKind::Custom;
    range.custom_editor = "property.range.double";
    range.editor_variant = "range";
    range.default_value = value;
    range.minimum = 0.0;
    range.maximum = 100.0;
    range.step = 1.0;
    range.decimals = 0;
    range.row_span = 1;
    range.expanded_row_span = 2;
    range.domain = PropertyEditorDomain::Behaviour;
    range.impact = PropertyImpactControlState |
                   PropertyImpactPaint | PropertyImpactCode;
    spec.properties.Add(range);
    spec.defaults.Set("value", value);

    UiDesignerEventSpec& action = spec.events.Add();
    action.id = "WhenAction";
    action.label = "Action";
    action.help = "Runs when the selected range is committed.";
    spec.capabilities |= UiDesignerCapabilityAcceptActions;
    return spec;
}

static UiDesignerControlSpec MakeNodeGraphSpec()
{
    UiDesignerControlSpec spec;
    spec.type_id = "UiNodeGraph";
    spec.display_name = "Node Graph";
    spec.category = "Advanced";
    spec.runtime_cpp_type = "UiNodeGraph";
    spec.default_base_name = "node_graph";
    spec.help = "Model-driven graph editor/viewer. Graph topology remains application-owned.";
    spec.icon_key = "ICON_DESIGN_ACCOUNT_TREE_48";
    spec.runtime_kind = UiDesignerRuntimeKind::Placeholder;
    spec.default_size = Size(420, 260);
    spec.minimum_size = Size(160, 100);
    spec.data_capability = UiDesignerDataCapability::Unsupported;
    FinalizeAdvancedLeaf(spec);
    return spec;
}

void RegisterUiDesignerAdvancedCatalog(UiDesignerCatalog& catalog)
{
    if(UiDesignerControlSpec *tool = MutableSpec(catalog, "UiToolButton"))
        AddTextProperty(*tool, String());

    if(UiDesignerControlSpec *group = MutableSpec(catalog, "UiGroupPanel"))
        AddGroupPanelMetadata(*group);

    if(UiDesignerControlSpec *doc = MutableSpec(catalog, "UiDoc"))
        AddUiDocScalarData(*doc);

    for(int i = 0; i < catalog.GetCount(); i++)
        ApplyWorkingSizingEditors(
            *const_cast<UiDesignerControlSpec *>(&catalog[i]));

    if(!catalog.Find("UiRangeSlider"))
        catalog.Register(MakeRangeSliderSpec());
    if(!catalog.Find("UiNodeGraph"))
        catalog.Register(MakeNodeGraphSpec());
    if(!catalog.FindPreset("Demo"))
        catalog.RegisterPreset({"Demo", "Demo", "Designer closure showcase with advanced controls and a four-panel Grid", "ICON_DESIGN_WIDGETS_48"});
}

UiDesignerApplicationCatalog::UiDesignerApplicationCatalog()
{
    RegisterUiDesignerBuiltins(*this);
    RegisterUiDesignerAdvancedCatalog(*this);
}

}
