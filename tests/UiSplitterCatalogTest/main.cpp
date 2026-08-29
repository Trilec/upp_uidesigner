#include <Core/Core.h>
#include <UiDesigner/Services/UiDesignerSession.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
    int checks = 0;
    int failed = 0;
    auto Check = [&](bool condition, const char *label) {
        checks++;
        if(!condition) {
            failed++;
            Cout() << "FAIL: " << label << '\n';
        }
    };

    auto Insert = [&](UiDesignerSession& session, const String& type,
                      UiDesignerNodeId target, const char *label) {
        String error;
        UiDesignerDropPlan plan = session.PlanAddControl(type, target);
        Check(plan.valid, label);
        UiDesignerNodeId created = 0;
        Check(plan.valid && session.ExecuteDrop(plan, &created, error) &&
              created != 0, label);
        if(!error.IsEmpty())
            Cout() << "ERROR: " << error << '\n';
        return created;
    };

    UiDesignerSession splitter_session;
    splitter_session.NewDocument("blank");
    const UiDesignerNodeId splitter = Insert(
        splitter_session, "UiSplitter",
        splitter_session.Document().GetRootId(),
        "UiSplitter insertion through normal catalog drop path");
    for(int i = 0; i < 3; i++)
        Insert(splitter_session, "UiPanel", splitter,
               "UiSplitter pane insertion through normal catalog drop path");

    String error;
    Check(splitter_session.Document().Find(splitter)->children.GetCount() == 3,
          "UiSplitter accepts three direct panes");
    error.Clear();
    Check(splitter_session.Catalog().ValidateDocument(
              splitter_session.Document(), error),
          "three-pane UiSplitter passes document validation");
    if(!error.IsEmpty())
        Cout() << "ERROR: " << error << '\n';

    Insert(splitter_session, "UiPanel", splitter,
           "UiSplitter accepts a fourth direct pane");
    error.Clear();
    Check(splitter_session.Catalog().ValidateDocument(
              splitter_session.Document(), error),
          "four-pane UiSplitter remains valid");

    UiDesignerSession quad_session;
    quad_session.NewDocument("blank");
    const UiDesignerNodeId quad = Insert(
        quad_session, "UiQuadSplitter",
        quad_session.Document().GetRootId(),
        "UiQuadSplitter insertion through normal catalog drop path");
    for(int i = 0; i < 4; i++)
        Insert(quad_session, "UiPanel", quad,
               "UiQuadSplitter pane insertion through normal catalog drop path");

    UiDesignerDropPlan fifth = quad_session.PlanAddControl("UiPanel", quad);
    Check(!fifth.valid && fifth.reason.Find("four panes") >= 0,
          "UiQuadSplitter fifth pane remains rejected");

    Cout() << "UI_SPLITTER_CATALOG_SUMMARY checks=" << checks
           << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
