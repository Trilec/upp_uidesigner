#ifndef _UiDesigner_Core_UiDesignerChartRingData_h_
#define _UiDesigner_Core_UiDesignerChartRingData_h_

#include "UiDesignerDocument.h"
#include <cmath>

namespace Upp {

// One ordered, document-owned collection. Null colour means use the runtime
// theme's series colour; it is not a saved copy of the currently resolved colour.
struct UiDesignerChartRingSegment : Moveable<UiDesignerChartRingSegment> {
    double value = 0.0;
    String label;
    Color color = Null;
};

inline Value UiDesignerChartRingSegmentValue(double value, const String& label,
                                             Color color = Null)
{
    ValueMap row;
    row.Set("value", value);
    row.Set("label", label);
    row.Set("color", IsNull(color) ? Value() : Value(color));
    return row;
}

inline ValueArray UiDesignerChartRingSegmentsValue(
    const Vector<UiDesignerChartRingSegment>& segments)
{
    ValueArray result;
    for(const auto& segment : segments)
        result.Add(UiDesignerChartRingSegmentValue(
            segment.value, segment.label, segment.color));
    return result;
}

inline bool UiDesignerChartRingNumber(const Value& value, double& result)
{
    if(IsNull(value) || !(value.Is<int>() || value.Is<int64>() || value.Is<double>()))
        return false;
    result = (double)value;
    return std::isfinite(result) && result >= 0.0;
}

// Transactional decoding: failure never partially replaces the destination.
// Do not silently coerce strings, discard malformed records, or clamp bad data.
inline bool UiDesignerReadChartRingSegments(
    const Value& value, Vector<UiDesignerChartRingSegment>& result, String& error)
{
    if(!value.Is<ValueArray>()) {
        error = "ChartRing segments must be an array";
        return false;
    }
    Vector<UiDesignerChartRingSegment> decoded;
    const ValueArray rows = value;
    double sum = 0.0;
    for(int i = 0; i < rows.GetCount(); ++i) {
        const String prefix = "ChartRing segment " + AsString(i + 1) + ": ";
        if(!rows[i].Is<ValueMap>()) {
            error = prefix + "expected an object";
            return false;
        }
        const ValueMap row = rows[i];
        for(int k = 0; k < row.GetCount(); ++k) {
            const Value key = row.GetKey(k);
            if(!key.Is<String>() || (key != "value" && key != "label" && key != "color")) {
                error = prefix + "unknown field " + AsString(key);
                return false;
            }
        }
        UiDesignerChartRingSegment segment;
        if(!UiDesignerChartRingNumber(UiDesignerMapValue(row, "value"), segment.value)) {
            error = prefix + "value must be a finite non-negative number";
            return false;
        }
        const Value label = UiDesignerMapValue(row, "label", String());
        if(!label.Is<String>()) {
            error = prefix + "label must be text";
            return false;
        }
        segment.label = (String)label;
        const Value color = UiDesignerMapValue(row, "color");
        if(!IsNull(color) && !color.Is<Color>()) {
            error = prefix + "color must be Color or null (automatic)";
            return false;
        }
        segment.color = IsNull(color) ? Color(Null) : (Color)color;
        sum += segment.value;
        if(!std::isfinite(sum)) {
            error = prefix + "segment sum exceeds the numeric range";
            return false;
        }
        decoded.Add(pick(segment));
    }
    result = pick(decoded);
    error.Clear();
    return true;
}

inline bool UiDesignerValidateChartRingNode(const UiDesignerNode& node, String& error)
{
    Vector<UiDesignerChartRingSegment> segments;
    if(!UiDesignerReadChartRingSegments(
           node.GetProperty("segments", ValueArray()), segments, error))
        return false;
    double total = 0.0;
    if(!UiDesignerChartRingNumber(node.GetProperty("explicit_total", 0.0), total)) {
        error = "ChartRing explicit total must be a finite non-negative number (0 = automatic)";
        return false;
    }
    if(!node.GetProperty("center_text", String()).Is<String>()) {
        error = "ChartRing center text must be text";
        return false;
    }
    error.Clear();
    return true;
}

}

#endif
