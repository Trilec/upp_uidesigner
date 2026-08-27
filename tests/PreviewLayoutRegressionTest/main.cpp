#include <Core/Core.h>
#include <UiDesigner/Services/UiDesignerServices.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/Preview/UiDesignerSelectionHit.h>

using namespace Upp;

struct PreviewLayoutRegressionTester {
    int checks = 0;
    int failures = 0;

    void Check(bool condition, const String& label)
    {
        checks++;
        if(condition)
            Cout() << "PASS  " << label << '\n';
        else {
            failures++;
            Cout() << "FAIL  " << label << '\n';
        }
    }
};

static int GridCellCount(const UiDesignerPreviewCanvas& preview,
                         UiDesignerNodeId grid)
{
    const UiDesignerGeometryRecord *record = preview.FindGeometry(grid);
    return record ? record->cell_rects.GetCount() : -1;
}

CONSOLE_APP_MAIN
{
    PreviewLayoutRegressionTester t;

    UiDesignerSession session;
    session.NewDocument("blank");
    const UiDesignerNodeId grid = session.AddControl("UiGridLayout");
    t.Check(grid != 0, "creates Grid fixture");

    UiDesignerPreviewCanvas preview;
    preview.SetRect(0, 0, 512, 250);
    session.AttachProjection(&preview);
    session.Select(grid);

    String error;
    t.Check(session.CommitProperty("rows", 1, error),
            "authors Grid rows=1: " + error);
    error.Clear();
    t.Check(session.CommitProperty("columns", 3, error),
            "authors Grid columns=3: " + error);
    t.Check(GridCellCount(preview, grid) == 3,
            Format("live Grid is 1x3 before structural rebuild (%d cells)",
                   GridCellCount(preview, grid)));

    error.Clear();
    t.Check(session.CommitProperty("inset", 0, error),
            "authors Grid inset=0: " + error);
    error.Clear();
    t.Check(session.CommitProperty("gap", 0, error),
            "authors Grid gap=0: " + error);

    UiDesignerDropPlan box_plan = session.PlanAddControl(
        "UiBoxLayout", grid, Point(), false, -1, 0, 0);
    UiDesignerNodeId box = 0;
    error.Clear();
    t.Check(box_plan.valid && session.ExecuteDrop(box_plan, &box, error),
            "adds Box to Grid and forces structural preview rebuild: " + error);
    t.Check(box != 0, "creates nested Box fixture");
    t.Check(session.Document().GetProperty(grid, "rows", -1) == 1 &&
                session.Document().GetProperty(grid, "columns", -1) == 3,
            "Grid document remains authored as 1x3 after child insertion");
    t.Check(GridCellCount(preview, grid) == 3,
            Format("rebuilt Grid remains 1x3 after Box insertion (%d cells)",
                   GridCellCount(preview, grid)));

    UiGridLayout *runtime_grid =
        dynamic_cast<UiGridLayout *>(preview.FindRuntime(grid));
    Vector<Rect> runtime_cells;
    if(runtime_grid)
        runtime_grid->GetCellRects(runtime_cells);
    t.Check(runtime_grid && runtime_cells.GetCount() == 3,
            Format("runtime Grid retains three configured cells after rebuild (%d)",
                   runtime_cells.GetCount()));

    session.Select(box);
    error.Clear();
    t.Check(session.CommitProperty("direction", "H", error),
            "authors Box horizontal direction: " + error);
    error.Clear();
    t.Check(session.CommitProperty("wrap", "Flow", error),
            "authors Box Flow wrapping: " + error);
    error.Clear();
    t.Check(session.CommitProperty("inset", 0, error),
            "authors Box inset=0 for full-overlap selection fixture: " + error);
    error.Clear();
    t.Check(session.CommitProperty("gap", 0, error),
            "authors Box gap=0 for full-overlap selection fixture: " + error);

    UiDesignerDropPlan group_plan = session.PlanAddControl("UiGroupPanel", box);
    UiDesignerNodeId group = 0;
    error.Clear();
    t.Check(group_plan.valid && session.ExecuteDrop(group_plan, &group, error),
            "adds GroupPanel inside Box and rebuilds nested preview: " + error);
    t.Check(group != 0, "creates nested GroupPanel fixture");
    t.Check(GridCellCount(preview, grid) == 3,
            "second structural rebuild still preserves Grid 1x3 state");

    UiBoxLayout *runtime_box =
        dynamic_cast<UiBoxLayout *>(preview.FindRuntime(box));
    t.Check(runtime_box && runtime_box->GetDirection() == UiDirection::H &&
                runtime_box->GetWrapMode() == UiBoxWrap::Flow,
            "Box rebuild preserves authored direction and wrap state");

    const UiDesignerGeometrySnapshot& geometry = preview.GetGeometrySnapshot();
    const Rect box_rect = preview.GetNodeRect(box);
    const Rect group_rect = preview.GetNodeRect(group);
    t.Check(!box_rect.IsEmpty() && !group_rect.IsEmpty() &&
                box_rect.Contains(group_rect),
            Format("nested Box and GroupPanel publish coherent geometry: %s / %s",
                   AsString(box_rect), AsString(group_rect)));

    // Probe the Box rail on the GroupPanel's centerline. Flow layout keeps
    // this natural-height child shorter than the Box, so the Box center can
    // fall outside the child rather than testing an overlapping rail.
    const Point box_rail(box_rect.left + DPI(1), group_rect.CenterPoint().y);
    t.Check(group_rect.Contains(box_rail),
            "zero-inset GroupPanel overlaps the Box perimeter selection rail");
    t.Check(geometry.Hit(box_rail) == box,
            Format("Box perimeter remains directly selectable over its stretched child (%d)",
                   (int)geometry.Hit(box_rail)));

    const Point group_center = group_rect.CenterPoint();
    t.Check(geometry.Hit(group_center) == group,
            Format("GroupPanel remains the normal deepest selection away from the Box rail (%d)",
                   (int)geometry.Hit(group_center)));

    const Vector<UiDesignerNodeId> stack = UiDesignerPreviewSelectionStack(
        geometry, session.Document(), group_center);
    t.Check(stack.GetCount() >= 3,
            Format("overlapping preview point exposes nested selection stack (%d)",
                   stack.GetCount()));
    const int box_in_stack = FindIndex(stack, box);
    const int grid_in_stack = FindIndex(stack, grid);
    t.Check(!stack.IsEmpty() && stack[0] == group && box_in_stack > 0 &&
                grid_in_stack > box_in_stack,
            "selection stack orders GroupPanel -> Box -> Grid ancestors");

    UiDesignerDropPlan button_plan = session.PlanAddControl("UiButton", box);
    UiDesignerNodeId button = 0;
    error.Clear();
    t.Check(button_plan.valid && session.ExecuteDrop(button_plan, &button, error) && button != 0,
            "creates Button beside GroupPanel for preview reparent fixture: " + error);

    session.Select(button);
    UiDesignerDropPlan button_to_group = session.PlanMoveSelection(group);
    t.Check(button_to_group.valid,
            "Button -> GroupPanel move plan is valid: " + button_to_group.reason);
    error.Clear();
    t.Check(button_to_group.valid && session.ExecuteDrop(button_to_group, nullptr, error),
            "executes Button -> GroupPanel reparent: " + error);
    const UiDesignerNode *moved_button = session.Document().Find(button);
    t.Check(moved_button && moved_button->parent == group,
            "Button parent becomes GroupPanel after move");

    session.Select(group);
    UiDesignerDropPlan group_to_grid = session.PlanMoveSelection(
        grid, Point(), true, -1, 0, 1);
    t.Check(group_to_grid.valid,
            "GroupPanel -> free Grid cell move plan is valid: " + group_to_grid.reason);
    error.Clear();
    t.Check(group_to_grid.valid && session.ExecuteDrop(group_to_grid, nullptr, error),
            "executes GroupPanel -> Grid cell reparent: " + error);
    const UiDesignerNode *moved_group = session.Document().Find(group);
    t.Check(moved_group && moved_group->parent == grid &&
                (int)moved_group->GetProperty("grid_row", -1) == 0 &&
                (int)moved_group->GetProperty("grid_column", -1) == 1,
            "GroupPanel lands in authored Grid row 0 column 1");
    t.Check(GridCellCount(preview, grid) == 3,
            "preview move rebuild preserves Grid 1x3 state");

    Cout() << "PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=" << t.checks
           << " failed=" << t.failures << '\n';
    SetExitCode(t.failures ? 1 : 0);
}
