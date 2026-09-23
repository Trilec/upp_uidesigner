#include <CtrlLib/CtrlLib.h>
#include <UiDesigner/Theme/UiDesignerThemeBuilderV2.h>

using namespace Upp;

namespace {
int checks = 0;
int failed = 0;

void Check(bool ok, const char* text)
{
    ++checks;
    if(!ok) {
        ++failed;
        Cout() << "FAIL: " << text << '\n';
    }
}

template <class T>
void Collect(Ctrl& root, Vector<T*>& result)
{
    if(auto* match = dynamic_cast<T*>(&root)) result.Add(match);
    for(Ctrl* child = root.GetFirstChild(); child; child = child->GetNext())
        Collect(*child, result);
}

bool SameFace(const UiFill& a, const UiFill& b)
{
    return a.IsSolid() == b.IsSolid() && (!a.IsSolid() || a.color == b.color);
}

void Run()
{
    UiDesignerThemeDocument theme; // Outlives the gallery's model provider.
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    UiDesignerThemeGalleryV2 gallery;
    gallery.SetCatalog(&catalog);
    gallery.SetThemeDocument(&theme);
    UiDesignerThemeToolbarV2 toolbar(theme, gallery);
    gallery.SetRect(0, 0, DPI(1080), DPI(760));
    gallery.Layout();

    // Exercise the production dropdown callbacks, not just the public setters.
    Vector<UiDropdown*> roles;
    for(Ctrl* child = toolbar.GetFirstChild(); child; child = child->GetNext())
        if(auto* drop = dynamic_cast<UiDropdown*>(child)) roles.Add(drop);
    Check(roles.GetCount() == 2, "toolbar contains its two real role dropdowns");
    if(roles.GetCount() != 2) return;

    Vector<UiDesignerThemeSelectable<UiPanel>*> panels;
    Vector<UiDesignerThemeSelectable<UiGroupPanel>*> groups;
    Vector<UiDesignerThemeSelectable<UiButton>*> buttons;
    Vector<UiSliderEdit*> compounds;
    Vector<UiBreadcrumbs*> breadcrumbs;
    Vector<UiTable*> tables;
    Collect(gallery, panels);
    Collect(gallery, groups);
    Collect(gallery, buttons);
    Collect(gallery, compounds);
    Collect(gallery, breadcrumbs);
    Collect(gallery, tables);
    Check(panels.GetCount() >= 4 && groups.GetCount() >= 10 && buttons.GetCount() >= 4,
          "both visible and hidden gallery pages contain representative samples");
    Check(compounds.GetCount() == 1 && breadcrumbs.GetCount() == 1 && tables.GetCount() == 1,
          "compound, breadcrumb and table samples are present");
    if(panels.IsEmpty() || groups.IsEmpty() || buttons.IsEmpty() ||
       compounds.GetCount() != 1 || breadcrumbs.GetCount() != 1 || tables.GetCount() != 1)
        return;

    UiTable& table = *tables[0];
    Check(table.Model().GetColumnCount() == 3 && table.Model().GetRowCount() == 6,
          "table contains a populated three-column six-row sample");
    Check(table.Model().GetHeader(UITABLE_COLUMN_AXIS, 0).text == "Item" &&
          table.Model().GetHeader(UITABLE_COLUMN_AXIS, 1).text == "Status" &&
          table.Model().GetHeader(UITABLE_COLUMN_AXIS, 2).text == "Count",
          "table has meaningful column headers");
    Check(table.Model().GetCellValue(0, 0) == "Layout" &&
          table.Model().GetCellValue(5, 1) == "Queued" &&
          (int)table.Model().GetCellValue(2, 2) == 9,
          "table contains text, states and numeric data");
    table.Model().SetCellValue(0, 0, "User sample edit");
    table.SetActiveCell(2, 1);

    String error;
    for(const char* mode : {"Light", "Dark", "Light"}) {
        Check(theme.Commit("mode", mode, "Test mode", error), "mode selection commits");
        gallery.RefreshTheme();
        toolbar.ApplyTheme(theme.GetEffective());
        for(UiRole role : {UiRole::Standard, UiRole::Subtle, UiRole::Accent, UiRole::Alert}) {
            const UiRole control_before = gallery.GetUniversalControlRole();
            const UiFill button_before = buttons[0]->GetStyle().palette.face[ST_NORMAL];
            const String saved = theme.Serialize(false);
            roles[0]->SelectByData((int)role);
            Check(gallery.GetUniversalPanelRole() == role &&
                  toolbar.GetUniversalPanelRole() == role,
                  "Panel dropdown immediately reaches gallery and toolbar");
            Check(gallery.GetUniversalControlRole() == control_before &&
                  SameFace(button_before, buttons[0]->GetStyle().palette.face[ST_NORMAL]),
                  "Panel dropdown does not change control role or button appearance");
            for(auto* panel : panels)
                Check(SameFace(panel->GetStyle().palette.face[ST_NORMAL],
                               UiTheme::ResolvePanel(role).palette.face[ST_NORMAL]),
                      "every panel sample resolves the chosen Panel Role");
            for(auto* group : groups)
                Check(group->GetStyle().title_color == UiTheme::ResolveGroupPanel(role).title_color &&
                      SameFace(group->GetStyle().palette.face[ST_NORMAL],
                               UiTheme::ResolveGroupPanel(role).palette.face[ST_NORMAL]),
                      "every group sample resolves the chosen Panel Role");

            const Color group_before = groups[0]->GetStyle().title_color;
            roles[1]->SelectByData((int)role);
            Check(gallery.GetUniversalControlRole() == role &&
                  toolbar.GetUniversalControlRole() == role,
                  "Control dropdown immediately reaches gallery and toolbar");
            Check(gallery.GetUniversalPanelRole() == role && groups[0]->GetStyle().title_color == group_before,
                  "Control dropdown leaves panel role and group appearance intact");
            for(auto* button : buttons)
                Check(SameFace(button->GetStyle().palette.face[ST_NORMAL],
                               UiTheme::ResolveButton(role).palette.face[ST_NORMAL]),
                      "every button sample resolves the chosen Control Role");
            Check(SameFace(compounds[0]->Slider().GetStyle().track_palette.face[ST_NORMAL],
                           UiTheme::ResolveSlider(role).track_palette.face[ST_NORMAL]),
                  "compound slider participates in live Control Role preview");
            Check(breadcrumbs[0]->GetStyle().text_role == role &&
                  breadcrumbs[0]->GetStyle().current_role == role &&
                  breadcrumbs[0]->GetStyle().current_ink == UiTheme::ResolveLabel(role).palette.ink[ST_NORMAL],
                  "breadcrumb roles and actual ink update in the selected mode");
            Check(table.GetStyle().header_ink == UiTheme::ResolveLabel(role).palette.ink[ST_NORMAL] &&
                  table.GetStyle().show_column_headers,
                  "table header is visible and follows Control Role");
            Check(theme.Serialize(false) == saved, "role preview does not rewrite saved recipes or Theme state");
            Check(table.Model().GetCellValue(0, 0) == "User sample edit" &&
                  table.GetActiveCell().row == 2 && table.GetActiveCell().col == 1,
                  "role/mode updates preserve table contents and selection");
        }
        // Rebinding metadata must not silently reapply legacy panel roles.
        gallery.SetCatalog(&catalog);
        Check(SameFace(panels[0]->GetStyle().palette.face[ST_NORMAL],
                       UiTheme::ResolvePanel(UiRole::Alert).palette.face[ST_NORMAL]),
              "catalog rebind preserves the current universal Panel Role");
    }

    // Explicit recipes still win, but only within their selected role target.
    roles[1]->SelectByData((int)UiRole::Accent);
    theme.SetActiveStyleTarget("Light|control|UiButton|Accent");
    const Color authored(91, 71, 51);
    Check(theme.Commit("studio.face_normal", authored, "Test recipe", error), "role recipe commits");
    gallery.RefreshTheme();
    Check(buttons[0]->GetStyle().palette.face[ST_NORMAL].color == authored,
          "authored Accent recipe survives refresh");
    roles[1]->SelectByData((int)UiRole::Alert);
    Check(SameFace(buttons[0]->GetStyle().palette.face[ST_NORMAL],
                   UiTheme::ResolveButton(UiRole::Alert).palette.face[ST_NORMAL]),
          "Accent recipe does not leak into Alert preview");
    roles[1]->SelectByData((int)UiRole::Accent);
    Check(buttons[0]->GetStyle().palette.face[ST_NORMAL].color == authored,
          "returning to Accent restores its authored recipe");
}
}

GUI_APP_MAIN
{
    Run();
    Cout() << "THEME_STUDIO_ROLE checks=" << checks << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
