#include "UiDesignerPreview.h"
#include <UiDesigner/Core/UiDesignerDateTimeData.h>
#include <Ui/UiDateTime.h>

namespace Upp {
namespace {

UiRole DateTimePreviewRole(const Value& value)
{
    const String role = AsString(value);
    return role == "Accent" ? UiRole::Accent : role == "Alert" ? UiRole::Alert :
           role == "Subtle" ? UiRole::Subtle : UiRole::Standard;
}

UiDesignerApplyResult ApplyDateTime(Ctrl& ctrl, const UiDesignerControlSpec&,
                                    const String& property, const Value& value)
{
    auto* field = dynamic_cast<UiDateTime*>(&ctrl);
    if(!field)
        return UiDesignerApplyResult::Rejected;
    if(property == "visible") { field->Show((bool)value); return UiDesignerApplyResult::AppliedControlState; }
    if(property == "enabled") { field->Enable((bool)value); return UiDesignerApplyResult::AppliedControlState; }
    if(property == "role") { field->SetRole(DateTimePreviewRole(value)); return UiDesignerApplyResult::AppliedPaint; }
    if(property == "mode") {
        const String mode = AsString(value);
        if(mode != "Date" && mode != "Time" && mode != "DateTime") return UiDesignerApplyResult::Rejected;
        field->SetMode(mode == "Date" ? UiDateTimeMode::Date : mode == "Time" ? UiDateTimeMode::Time : UiDateTimeMode::DateTime);
    }
    else if(property == "show_seconds") field->ShowSeconds((bool)value);
    else if(property == "format_style") field->SetFormatStyle(AsString(value) == "Iso" ? UiDateTimeFormatStyle::Iso : UiDateTimeFormatStyle::Locale);
    else if(property == "clock_format") field->SetClockFormat(AsString(value) == "Hour12" ? UiClockFormat::Hour12 : AsString(value) == "Hour24" ? UiClockFormat::Hour24 : UiClockFormat::Locale);
    else if(property == "button_role") field->SetButtonRole(DateTimePreviewRole(value));
    else if(property == "first_day") field->SetFirstDayOfWeek((int)value);
    else if(property == "datetime_value" || property == "minimum_value" || property == "maximum_value") {
        Time decoded;
        String error;
        if(!UiDesignerReadDateTimeValue(value, decoded, error)) return UiDesignerApplyResult::Rejected;
        if(property == "minimum_value") field->SetRange(decoded, field->GetMaximum());
        else if(property == "maximum_value") field->SetRange(field->GetMinimum(), decoded);
        else {
            // Permit clearing while reconstructing; the authored nullable policy
            // is applied after the value, never seeded from the system clock.
            field->AllowNull(true).SetValue(decoded);
        }
    }
    else if(property == "allow_null") {
        if(!(bool)value && field->IsNullValue()) return UiDesignerApplyResult::Rejected;
        field->AllowNull((bool)value);
    }
    else if(property == "editable") field->SetEditable((bool)value);
    else if(property == "presentation_frame") field->ShowPresentationFrame((bool)value);
    else if(property == "allow_copy") field->AllowCopy((bool)value);
    else if(property == "allow_paste") field->AllowPaste((bool)value);
    else return UiDesignerApplyResult::Rejected;
    // Reapply this one subtree from canonical data after configuration edits.
    // SetMode/ShowSeconds/SetRange normalize the runtime value; rebuilding also
    // restores hidden authored fields when the user switches back.
    return UiDesignerApplyResult::RequiresSubtreeRebuild;
}

struct RegisterDateTimePreview {
    RegisterDateTimePreview()
    {
        UiDesignerPreviewAdapter adapter;
        adapter.id = "runtime:UiDateTime";
        adapter.create = [] {
            auto* field = new UiDateTime;
            field->AllowNull(true).ClearValue();
            return One<Ctrl>(field);
        };
        adapter.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) { ctrl.Tip(spec.help); };
        adapter.apply = ApplyDateTime;
        UiDesignerPreviewAdapterRegistry::Global().Register(pick(adapter));
    }
};
RegisterDateTimePreview s_date_time_preview;

}
}
