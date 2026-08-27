#include "UiDesignerSelectionHit.h"

namespace Upp {

Vector<UiDesignerNodeId> UiDesignerPreviewSelectionStack(
    const UiDesignerGeometrySnapshot& geometry,
    const UiDesignerDocument& document,
    Point canvas_point)
{
    Vector<UiDesignerNodeId> result;
    UiDesignerNodeId id = geometry.Hit(canvas_point);
    const UiDesignerNode *node = document.Find(id);
    while(node) {
        const UiDesignerGeometryRecord *record = geometry.Find(node->id);
        if(record && record->selectable && record->rect.Contains(canvas_point))
            result.Add(node->id);
        if(!node->parent)
            break;
        node = document.Find(node->parent);
    }
    return result;
}

}
