#ifndef _Utilities_UiDesigner_Preview_UiDesignerSelectionHit_h_
#define _Utilities_UiDesigner_Preview_UiDesignerSelectionHit_h_

#include "UiDesignerGeometrySnapshot.h"

namespace Upp {

// Returns the selectable preview stack at a canvas point from deepest runtime
// node outward through its authored ancestors. This lets the Designer cycle
// through fully overlapping containers/layouts without requiring a visible
// inset or debug rail for every structural node.
Vector<UiDesignerNodeId> UiDesignerPreviewSelectionStack(
    const UiDesignerGeometrySnapshot& geometry,
    const UiDesignerDocument& document,
    Point canvas_point);

}

#endif
