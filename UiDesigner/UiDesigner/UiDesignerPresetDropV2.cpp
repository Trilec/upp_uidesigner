#include "UiDesignerInteractionOverlayV2.h"
#include "UiDesignerWindow.h"

namespace Upp {

bool UiDesignerInteractionOverlayV2::FinishCatalogDrag(
    const String& type_id, Point screen)
{
    if(!type_id.StartsWith("preset:"))
        return UiDesignerInteractionOverlay::FinishCatalogDrag(type_id, screen);
    if(!owner_v2_) {
        UiDesignerInteractionOverlay::CancelCatalogDrag();
        return false;
    }

    const String preset_id = type_id.Mid(7);
    UiDesignerDocument fragment;
    UiDesignerNodeId fragment_root = 0;
    String error;
    if(!UiDesignerPresetLibrary::Build(preset_id, owner_v2_->session_.Catalog(),
                                       fragment, fragment_root, error)) {
        UiDesignerInteractionOverlay::CancelCatalogDrag();
        owner_v2_->RefreshStatus(error);
        return false;
    }
    const UiDesignerNode *preset_root = fragment.Find(fragment_root);
    if(!preset_root) {
        UiDesignerInteractionOverlay::CancelCatalogDrag();
        owner_v2_->RefreshStatus("Preset construction failed");
        return false;
    }

    const Point canvas = screen - owner_v2_->preview_canvas_.GetScreenRect().TopLeft();
    const UiDesignerGeometrySnapshot& geometry =
        owner_v2_->preview_canvas_.GetGeometrySnapshot();
    const UiDesignerDropRegion *region = geometry.HitDropRegion(canvas);
    if(!region) {
        UiDesignerInteractionOverlay::CancelCatalogDrag();
        owner_v2_->RefreshStatus("Preset drop has no target region");
        return false;
    }

    Point position = canvas;
    if(region->owner != owner_v2_->session_.Document().GetRootId()) {
        if(const UiDesignerGeometryRecord *target = geometry.Find(region->owner))
            position -= target->rect.TopLeft();
    }

    UiDesignerDropPlan plan = owner_v2_->session_.PlanAddControl(
        preset_root->type, region->owner, position, true,
        region->insertion_index, region->grid_row, region->grid_column);
    if(!plan.valid) {
        UiDesignerInteractionOverlay::CancelCatalogDrag();
        owner_v2_->RefreshStatus(plan.reason.IsEmpty()
            ? "Preset drop is invalid" : plan.reason);
        return false;
    }

    UiDesignerNodeId created = 0;
    const bool ok = owner_v2_->session_.InsertPresetAt(
        preset_id, region->owner, position, true,
        region->insertion_index, region->grid_row, region->grid_column,
        &created, error);
    UiDesignerInteractionOverlay::CancelCatalogDrag();
    owner_v2_->RefreshStatus(ok ? region->label + " : preset inserted"
                                : (error.IsEmpty() ? "Preset insertion failed" : error));
    return ok;
}

}
