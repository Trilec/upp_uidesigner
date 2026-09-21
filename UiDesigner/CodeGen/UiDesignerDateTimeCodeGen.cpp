#include "UiDesignerDateTimeCodeGen.h"
#include <UiDesigner/Core/UiDesignerDateTimeData.h>

namespace Upp {
namespace {
String DateTimeCpp(const Value& value)
{
    Time time;
    String error;
    // Catalog validation precedes code emission.
    if(!UiDesignerReadDateTimeValue(value, time, error) || IsNull(time))
        return "Time(Null)";
    return Format("Time(%d, %d, %d, %d, %d, %d)", (int)time.year,
                  (int)time.month, (int)time.day, (int)time.hour,
                  (int)time.minute, (int)time.second);
}
String DateTimeBool(const UiDesignerNode& node, const char* key, bool fallback)
{
    return (bool)node.GetProperty(key, fallback) ? "true" : "false";
}
}

void UiDesignerEmitDateTimeSetup(String& out, const String& member, const UiDesignerNode& node)
{
    out << "\t" << member << ".AllowNull(true).ClearRange();\n";
    out << "\t" << member << ".ShowSeconds(" << DateTimeBool(node, "show_seconds", true) << ");\n";
    out << "\t" << member << ".SetMode(UiDateTimeMode::" << AsString(node.GetProperty("mode", "DateTime")) << ");\n";
    out << "\t" << member << ".SetFormatStyle(UiDateTimeFormatStyle::" << AsString(node.GetProperty("format_style", "Iso")) << ");\n";
    out << "\t" << member << ".SetClockFormat(UiClockFormat::" << AsString(node.GetProperty("clock_format", "Hour24")) << ");\n";
    out << "\t" << member << ".SetFirstDayOfWeek(" << (int)node.GetProperty("first_day", 1) << ");\n";
    out << "\t" << member << ".SetRange(" << DateTimeCpp(node.GetProperty("minimum_value", Value()))
        << ", " << DateTimeCpp(node.GetProperty("maximum_value", Value())) << ");\n";
    out << "\t" << member << ".SetValue(" << DateTimeCpp(node.GetProperty("datetime_value", UiDesignerDateTimeDefaultValue())) << ");\n";
    for(const auto& property : {"allow_null", "allow_copy", "allow_paste"}) {
        const String method = String(property) == "allow_null" ? "AllowNull" :
                              String(property) == "allow_copy" ? "AllowCopy" : "AllowPaste";
        out << "\t" << member << "." << method << "(" << DateTimeBool(node, property, true) << ");\n";
    }
    out << "\t" << member << ".ShowPresentationFrame(" << DateTimeBool(node, "presentation_frame", false) << ");\n";
    out << "\t" << member << ".SetEditable(" << DateTimeBool(node, "editable", true) << ");\n";
}

}
