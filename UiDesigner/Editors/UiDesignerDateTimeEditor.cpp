#include "UiDesignerDateTimeEditor.h"
#include <UiDesigner/Core/UiDesignerDateTimeData.h>
#include <Utilities/PropertyEditor/PropertyValueEditors.h>
#include <Ui/UiDateTime.h>

namespace Upp {
namespace {

class DateTimeValueEditor final : public PropertyValueEditor {
public:
    DateTimeValueEditor()
    {
        Add(field_);
        Add(apply_);
        apply_.SetText("Apply");
        apply_.Tip("Commit a typed date/time value");
        field_.AllowNull(true).ClearValue();
        field_.WhenAction = [this] { Commit(); };
        // UiDateTime publishes live typed input through WhenChanging. Its final
        // WhenAction can be absent when the live value already equals the text;
        // provide an explicit commit control without reaching into private children.
        apply_.WhenAction = [this] { Commit(); };
    }

    void Layout() override
    {
        const int width = min(DPI(52), max(0, GetSize().cx));
        const int gap = min(DPI(4), max(0, GetSize().cx - width));
        field_.SetRect(0, 0, max(0, GetSize().cx - width - gap), GetSize().cy);
        apply_.SetRect(GetSize().cx - width, 0, width, GetSize().cy);
    }

    void Configure(const PropertyEditorItem& item) override
    {
        loading_ = true;
        const Vector<String> parts = Split(item.editor_variant, ':');
        const String mode = parts.IsEmpty() ? String("DateTime") : parts[0];
        field_.AllowNull(true).ClearRange();
        field_.ShowSeconds(parts.GetCount() < 2 || parts[1] == "1");
        field_.SetMode(mode == "Date" ? UiDateTimeMode::Date :
                       mode == "Time" ? UiDateTimeMode::Time : UiDateTimeMode::DateTime);
        field_.SetFormatStyle(UiDateTimeFormatStyle::Iso).SetClockFormat(UiClockFormat::Hour24);
        nullable_ = parts.GetCount() < 3 || parts[2] == "1";
        Time low = Null, high = Null;
        String error;
        if(UiDesignerReadDateTimeValue(item.minimum, low, error) &&
           UiDesignerReadDateTimeValue(item.maximum, high, error))
            field_.SetRange(low, high);
        field_.SetEditable(!item.read_only && item.value_editable);
        enabled_ = item.enabled && !item.read_only && item.value_editable;
        field_.Enable(enabled_);
        apply_.Enable(enabled_);
        loading_ = false;
    }

    void SetEditorValue(const Value& value, bool mixed) override
    {
        loading_ = true;
        original_ = value;
        mixed_ = mixed;
        Time decoded;
        String error;
        valid_ = UiDesignerReadDateTimeValue(value, decoded, error);
        field_.AllowNull(true);
        if(valid_) {
            field_.SetValue(decoded);
            if(!nullable_ && !field_.IsNullValue())
                field_.AllowNull(false);
            loaded_value_ = field_.GetValue();
        }
        else {
            field_.ClearValue();
            loaded_value_ = Null;
        }
        field_.Tip(!valid_ ? error : mixed ? String("Mixed values; an explicit edit applies to the selection") : String());
        field_.Enable(enabled_ && valid_);
        apply_.Enable(enabled_ && valid_);
        loading_ = false;
    }

    Value GetEditorValue() const override
    {
        // Do not round hidden seconds/date components merely by focusing or
        // opening a value. Only an actual picker/text change replaces the source.
        if(!valid_ || field_.GetValue() == loaded_value_)
            return original_;
        return UiDesignerDateTimeValue(field_.GetValue());
    }

    void FocusEditor() override
    {
        if(Ctrl* edit = field_.GetFirstChild())
            edit->SetFocus();
    }

private:
    void Commit()
    {
        if(loading_ || !valid_ || !enabled_) return;
        const Value accepted = GetEditorValue();
        if(!mixed_ && accepted == original_) return;
        original_ = accepted;
        mixed_ = false;
        loaded_value_ = field_.GetValue();
        // A command can synchronously rebuild/destroy this editor.
        WhenCommit(accepted);
    }

    UiDateTime field_;
    UiButton apply_;
    Value original_;
    Time loaded_value_ = Null;
    bool nullable_ = true;
    bool mixed_ = false;
    bool loading_ = false;
    bool valid_ = true;
    bool enabled_ = true;
};

}

void RegisterUiDesignerDateTimeEditor()
{
    auto& factory = PropertyEditorFactory::Global();
    if(!factory.HasCustom("designer.date-time.value"))
        factory.RegisterCustom("designer.date-time.value", [] {
            return One<PropertyValueEditor>(new DateTimeValueEditor);
        });
}

}
