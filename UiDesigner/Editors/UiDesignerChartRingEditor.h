#ifndef _UiDesigner_Editors_UiDesignerChartRingEditor_h_
#define _UiDesigner_Editors_UiDesignerChartRingEditor_h_

#include <UiDesigner/Core/UiDesignerChartRingData.h>
#include <Utilities/PropertyEditor/PropertyValueEditors.h>

namespace Upp {

// Disposable dialog working copy. Only its accepted Value enters the existing
// Session command path; this is neither persistent state nor a second history.
class UiDesignerChartRingDraft {
public:
    bool Set(const Value& value, String& error);
    ValueArray GetValue() const;
    int GetCount() const { return segments_.GetCount(); }
    const UiDesignerChartRingSegment& Get(int index) const { return segments_[index]; }
    bool Append(double value, const String& label, Color color, String& error);
    bool Replace(int index, double value, const String& label, Color color, String& error);
    bool Remove(int index);
    bool Move(int from, int to);

private:
    Vector<UiDesignerChartRingSegment> segments_;
};

void RegisterUiDesignerChartRingEditor();

}

#endif
