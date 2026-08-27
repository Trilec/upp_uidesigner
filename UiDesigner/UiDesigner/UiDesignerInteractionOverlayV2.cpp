#include "UiDesignerInteractionOverlayV2.h"
#include "UiDesignerWindow.h"

namespace Upp {

UiDesignerInteractionOverlayV2::UiDesignerInteractionOverlayV2(UiDesignerWindow& owner)
    : UiDesignerInteractionOverlay(owner), owner_v2_(&owner)
{
}

bool UiDesignerInteractionOverlayV2::IsRootResizePoint(Point p) const
{
    if(!owner_v2_)
        return false;
    const Rect root = owner_v2_->preview_canvas_.GetRect();
    const int grab = DPI(5);
    if(!root.Inflated(grab).Contains(p))
        return false;
    return abs(p.x - root.left) <= grab || abs(p.x - root.right) <= grab ||
           abs(p.y - root.top) <= grab || abs(p.y - root.bottom) <= grab;
}

Point UiDesignerInteractionOverlayV2::CanvasPoint(Point p) const
{
    return owner_v2_ ? p - owner_v2_->preview_canvas_.GetRect().TopLeft() : p;
}

UiDesignerNodeId UiDesignerInteractionOverlayV2::ResolveClickSelection(
    Point p, dword keyflags)
{
    if(!owner_v2_)
        return 0;
    const Vector<UiDesignerNodeId> stack = UiDesignerPreviewSelectionStack(
        owner_v2_->preview_canvas_.GetGeometrySnapshot(),
        owner_v2_->session_.Document(), CanvasPoint(p));
    if(stack.IsEmpty()) {
        cycle_valid_ = false;
        return 0;
    }

    UiDesignerNodeId selected = stack[0];
    if(!(keyflags & K_CTRL) && cycle_valid_ &&
       Length(p - cycle_point_) <= DPI(3)) {
        const UiDesignerNodeId current = owner_v2_->session_.State().selection.primary;
        const int q = FindIndex(stack, current);
        if(q >= 0)
            selected = stack[(q + 1) % stack.GetCount()];
    }

    if(keyflags & K_CTRL) {
        owner_v2_->session_.Select(selected, true);
        cycle_valid_ = false;
    }
    else {
        owner_v2_->session_.Select(selected, false);
        cycle_point_ = p;
        cycle_valid_ = true;
    }
    owner_v2_->RefreshStatus(Format("select node=%d", (int)selected));
    return selected;
}

void UiDesignerInteractionOverlayV2::ArmMove(Point p, UiDesignerNodeId selected)
{
    move_armed_ = false;
    moving_ = false;
    move_nodes_.Clear();
    move_plan_ = UiDesignerDropPlan();
    move_visual_rect_ = Rect();
    move_reason_.Clear();
    if(!owner_v2_ || !selected ||
       selected == owner_v2_->session_.Document().GetRootId() ||
       !owner_v2_->session_.State().selection.Contains(selected))
        return;

    move_nodes_ = clone(owner_v2_->session_.State().selection.nodes);
    for(UiDesignerNodeId id : move_nodes_)
        if(id == owner_v2_->session_.Document().GetRootId()) {
            move_nodes_.Clear();
            return;
        }
    if(move_nodes_.IsEmpty())
        return;

    press_point_ = p;
    move_armed_ = true;
    if(!HasCapture())
        move_capture_owned_ = SetCapture();
}

void UiDesignerInteractionOverlayV2::LeftDown(Point p, dword keyflags)
{
    if(IsRootResizePoint(p)) {
        cycle_valid_ = false;
        delegating_base_ = true;
        UiDesignerInteractionOverlay::LeftDown(p, keyflags);
        return;
    }

    delegating_base_ = false;
    const UiDesignerNodeId selected = ResolveClickSelection(p, keyflags);
    if(selected)
        ArmMove(p, selected);
    SetFocus();
}

void UiDesignerInteractionOverlayV2::BeginMove(Point p)
{
    if(!move_armed_ || moving_)
        return;
    moving_ = true;
    cycle_valid_ = false;
    if(owner_v2_)
        owner_v2_->RefreshStatus(move_nodes_.GetCount() == 1
            ? "move control" : "move selection");
    UpdateMove(p);
}

bool UiDesignerInteractionOverlayV2::GridCellBlocked(
    const UiDesignerDropRegion& region) const
{
    if(!owner_v2_ || region.kind != UiDesignerDropRegionKind::GridCell ||
       !region.occupied)
        return false;
    const UiDesignerNode *grid = owner_v2_->session_.Document().Find(region.owner);
    if(!grid)
        return true;
    for(UiDesignerNodeId child_id : grid->children) {
        const UiDesignerNode *child = owner_v2_->session_.Document().Find(child_id);
        if(!child)
            continue;
        if((int)child->GetProperty("grid_row", -1) == region.grid_row &&
           (int)child->GetProperty("grid_column", -1) == region.grid_column &&
           FindIndex(move_nodes_, child_id) < 0)
            return true;
    }
    return false;
}

void UiDesignerInteractionOverlayV2::UpdateMove(Point p)
{
    if(!owner_v2_ || !moving_)
        return;

    move_plan_ = UiDesignerDropPlan();
    move_visual_rect_ = Rect();
    move_reason_.Clear();

    const Rect root = owner_v2_->preview_canvas_.GetRect();
    if(!root.Contains(p)) {
        move_reason_ = "Move target is outside the Window";
        owner_v2_->RefreshStatus(move_reason_);
        Refresh();
        return;
    }

    const Point canvas = CanvasPoint(p);
    const UiDesignerGeometrySnapshot& geometry =
        owner_v2_->preview_canvas_.GetGeometrySnapshot();
    const UiDesignerDropRegion *region = geometry.HitDropRegion(canvas);
    if(!region) {
        move_reason_ = "Move target has no drop region";
        owner_v2_->RefreshStatus(move_reason_);
        Refresh();
        return;
    }

    move_visual_rect_ = region->visual_rect.IsEmpty() ? region->rect
                                                       : region->visual_rect;
    if(GridCellBlocked(*region)) {
        move_reason_ = "Grid cell is occupied";
        owner_v2_->RefreshStatus(move_reason_);
        Refresh();
        return;
    }

    Point position = canvas;
    if(region->owner != owner_v2_->session_.Document().GetRootId()) {
        const UiDesignerGeometryRecord *target = geometry.Find(region->owner);
        if(target)
            position -= target->rect.TopLeft();
    }

    move_plan_ = owner_v2_->session_.PlanMoveSelection(
        region->owner, position, true, region->insertion_index,
        region->grid_row, region->grid_column);
    move_reason_ = move_plan_.reason;
    owner_v2_->RefreshStatus(move_plan_.valid
        ? region->label + " : valid move"
        : (move_reason_.IsEmpty() ? "Invalid move" : move_reason_));
    Refresh();
}

void UiDesignerInteractionOverlayV2::LeftDrag(Point p, dword keyflags)
{
    if(delegating_base_) {
        UiDesignerInteractionOverlay::MouseMove(p, keyflags);
        return;
    }
    if(move_armed_) {
        BeginMove(p);
        UpdateMove(p);
    }
}

void UiDesignerInteractionOverlayV2::MouseMove(Point p, dword keyflags)
{
    if(delegating_base_) {
        UiDesignerInteractionOverlay::MouseMove(p, keyflags);
        return;
    }
    if(moving_) {
        UpdateMove(p);
        return;
    }
    if(move_armed_ && (keyflags & K_MOUSELEFT) &&
       Length(p - press_point_) >= DPI(5)) {
        BeginMove(p);
        UpdateMove(p);
    }
}

void UiDesignerInteractionOverlayV2::ReleaseMoveCapture()
{
    if(!move_capture_owned_)
        return;
    releasing_capture_ = true;
    if(HasCapture())
        ReleaseCapture();
    move_capture_owned_ = false;
    releasing_capture_ = false;
}

void UiDesignerInteractionOverlayV2::FinishMove()
{
    if(!owner_v2_)
        return;
    const UiDesignerDropPlan plan = move_plan_;
    const bool valid = moving_ && plan.valid;
    const String reason = move_reason_;

    move_armed_ = false;
    moving_ = false;
    move_nodes_.Clear();
    move_plan_ = UiDesignerDropPlan();
    move_visual_rect_ = Rect();
    move_reason_.Clear();
    ReleaseMoveCapture();

    if(!valid) {
        owner_v2_->RefreshStatus(reason.IsEmpty() ? "Move cancelled" : reason);
        Refresh();
        return;
    }

    String error;
    UiDesignerNodeId created = 0;
    if(owner_v2_->session_.ExecuteDrop(plan, &created, error))
        owner_v2_->RefreshStatus("Move completed");
    else
        owner_v2_->RefreshStatus(error.IsEmpty() ? "Move failed" : error);
    Refresh();
}

void UiDesignerInteractionOverlayV2::CancelMove()
{
    const bool active = move_armed_ || moving_;
    move_armed_ = false;
    moving_ = false;
    move_nodes_.Clear();
    move_plan_ = UiDesignerDropPlan();
    move_visual_rect_ = Rect();
    move_reason_.Clear();
    ReleaseMoveCapture();
    if(active && owner_v2_)
        owner_v2_->RefreshStatus("Move cancelled");
    Refresh();
}

void UiDesignerInteractionOverlayV2::LeftUp(Point p, dword keyflags)
{
    if(delegating_base_) {
        UiDesignerInteractionOverlay::LeftUp(p, keyflags);
        delegating_base_ = false;
        return;
    }
    if(moving_)
        FinishMove();
    else if(move_armed_) {
        move_armed_ = false;
        move_nodes_.Clear();
        ReleaseMoveCapture();
    }
}

void UiDesignerInteractionOverlayV2::Paint(Draw& w)
{
    UiDesignerInteractionOverlay::Paint(w);
    if(!moving_ || move_visual_rect_.IsEmpty() || !owner_v2_)
        return;
    Rect r = move_visual_rect_.Offseted(owner_v2_->preview_canvas_.GetRect().TopLeft());
    const Color color = move_plan_.valid ? Color(34, 197, 94) : Color(220, 38, 38);
    const int t = DPI(3);
    w.DrawRect(r.left, r.top, r.Width(), t, color);
    w.DrawRect(r.left, r.bottom - t, r.Width(), t, color);
    w.DrawRect(r.left, r.top, t, r.Height(), color);
    w.DrawRect(r.right - t, r.top, t, r.Height(), color);
}

Image UiDesignerInteractionOverlayV2::CursorImage(Point p, dword keyflags)
{
    if(moving_)
        return Image::SizeAll();
    return UiDesignerInteractionOverlay::CursorImage(p, keyflags);
}

bool UiDesignerInteractionOverlayV2::Key(dword key, int count)
{
    if(key == K_ESCAPE && (move_armed_ || moving_)) {
        CancelMove();
        return true;
    }
    const bool handled = UiDesignerInteractionOverlay::Key(key, count);
    if(key == K_ESCAPE && delegating_base_ && handled)
        delegating_base_ = false;
    return handled;
}

void UiDesignerInteractionOverlayV2::CancelMode()
{
    if(releasing_capture_)
        return;
    if(move_armed_ || moving_) {
        CancelMove();
        return;
    }
    UiDesignerInteractionOverlay::CancelMode();
    delegating_base_ = false;
}

}
