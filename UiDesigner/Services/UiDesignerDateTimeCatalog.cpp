#include "UiDesignerDateTimeCatalog.h"
#include <UiDesigner/Core/UiDesignerDateTimeData.h>

namespace Upp {

void RegisterUiDesignerDateTimeCatalog(UiDesignerCatalog& catalog)
{
    if(catalog.Find("UiDateTime"))
        return;
    UiDesignerControlSpec spec;
    spec.type_id = "UiDateTime";
    spec.display_name = "Date / Time";
    spec.category = "Ui Controls";
    spec.runtime_cpp_type = "UiDateTime";
    spec.runtime_kind = UiDesignerRuntimeKind::UiDateTime;
    spec.default_base_name = "date_time";
    spec.help = "Local date/time, without timezone conversion. Inspector and Data edit one saved ISO value.";
    spec.icon_key = "controls";
    spec.default_size = Size(240, 32);
    spec.minimum_size = Size(150, 28);
    spec.preview_adapter_id = "runtime:UiDateTime";
    spec.codegen_adapter_id = "control";
    spec.child_adapter_id = "none";
    spec.adapter_backed_runtime = true;
    spec.data_capability = UiDesignerDataCapability::Scalar;
    spec.data_adapter_id = "scalar";
    spec.data_property_id = "datetime_value";
    spec.theme = true;
    spec.theme_adapter_id = "date_time";
    AddUiDesignerCommonProperties(spec);

    // Keep configuration, bounds and value in one stable group: the generic
    // Preview applies catalog properties in group order. Value is applied after
    // mode/seconds/range; nullable policy is applied only after the saved value.
    const auto Add = [&](UiDesignerPropertySpec property) {
        property.group = "Date / time";
        property.impact = PropertyImpactControlState | PropertyImpactLocalLayout |
                          PropertyImpactInspectorSchema | PropertyImpactCode;
        spec.defaults.Set(property.id, property.default_value);
        spec.properties.Add(pick(property));
    };
    UiDesignerPropertySpec mode;
    mode.id = "mode"; mode.label = "Mode";
    mode.kind = PropertyEditorKind::Choice; mode.default_value = "DateTime";
    mode.Choice("Date", "Date").Choice("Time", "Time").Choice("DateTime", "Date and time");
    Add(mode);
    UiDesignerPropertySpec format;
    format.id = "format_style"; format.label = "Display format";
    format.kind = PropertyEditorKind::Choice; format.default_value = "Iso";
    format.Choice("Locale", "Locale").Choice("Iso", "ISO");
    Add(format);
    UiDesignerPropertySpec clock;
    clock.id = "clock_format"; clock.label = "Clock";
    clock.kind = PropertyEditorKind::Choice; clock.default_value = "Hour24";
    clock.Choice("Locale", "Locale").Choice("Hour12", "12 hour").Choice("Hour24", "24 hour");
    clock.help = "ISO display always uses 24 hours. This option applies to Locale display.";
    Add(clock);
    Add(UiDesignerBoolProperty("show_seconds", "Show seconds", true));
    UiDesignerPropertySpec button_role;
    button_role.id = "button_role"; button_role.label = "Picker button role";
    button_role.kind = PropertyEditorKind::Choice; button_role.default_value = "Subtle";
    button_role.Choice("Standard", "Standard").Choice("Subtle", "Subtle")
               .Choice("Accent", "Accent").Choice("Alert", "Alert");
    Add(button_role);
    Add(UiDesignerNumberProperty("first_day", "First weekday (0 = Sunday)", 1, 0, 6, 1, PropertyEditorKind::Integer));
    for(const char* id : {"minimum_value", "maximum_value", "datetime_value"}) {
        UiDesignerPropertySpec value;
        value.id = id;
        value.label = String(id) == "minimum_value" ? "Minimum" :
                      String(id) == "maximum_value" ? "Maximum" : "Value";
        value.domain = PropertyEditorDomain::Content;
        value.Editor("designer.date-time.value");
        value.preserve_null = true;
        value.default_value = String(id) == "datetime_value"
            ? UiDesignerDateTimeDefaultValue() : Value();
        value.help = String(id) == "datetime_value"
            ? "One local ISO timestamp or null. Hidden date/time fields stay authored until this value is edited."
            : "Null means unbounded. The runtime normalizes bounds to the selected mode and orders them.";
        Add(value);
    }
    Add(UiDesignerBoolProperty("allow_null", "Allow empty value", true));
    Add(UiDesignerBoolProperty("allow_copy", "Allow copy", true));
    Add(UiDesignerBoolProperty("allow_paste", "Allow paste", true));
    Add(UiDesignerBoolProperty("presentation_frame", "Presentation frame", false));
    Add(UiDesignerBoolProperty("editable", "Editable", true));
    for(const char* id : {"WhenChanging", "WhenAction", "WhenOpenPicker"}) {
        UiDesignerEventSpec& event = spec.events.Add();
        event.id = id; event.label = id;
    }
    spec.capabilities |= UiDesignerCapabilityAcceptActions;
    catalog.Register(pick(spec));
}

void UiDesignerConfigureDateTimeEditor(const UiDesignerNode& node, PropertyEditorItem& item)
{
    if(item.custom_editor != "designer.date-time.value")
        return;
    const bool value = item.id == "datetime_value";
    item.editor_variant = AsString(node.GetProperty("mode", "DateTime")) + ":" +
        ((bool)node.GetProperty("show_seconds", true) ? "1" : "0") + ":" +
        (!value || (bool)node.GetProperty("allow_null", true) ? "1" : "0");
    if(value) {
        item.minimum = node.GetProperty("minimum_value", Value());
        item.maximum = node.GetProperty("maximum_value", Value());
    }
}

}
