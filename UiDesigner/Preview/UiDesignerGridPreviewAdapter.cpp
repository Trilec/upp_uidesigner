#include "UiDesignerPreview.h"

namespace Upp {
namespace {

// UiGridLayout owns coupled structural state (rows/columns and minimum cell
// dimensions). The generic property adapter can apply scalar layout fields one
// at a time, but a freshly reconstructed Grid must retain the other half of
// each pair while those properties are replayed. Keep that reconstruction
// state on the preview instance itself so document/subtree rebuilds reproduce
// the authored Grid rather than falling back to UiGridLayout's 2x2 defaults.
class UiDesignerGridPreviewCtrl : public UiGridLayout {
public:
    void InitializeDesignerState(const UiDesignerControlSpec& spec)
    {
        rows_ = max(1, (int)UiDesignerMapValue(spec.defaults, "rows", 2));
        columns_ = max(1, (int)UiDesignerMapValue(spec.defaults, "columns", 2));
        min_cell_width_ = max(0, (int)UiDesignerMapValue(
            spec.defaults, "min_cell_width", 10));
        min_cell_height_ = max(0, (int)UiDesignerMapValue(
            spec.defaults, "min_cell_height", 10));
        ApplyStructure();
        ApplyMinimumCell();
    }

    void SetDesignerRows(int rows)
    {
        rows_ = max(1, rows);
        ApplyStructure();
    }

    void SetDesignerColumns(int columns)
    {
        columns_ = max(1, columns);
        ApplyStructure();
    }

    void SetDesignerMinCellWidth(int width)
    {
        min_cell_width_ = max(0, width);
        ApplyMinimumCell();
    }

    void SetDesignerMinCellHeight(int height)
    {
        min_cell_height_ = max(0, height);
        ApplyMinimumCell();
    }

private:
    void ApplyStructure()
    {
        SetGridSize(columns_, rows_);
    }

    void ApplyMinimumCell()
    {
        SetMinCellSize(Size(DPI(min_cell_width_), DPI(min_cell_height_)));
    }

    int rows_ = 2;
    int columns_ = 2;
    int min_cell_width_ = 10;
    int min_cell_height_ = 10;
};

static UiDesignerApplyResult ApplyDesignerGridPreview(
    Ctrl& ctrl, const UiDesignerControlSpec& spec,
    const String& property, const Value& value)
{
    (void)spec;
    UiDesignerGridPreviewCtrl *grid =
        dynamic_cast<UiDesignerGridPreviewCtrl *>(&ctrl);
    if(!grid)
        return UiDesignerApplyResult::Rejected;

    if(property == "visible") {
        ctrl.Show((bool)value);
        return UiDesignerApplyResult::AppliedControlState;
    }
    if(property == "enabled") {
        ctrl.Enable((bool)value);
        return UiDesignerApplyResult::AppliedControlState;
    }
    if(property == "inset") {
        grid->SetInset(DPI(max(0, (int)value)));
        return UiDesignerApplyResult::AppliedAncestorLayout;
    }
    if(property == "gap") {
        grid->SetGap(DPI(max(0, (int)value)));
        return UiDesignerApplyResult::AppliedAncestorLayout;
    }
    if(property == "debug_layout") {
        grid->SetDebug((bool)value);
        return UiDesignerApplyResult::AppliedPaint;
    }
    if(property == "rows") {
        grid->SetDesignerRows((int)value);
        return UiDesignerApplyResult::AppliedAncestorLayout;
    }
    if(property == "columns") {
        grid->SetDesignerColumns((int)value);
        return UiDesignerApplyResult::AppliedAncestorLayout;
    }
    if(property == "min_cell_width") {
        grid->SetDesignerMinCellWidth((int)value);
        return UiDesignerApplyResult::AppliedAncestorLayout;
    }
    if(property == "min_cell_height") {
        grid->SetDesignerMinCellHeight((int)value);
        return UiDesignerApplyResult::AppliedAncestorLayout;
    }
    if(property == "name")
        return UiDesignerApplyResult::AppliedControlState;
    if(property == "role") {
        ctrl.Refresh();
        return UiDesignerApplyResult::AppliedPaint;
    }

    // Common sizing/alignment fields are owned by the parent layout descriptor.
    // Returning Rejected lets UiDesignerPreviewCanvas route live edits through
    // UpdateManagedLayoutItem(); BuildNode performs the same managed-item replay
    // once all authored properties have been reconstructed.
    return UiDesignerApplyResult::Rejected;
}

static void RegisterGridPreviewAdapter(UiDesignerPreviewAdapterRegistry& registry)
{
    UiDesignerPreviewAdapter adapter;
    adapter.id = "runtime:UiGridLayout";
    adapter.create = [] { return MakeOne<UiDesignerGridPreviewCtrl>(); };
    adapter.initialize = [](Ctrl& ctrl, const UiDesignerControlSpec& spec) {
        ctrl.Tip(spec.help.IsEmpty() ? spec.display_name : spec.help);
        if(UiDesignerGridPreviewCtrl *grid =
               dynamic_cast<UiDesignerGridPreviewCtrl *>(&ctrl))
            grid->InitializeDesignerState(spec);
    };
    adapter.apply = ApplyDesignerGridPreview;
    registry.Register(pick(adapter));
}

} // namespace

// The Preview package is linked through package archives, so a translation
// unit containing only a file-static registration object can be discarded by
// the linker. Defining the registry constructor here gives UiDesignerPreview.cpp
// a concrete symbol dependency on this unit and makes Grid adapter registration
// deterministic for every preview/rebuild path.
UiDesignerPreviewAdapterRegistry::UiDesignerPreviewAdapterRegistry()
{
    RegisterGridPreviewAdapter(*this);
}

} // namespace Upp
