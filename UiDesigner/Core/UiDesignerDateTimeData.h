#ifndef _UiDesigner_Core_UiDesignerDateTimeData_h_
#define _UiDesigner_Core_UiDesignerDateTimeData_h_

#include "UiDesignerDocument.h"
#include <cmath>

namespace Upp {

inline Value UiDesignerDateTimeDefaultValue() { return "2000-01-01T12:00:00"; }

// Transport-safe, local (not UTC) ISO value. Null is distinct from a timestamp.
// A string is used at the document boundary so existing JSON, command snapshots,
// CLI and MCP all preserve exactly the same value without locale conversion.
inline Value UiDesignerDateTimeValue(Time value)
{
    if(IsNull(value))
        return Value();
    return Format("%04d-%02d-%02dT%02d:%02d:%02d", (int)value.year,
                  (int)value.month, (int)value.day, (int)value.hour,
                  (int)value.minute, (int)value.second);
}

inline bool UiDesignerReadDateTimeValue(const Value& value, Time& out, String& error)
{
    if(IsNull(value)) {
        out = Null;
        error.Clear();
        return true;
    }
    if(!value.Is<String>()) {
        error = "DateTime value must be a local ISO timestamp or null";
        return false;
    }
    const String text = value;
    if(text.GetCount() != 19 || text[4] != '-' || text[7] != '-' ||
       text[10] != 'T' || text[13] != ':' || text[16] != ':') {
        error = "Use YYYY-MM-DDTHH:MM:SS, without a timezone suffix";
        return false;
    }
    const int starts[] = {0, 5, 8, 11, 14, 17};
    const int lengths[] = {4, 2, 2, 2, 2, 2};
    int fields[6] = {};
    for(int f = 0; f < 6; ++f)
        for(int j = 0; j < lengths[f]; ++j) {
            const int c = text[starts[f] + j];
            if(c < '0' || c > '9') {
                error = "DateTime fields must contain decimal digits";
                return false;
            }
            fields[f] = fields[f] * 10 + c - '0';
        }
    // Validate before narrowing to U++ Date/Time's byte-sized fields.
    if(fields[0] < 1 || fields[1] < 1 || fields[1] > 12 ||
       fields[2] < 1 || fields[2] > 31 || fields[3] > 23 ||
       fields[4] > 59 || fields[5] > 59) {
        error = "DateTime fields are outside their supported ranges";
        return false;
    }
    const Time decoded(fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
    if(!decoded.IsValid()) {
        error = "DateTime is not a valid calendar date/time";
        return false;
    }
    out = decoded;
    error.Clear();
    return true;
}

inline bool UiDesignerValidateDateTimeNode(const UiDesignerNode& node, String& error)
{
    Time decoded;
    for(const char* key : {"datetime_value", "minimum_value", "maximum_value"}) {
        if(!UiDesignerReadDateTimeValue(node.GetProperty(key, String(key) == "datetime_value"
                ? UiDesignerDateTimeDefaultValue() : Value()), decoded, error)) {
            error = String(key) + ": " + error;
            return false;
        }
    }
    for(const char* key : {"allow_null", "show_seconds", "allow_copy", "allow_paste",
                           "presentation_frame", "editable"}) {
        const Value value = node.GetProperty(key, String(key) != "presentation_frame");
        if(!value.Is<bool>()) {
            error = String(key) + " must be Boolean";
            return false;
        }
    }
    if(!(bool)node.GetProperty("allow_null", true) &&
       IsNull(node.GetProperty("datetime_value", UiDesignerDateTimeDefaultValue()))) {
        error = "Set a DateTime value before disabling empty values";
        return false;
    }
    const String button_role = AsString(node.GetProperty("button_role", "Subtle"));
    if(button_role != "Standard" && button_role != "Subtle" &&
       button_role != "Accent" && button_role != "Alert") {
        error = "Unknown DateTime picker button role";
        return false;
    }
    const String mode = AsString(node.GetProperty("mode", "DateTime"));
    if(mode != "Date" && mode != "Time" && mode != "DateTime") {
        error = "Unknown DateTime mode";
        return false;
    }
    const String format = AsString(node.GetProperty("format_style", "Iso"));
    const String clock = AsString(node.GetProperty("clock_format", "Hour24"));
    if((format != "Locale" && format != "Iso") ||
       (clock != "Locale" && clock != "Hour12" && clock != "Hour24")) {
        error = "Unknown DateTime display format";
        return false;
    }
    const Value first = node.GetProperty("first_day", 1);
    if(!(first.Is<int>() || first.Is<int64>() || first.Is<double>()) ||
       IsNull(first) || !std::isfinite((double)first) || (double)first < 0 || (double)first > 6 ||
       (double)first != (int)(double)first) {
        error = "First day must be an integer from 0 (Sunday) to 6 (Saturday)";
        return false;
    }
    error.Clear();
    return true;
}

}

#endif
