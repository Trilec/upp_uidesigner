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

    UiGridLayout *runtime_grid = dynamic_cast<UiGridLayout *>(preview.FindRuntime(grid));
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

    UiBoxLayout *runtime_box = dynamic_cast<UiBoxLayout *>(preview.FindRuntime(box));
    t.Check(runtime_box && runtime_box->GetDirection() == UiDirection::H &&
                runtime_box->GetWrapMode() == UiBoxWrap::Flow,
            "Box rebuild preserves authored direction and wrap state");

    const UiDesignerGeometrySnapshot& geometry = preview.GetGeometrySnapshot();
    const Rect box_rect = preview.GetNodeRect(box);
    const Rect group_rect = preview.GetNodeRect(group);
    t.Check(!box_rect.IsEmpty() && !group_rect.IsEmpty() && box_rect.Contains(group_rect),
            Format("nested Box and GroupPanel publish coherent geometry: %s / %s",
                   AsString(box_rect), AsString(group_rect)));
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

    UiDesignerSession demo;
    demo.NewDocument("blank");
    UiDesignerNodeId demo_root = 0;
    error.Clear();
    t.Check(demo.InsertPresetAt("Demo", 0, Point(), false,
                                -1, -1, -1, &demo_root, error),
            "inserts Demo preset through the location-aware activation path: " + error);
    const UiDesignerNode *demo_grid = demo.Document().Find(demo_root);
    t.Check(demo_grid && demo_grid->type == "UiGridLayout" &&
                demo_grid->children.GetCount() == 4,
            "Demo root is a four-panel Grid");
    bool cells[2][2] = {{false, false}, {false, false}};
    if(demo_grid)
        for(UiDesignerNodeId child_id : demo_grid->children) {
            const UiDesignerNode *panel = demo.Document().Find(child_id);
            const int row = panel ? (int)panel->GetProperty("grid_row", -1) : -1;
            const int column = panel ? (int)panel->GetProperty("grid_column", -1) : -1;
            if(panel && panel->type == "UiGroupPanel" &&
               row >= 0 && row < 2 && column >= 0 && column < 2)
                cells[row][column] = true;
        }
    t.Check(cells[0][0] && cells[0][1] && cells[1][0] && cells[1][1],
            "Demo authors one GroupPanel in every 2x2 Grid cell");

    UiDesignerPreviewCanvas demo_preview;
    demo_preview.SetRect(0, 0, 640, 420);
    demo.AttachProjection(&demo_preview);
    Vector<Rect> panel_rects;
    if(demo_grid)
        for(UiDesignerNodeId child_id : demo_grid->children)
            panel_rects.Add(demo_preview.GetNodeRect(child_id));
    bool distinct = panel_rects.GetCount() == 4;
    for(int i = 0; distinct && i < panel_rects.GetCount(); i++) {
        distinct &= !panel_rects[i].IsEmpty();
        for(int j = i + 1; distinct && j < panel_rects.GetCount(); j++)
            distinct &= panel_rects[i] != panel_rects[j] &&
                        !panel_rects[i].Intersects(panel_rects[j]);
    }
    t.Check(distinct,
            "four Demo GroupPanels resolve to distinct non-overlapping preview cells");

    UiDesignerSession positioned;
    positioned.NewDocument("blank");
    const UiDesignerNodeId absolute = positioned.AddControl("UiAbsoluteLayout");
    UiDesignerNodeId positioned_demo = 0;
    error.Clear();
    t.Check(absolute && positioned.InsertPresetAt(
                "Demo", absolute, Point(163, 117), true, -1, -1, -1,
                &positioned_demo, error),
            "location-aware preset inserts into Absolute Layout: " + error);
    const UiDesignerNode *positioned_node = positioned.Document().Find(positioned_demo);
    t.Check(positioned_node &&
                (int)positioned_node->GetProperty("x", -1) == 160 &&
                (int)positioned_node->GetProperty("y", -1) == 120,
            "location-aware preset preserves snapped canvas drop coordinates");

    Cout() << "PREVIEW_LAYOUT_REGRESSION_SUMMARY checks=" << t.checks
           << " failed=" << t.failures << '\n';
    SetExitCode(t.failures ? 1 : 0);
}
