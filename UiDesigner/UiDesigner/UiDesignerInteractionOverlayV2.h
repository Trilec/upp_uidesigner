#ifndef _Utilities_UiDesigner_UiDesigner_UiDesignerInteractionOverlayV2_h_
#define _Utilities_UiDesigner_UiDesigner_UiDesignerInteractionOverlayV2_h_

#include "UiDesignerInteractionOverlay.h"
#include <UiDesigner/Preview/UiDesignerSelectionHit.h>

namespace Upp {

class UiDesignerWindow;

class UiDesignerInteractionOverlayV2 : public UiDesignerInteractionOverlay {
public:
    typedef UiDesignerInteractionOverlayV2 CLASSNAME;

    explicit UiDesignerInteractionOverlayV2(UiDesignerWindow& owner);

    bool FinishCatalogDrag(const String& type_id, Point screen);

    virtual void Paint(Draw& w) override;
    virtual void LeftDown(Point p, dword keyflags) override;
    virtual void LeftDrag(Point p, dword keyflags) override;
    virtual void MouseMove(Point p, dword keyflags) override;
    virtual void LeftUp(Point p, dword keyflags) override;
    virtual Image CursorImage(Point p, dword keyflags) override;
    virtual bool Key(dword key, int count) override;
    virtual void CancelMode() override;

private:
    bool IsRootResizePoint(Point p) const;
    Point CanvasPoint(Point p) const;
    UiDesignerNodeId ResolveClickSelection(Point p, dword keyflags);
    void ArmMove(Point p, UiDesignerNodeId selected);
    void BeginMove(Point p);
    void UpdateMove(Point p);
    void FinishMove();
    void CancelMove();
    void ReleaseMoveCapture();
    bool GridCellBlocked(const UiDesignerDropRegion& region) const;

    UiDesignerWindow *owner_v2_ = nullptr;
    Point press_point_;
    Point cycle_point_;
    bool cycle_valid_ = false;
    bool move_armed_ = false;
    bool moving_ = false;
    bool move_capture_owned_ = false;
    bool releasing_capture_ = false;
    bool delegating_base_ = false;
    Vector<UiDesignerNodeId> move_nodes_;
    UiDesignerDropPlan move_plan_;
    Rect move_visual_rect_;
    String move_reason_;
};

}

#endif
