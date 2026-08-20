#include "UiDesignerGeometrySnapshot.h"
namespace Upp {
const UiDesignerGeometryRecord* UiDesignerGeometrySnapshot::Find(UiDesignerNodeId node) const {
    for(const auto& record : records_)
        if(record.node == node) return &record;
    return nullptr;
}

const UiDesignerDropRegion* UiDesignerGeometrySnapshot::FindDropRegion(int region_id) const {
    for(const auto& region : drop_regions_)
        if(region.paint_order == region_id)
            return &region;
    return nullptr;
}

static bool BetterSelectionRecord(const UiDesignerGeometryRecord& candidate,
                                  const UiDesignerGeometryRecord *best)
{
    return !best || candidate.depth > best->depth ||
           (candidate.depth == best->depth && candidate.order > best->order);
}

static bool HitLayoutSelectionRegion(const UiDesignerGeometryRecord& record,
                                     Point p)
{
    if(!record.selectable || record.cue_kind != UiDesignerCueKind::LayoutBounds ||
       record.rect.IsEmpty() || !record.rect.Contains(p))
        return false;

    // Inset and gap space belongs to the transparent layout, even if a nested
    // child happens to publish overlapping geometry. This makes the visible
    // spacing in Box/Grid layouts a reliable selection target.
    for(const Rect& inset : record.inset_rects)
        if(inset.Contains(p))
            return true;
    for(const Rect& gap : record.gap_rects)
        if(gap.Contains(p))
            return true;

    // A transparent layout can legitimately have zero inset/gap and a child
    // stretched over its entire body. Reserve a narrow perimeter rail so the
    // layout is still directly selectable without forcing the user back to the
    // hierarchy. The deepest layout rail wins when nested layout edges align.
    const int rail = DPI(4);
    const Rect inner = record.rect.Deflated(rail);
    return inner.IsEmpty() || !inner.Contains(p);
}

UiDesignerNodeId UiDesignerGeometrySnapshot::Hit(Point p) const {
    const UiDesignerGeometryRecord* best_layout = nullptr;
    for(const auto& record : records_)
        if(HitLayoutSelectionRegion(record, p) &&
           BetterSelectionRecord(record, best_layout))
            best_layout = &record;
    if(best_layout)
        return best_layout->node;

    const UiDesignerGeometryRecord* best = nullptr;
    for(const auto& record : records_)
        if(record.selectable && record.rect.Contains(p) &&
           BetterSelectionRecord(record, best))
            best = &record;
    return best ? best->node : 0;
}

static int DropRegionPriority(UiDesignerDropRegionKind kind)
{
    switch(kind) {
    case UiDesignerDropRegionKind::GridCell: return 50;
    case UiDesignerDropRegionKind::TitleCardContent: return 70;
    case UiDesignerDropRegionKind::TabPageContent: return 65;
    case UiDesignerDropRegionKind::AccordionSectionContent: return 60;
    case UiDesignerDropRegionKind::PanelBody: return 40;
    case UiDesignerDropRegionKind::GroupPanelBody: return 42;
    case UiDesignerDropRegionKind::BoxBody: return 38;
    case UiDesignerDropRegionKind::BoxFrame: return 36;
    case UiDesignerDropRegionKind::BoxGap: return 35;
    case UiDesignerDropRegionKind::BoxBeforeItem:
    case UiDesignerDropRegionKind::BoxAfterItem: return 30;
    case UiDesignerDropRegionKind::BoxEmptyBody: return 20;
    case UiDesignerDropRegionKind::WindowContent:
    default: return 10;
    }
}

const UiDesignerDropRegion* UiDesignerGeometrySnapshot::HitDropRegion(Point p) const {
    const UiDesignerDropRegion* best = nullptr;
    for(const auto& region : drop_regions_) {
        Rect r = region.visual_rect.IsEmpty() ? region.rect : region.visual_rect;
        if(r.IsEmpty() || !r.Contains(p))
            continue;
        if(!best || region.depth > best->depth ||
           (region.depth == best->depth &&
            DropRegionPriority(region.kind) > DropRegionPriority(best->kind)) ||
           (region.depth == best->depth &&
            DropRegionPriority(region.kind) == DropRegionPriority(best->kind) &&
            region.paint_order > best->paint_order))
            best = &region;
    }
    return best;
}

UiDesignerNodeId UiDesignerGeometrySnapshot::HitDropTarget(Point p) const {
    const UiDesignerDropRegion* region = HitDropRegion(p);
    if(region)
        return region->owner;
    const UiDesignerGeometryRecord* best = nullptr;
    for(const auto& record : records_)
        if(record.drop_target && record.rect.Contains(p) &&
           BetterSelectionRecord(record, best))
            best = &record;
    return best ? best->node : 0;
}
}
