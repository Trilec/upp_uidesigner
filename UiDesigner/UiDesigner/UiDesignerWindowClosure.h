#ifndef _Utilities_UiDesigner_UiDesigner_UiDesignerWindowClosure_h_
#define _Utilities_UiDesigner_UiDesigner_UiDesignerWindowClosure_h_

namespace Upp {

class UiDesignerWindow;

// Bounded closure wiring kept out of the already-large Window implementation.
// It attaches after UiDesignerWindow::ConnectServices so it can extend the
// existing event pipeline without replacing any established handlers.
class UiDesignerWindowClosureHook {
public:
    explicit UiDesignerWindowClosureHook(UiDesignerWindow& owner);
};

}

#endif
