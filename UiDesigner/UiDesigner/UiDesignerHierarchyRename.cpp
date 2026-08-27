#include "UiDesignerWidgets.h"

namespace Upp {

void UiDesignerHierarchyView::HierarchyTree::LeftDouble(Point p, dword flags)
{
    // A double-click begins with the same first button-down as a potential
    // manual drag. Tear that gesture down before handing the second click to
    // UiTree so its rename-on-double-click contract can acquire focus/editing.
    ResetManualDrag(false);
    ReleaseManualCapture();
    UiTree::LeftDouble(p, flags);
}

}
