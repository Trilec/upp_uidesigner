#ifndef _CurrentUiIntegrationTest_ModelDataFixture_h_
#define _CurrentUiIntegrationTest_ModelDataFixture_h_

#include <UiDesigner/Services/UiDesignerSession.h>

namespace Upp {
inline bool AddAuthoredModelFixture(UiDesignerSession& session, UiDesignerNodeId parent,
                                    UiDesignerNodeId& list, UiDesignerNodeId& tree)
{
    list = session.AddControl("UiList", parent);
    tree = session.AddControl("UiTree", parent);
    if(!list || !tree) return false;
    ValueMap payload; payload.Set("color", Color(12, 34, 56));
    ValueMap row;
    row.Set("text", "Authored list row"); row.Set("data", payload);
    row.Set("description", "Detail"); row.Set("right_text", "Badge");
    row.Set("enabled", false); row.Set("has_check", true); row.Set("checked", true);
    ValueArray items; items.Add(row);
    ValueMap list_root; list_root.Set("items", items);
    ValueMap leaf; leaf.Set("text", "Nested authored leaf"); leaf.Set("data", 42);
    leaf.Set("editable", true); leaf.Set("enabled", false);
    ValueArray children; children.Add(leaf);
    ValueMap branch; branch.Set("text", "Authored branch"); branch.Set("children", children);
    children.Clear(); children.Add(branch);
    ValueMap tree_root; tree_root.Set("text", "Authored root");
    tree_root.Set("data", "root payload"); tree_root.Set("children", children);
    return session.Commands().SetData(list, "root", list_root) &&
           session.Commands().SetData(tree, "root", tree_root);
}
}
#endif
