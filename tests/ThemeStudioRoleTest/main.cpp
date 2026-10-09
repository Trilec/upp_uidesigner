#include <CtrlLib/CtrlLib.h>
#include <UiDesigner/Theme/UiDesignerThemeBuilderV2.h>
#include <UiDesigner/Theme/UiDesignerThemeAdapter.h>
#include <UiDesigner/Services/UiDesignerSession.h>

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

template <class T>
void CheckFrameAccent(const UiDesignerCatalog& catalog, const char *type,
                      String& generated, int index)
{
    const UiDesignerControlSpec *spec = catalog.Find(type);
    const auto *adapter = spec ? UiDesignerGetThemeAdapter(*spec) : nullptr;
    Check(adapter != nullptr, "Frame Accent has a registered adapter");
    if(!adapter) return;
    for(const char *field : {"frame_accent_top", "frame_accent_bottom",
                            "frame_accent_left", "frame_accent_right",
                            "frame_accent_thickness", "frame_accent_color", "frame_accent_alpha"}) {
        const auto *property = spec->FindThemeOverride(field);
        Check(property && adapter->HasField(property->adapter_field_id),
              "Frame Accent metadata is owned by the runtime adapter");
        Check(property && !adapter->FieldAffectsLayout(property->adapter_field_id),
              "Frame Accent changes paint without changing layout");
    }
    UiDesignerDocument document;
    document.NewDocument();
    const auto id = document.AddNode(type, type, document.GetRootId(), UiDesignerNodeNone);
    auto& node = *document.Find(id);
    node.properties = spec->defaults;
    node.SetThemeOverride("frame_accent_top", true);
    node.SetThemeOverride("frame_accent_bottom", true);
    node.SetThemeOverride("frame_accent_left", false);
    node.SetThemeOverride("frame_accent_right", true);
    node.SetThemeOverride("frame_accent_thickness", 4);
    node.SetThemeOverride("frame_accent_color", Color(43, 101, 167));
    node.SetThemeOverride("frame_accent_alpha", 170);
    T control;
    adapter->ApplyPreviewStyle(control, node, *spec, nullptr);
    const auto actual = control.GetStyle().metrics.frame_accent;
    const int edges = StyledFrameAccent::Top | StyledFrameAccent::Bottom | StyledFrameAccent::Right;
    Check(actual.edges == edges && actual.thickness == 4 && actual.alpha == 170 &&
          actual.color == Color(43, 101, 167), "Frame Accent reaches the preview");
    UiDesignerTransientOverlay overlay;
    overlay.Set(id, UiDesignerTransientValueKind::ThemeOverride, "frame_accent_top", false);
    adapter->ApplyPreviewStyle(control, node, *spec, &overlay);
    Check(control.GetStyle().metrics.frame_accent.edges == (edges & ~StyledFrameAccent::Top),
          "Frame Accent transient preview preserves the other edges");
    adapter->ApplyPreviewStyle(control, node, *spec, nullptr);
    Check(control.GetStyle().metrics.frame_accent.edges == edges,
          "Frame Accent cancel restores authored edges");
    String error;
    UiDesignerDocument loaded;
    Check(UiDesignerDeserialize(UiDesignerSerialize(document, false), loaded, error),
          "Frame Accent document loads from canonical JSON");
    const auto *restored = loaded.Find(id);
    Check(restored && (bool)restored->GetThemeOverride("frame_accent_top") &&
          (Color)restored->GetThemeOverride("frame_accent_color") == Color(43, 101, 167),
          "Frame Accent side and colour survive JSON roundtrip");
    const String member = "accent_" + AsString(index);
    generated << "{\n" << type << " " << member << ";\n";
    adapter->EmitSetup(generated, member, node, *spec);
    generated << "if(" << member << ".GetStyle().metrics.frame_accent.edges != " << edges
              << " || " << member << ".GetStyle().metrics.frame_accent.thickness != 4"
              << " || " << member << ".GetStyle().metrics.frame_accent.alpha != 170"
              << " || " << member << ".GetStyle().metrics.frame_accent.color != Color(43, 101, 167)) ++failed;\n}\n";
    node.ClearThemeOverrides();
    adapter->ApplyPreviewStyle(control, node, *spec, nullptr);
    Check(control.GetStyle().metrics.frame_accent.edges == StyledFrameAccent::None,
          "Reset removes Frame Accent and resumes theme inheritance");
}

void CheckFrameAccents()
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    String generated = "#include <Ui/Ui.h>\nusing namespace Upp;\nGUI_APP_MAIN { int failed = 0;\n";
    CheckFrameAccent<UiPanel>(catalog, "UiPanel", generated, 0);
    CheckFrameAccent<UiGroupPanel>(catalog, "UiGroupPanel", generated, 1);
    CheckFrameAccent<UiScrollPanel>(catalog, "UiScrollPanel", generated, 2);
    CheckFrameAccent<UiButton>(catalog, "UiButton", generated, 3);
    CheckFrameAccent<UiLabel>(catalog, "UiLabel", generated, 4);
    CheckFrameAccent<UiDropdown>(catalog, "UiDropdown", generated, 5);
    CheckFrameAccent<UiList>(catalog, "UiList", generated, 6);
    CheckFrameAccent<UiAccordion>(catalog, "UiAccordion", generated, 7);
    CheckFrameAccent<UiLineEdit>(catalog, "UiLineEdit", generated, 8);
    CheckFrameAccent<UiTitleCard>(catalog, "UiTitleCard", generated, 9);
    CheckFrameAccent<UiMenu>(catalog, "UiMenu", generated, 10);
    CheckFrameAccent<UiColorPicker>(catalog, "UiColorPicker", generated, 11);
    generated << "Cout() << \"FRAME_ACCENT_GENERATED checks=12 failed=\" << failed << char(10); SetExitCode(failed ? 1 : 0); }\n";
    Check(SaveFile(AppendFileName(GetExeFolder(), "frame-accent-generated.cpp"), generated),
          "Generated Frame Accent fixture saves for a native compile check");
}

void CheckAccentShapeAvailability()
{
    UiDesignerSession session;
    for(const char *type : {"UiCheckBox", "UiRadioButton"}) {
        const auto *spec = session.Catalog().Find(type);
        Check(spec && !spec->FindProperty("visual"), "Designer indicator control currently supports Classic only");
        Check(spec && !spec->FindThemeOverride("frame_accent_top"), "Classic-only control does not advertise an ignored outer accent");
        Check(spec && spec->FindThemeOverride("indicator_frame_accent_top"), "Classic indicator still exposes its working accent");
    }
    const auto *spec = session.Catalog().Find("UiTab");
    const auto *property = spec ? spec->FindThemeOverride("tab_frame_accent_top") : nullptr;
    Check(property && property->visible_when_id == "visual" &&
          property->visible_when_value == "Segmented" && !property->help.IsEmpty(),
          "Tab cap metadata identifies its supported visual");
    if(!spec || !property) return;
    const auto id = session.Document().AddNode("UiTab", "accent_shapes", session.Document().GetRootId(), UiDesignerNodeNone);
    auto& node = *session.Document().Find(id);
    node.properties = spec->defaults;
    node.SetThemeOverride("tab_frame_accent_top", true);
    session.Select(id);
    const auto visible = [&] { const auto *row = session.ThemeOverrideModel().Find("tab_frame_accent_top"); return row && row->visible; };
    Check(!visible(), "Classic Tab Inspector hides the unsupported cap accent");
    Check(session.Commands().SetProperty(id, "visual", "Segmented", UiDesignerImpactPaint), "Actual normal-property command selects Segmented");
    Check(visible(), "Segmented Tab Inspector reveals the supported cap accent");
    Check(session.Commands().SetProperty(id, "visual", "Classic", UiDesignerImpactPaint) && !visible(), "Changing visual updates cap availability immediately");
    Check((bool)node.GetThemeOverride("tab_frame_accent_top"), "Hiding unsupported cap decoration retains authored values");
    Check(session.Commands().Undo() && visible(), "Undo restores Segmented cap availability and its saved accent");
}

void CheckRoleInheritance()
{
    UiDesignerCatalog catalog;
    RegisterUiDesignerBuiltins(catalog);
    const UiThemeContext previous = UiTheme::GetContext();
    for(UiThemePreset preset : {UiThemePreset::Minimal, UiThemePreset::Pill,
        UiThemePreset::Linear, UiThemePreset::Solid, UiThemePreset::Outline,
        UiThemePreset::Compact, UiThemePreset::Layered})
    for(UiThemeMode mode : {UiThemeMode::Light, UiThemeMode::Dark}) {
        UiThemeContext ctx;ctx.preset=preset;ctx.mode=mode;UiTheme::Set(ctx);
        for(const char* role_name : {"Standard", "Subtle", "Accent", "Alert"}) {
            UiRole role = String(role_name)=="Alert" ? UiRole::Alert : String(role_name)=="Accent" ? UiRole::Accent : String(role_name)=="Subtle" ? UiRole::Subtle : UiRole::Standard;
            UiList list;UiTree tree;UiAccordion accordion;
            for(const char* type : {"UiList", "UiTree", "UiAccordion"}) {
                const auto* spec=catalog.Find(type);const auto* adapter=spec?UiDesignerGetThemeAdapter(*spec):nullptr;
                Check(adapter!=nullptr,"role regression has registered adapter");if(!adapter)continue;
                UiDesignerNode node;node.type=type;node.properties=spec->defaults;node.SetProperty("role",role_name);
                Ctrl& control=String(type)=="UiList"?static_cast<Ctrl&>(list):String(type)=="UiTree"?static_cast<Ctrl&>(tree):static_cast<Ctrl&>(accordion);
                const String field=String(type)=="UiAccordion"?"header_subtitle_color":"selected_face";
                auto actual=[&]()->Color {return String(type)=="UiList"?list.GetStyle().selected_face:String(type)=="UiTree"?tree.GetStyle().selected_face:accordion.GetStyle().header_style.subtitle_color;};
                adapter->ApplyPreviewStyle(control,node,*spec,nullptr);
                Color inherited=actual();
                Color expected=String(type)=="UiAccordion"?UiTheme::ResolveTitleCard(role).subtitle_color:UiTheme::ResolveList(role).selected_face;
                Check(inherited==expected && adapter->ResolveFieldValue(node,*spec,field,nullptr)==inherited,"role reaches preview and Inspector across presets/modes");
                String output;adapter->EmitSetup(output,"sample",node,*spec);
                Check(role==UiRole::Standard || output.Find(String("UiRole::")+role_name)>=0,"role-only export retains requested role without authored override");
                const Color custom(37,91,143);node.theme_overrides.Set(field,custom);
                adapter->ApplyPreviewStyle(control,node,*spec,nullptr);
                Check(actual()==custom && adapter->ResolveFieldValue(node,*spec,field,nullptr)==custom,"explicit field override wins over role defaults");
                output.Clear();adapter->EmitSetup(output,"sample",node,*spec);
                Check(output.Find("37, 91, 143")>=0,"export retains explicit field override");
                node.SetProperty("role", "Alert");
                adapter->ApplyPreviewStyle(control,node,*spec,nullptr);
                Check(actual()==custom,"changing role preserves explicit field override");
                node.theme_overrides.Clear();adapter->ApplyPreviewStyle(control,node,*spec,nullptr);
                Color alert_default=String(type)=="UiAccordion"?UiTheme::ResolveTitleCard(UiRole::Alert).subtitle_color:UiTheme::ResolveList(UiRole::Alert).selected_face;
                Check(actual()==alert_default,"reset after role change restores the new role default");
                node.SetProperty("role", role_name);
                node.theme_overrides.Clear();adapter->ApplyPreviewStyle(control,node,*spec,nullptr);
                Check(actual()==inherited,"reset returns to current role default");
            }
            if(role==UiRole::Alert) {
                const auto* toggle_spec = catalog.Find("UiToggle");
                const auto* toggle_adapter = UiDesignerGetThemeAdapter(*toggle_spec);
                UiDesignerNode toggle_node;
                toggle_node.type = "UiToggle";
                toggle_node.properties = toggle_spec->defaults;
                toggle_node.SetProperty("role", "Alert");
                UiToggle toggle;
                toggle_adapter->ApplyPreviewStyle(toggle, toggle_node, *toggle_spec, nullptr);
                Color on = toggle.GetStyle().track_palette.face[ST_PRESSED].color;
                Check(on.GetR()>on.GetB(), "Alert Toggle uses role for its on-state track");
                String output;
                toggle_adapter->EmitSetup(output, "toggle", toggle_node, *toggle_spec);
                Check(output.Find("ResolveToggle(UiRole::Alert)")>=0, "Toggle export retains Alert role");
                Color track=UiTheme::ResolveProgressBar(role).track_palette.face[ST_NORMAL].color;
                Check(track.GetR()>track.GetB(),"Alert progress track follows red role family rather than blue slate");
                for(UiTabVisual visual : {UITAB_CLASSIC,UITAB_UNDERLINE,UITAB_SEGMENTED,UITAB_RAIL,UITAB_DOCUMENT}) {
                    UiTab::Style tab=UiTheme::ResolveTab(role,visual);
                    Color ink=tab.tab_palette.ink[ST_PRESSED];
                    Check(ink.GetR()>ink.GetB() && tab.visual==visual,"Alert tab active ink survives visual-family restoration");
                }
            }
        }
    }
    UiTheme::Set(previous);
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
    Check(panels.GetCount() == 3 && groups.GetCount() >= 10 && buttons.GetCount() == 3,
          "both visible and hidden gallery pages contain representative samples");
    Check(compounds.GetCount() == 1 && breadcrumbs.GetCount() == 1 && tables.GetCount() == 1,
          "compound, breadcrumb and table samples are present");
    if(panels.IsEmpty() || groups.IsEmpty() || buttons.IsEmpty() ||
       compounds.GetCount() != 1 || breadcrumbs.GetCount() != 1 || tables.GetCount() != 1)
        return;

    UiTable& table = *tables[0];
    // Rebinding the V2 gallery during shell refresh must never pass through
    // the legacy panel-role target or publish a fake theme preview change.
    int selection_events = 0, preview_events = 0;
    theme.WhenTargetChanged << [&] { ++selection_events; };
    theme.WhenPreview << [&] { ++preview_events; };
    const String before_selection = theme.Serialize(false);
    gallery.SetPanelRole(UiRole::Alert);
    for(int repeat = 0; repeat < 40; ++repeat) {
        groups[0]->WhenThemeSelect();
        const String target = theme.GetActiveStyleTarget();
        const int events = selection_events;
        gallery.SetThemeDocument(&theme);
        Check(theme.GetActiveStyleTarget() == target && selection_events == events,
              "gallery rebinding retains one canonical panel target without oscillation");
        buttons[0]->WhenThemeSelect();
    }
    Check(preview_events == 0 && selection_events >= 80,
          "sample selection notifies Inspector without broadcasting theme changes");
    Check(theme.Serialize(false) == before_selection && !theme.IsDirty(),
          "repeated sample inspection leaves authored theme and history unchanged");
    gallery.SetPanelRole(UiRole::Standard);
    theme.WhenTargetChanged.Clear();
    theme.WhenPreview.Clear();
    Check(table.Model().GetColumnCount() == 3 && table.Model().GetRowCount() == 4,
          "table contains a populated three-column four-row sample");
    Check(table.Model().GetHeader(UITABLE_COLUMN_AXIS, 0).text == "Item" &&
          table.Model().GetHeader(UITABLE_COLUMN_AXIS, 1).text == "Status" &&
          table.Model().GetHeader(UITABLE_COLUMN_AXIS, 2).text == "Count",
          "table has meaningful column headers");
    Check(table.Model().GetCellValue(0, 0) == "Layout" &&
          table.Model().GetCellValue(3, 1) == "Ready" &&
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
    for(const char* preset : {"Minimal", "Pill", "Linear", "Solid", "Outline", "Compact", "Layered"}) {
        Check(theme.Commit("preset", preset, "Select preset", error), "preset commits");
        gallery.RefreshTheme();
        Check(buttons[0]->GetStyle().palette.face[ST_NORMAL].color == authored,
              "preset switch retains authored colour");
        Check(buttons[0]->GetStyle().metrics.radius == UiTheme::ResolveButton(UiRole::Accent).metrics.radius,
              "unauthored button radius follows selected preset");
    }
    Vector<UiProgressRing*> progress_rings;
    Vector<UiChartRing*> chart_rings;
    Collect(gallery, progress_rings);
    Collect(gallery, chart_rings);
    Check(progress_rings.GetCount() == 1 && chart_rings.GetCount() == 1,
          "gallery includes both ring samples");
    const String saved_theme = theme.Serialize(false);
    Check(theme.ResetCustomizations(error), "theme reset commits");
    Check(theme.Get().preset == "Layered" && theme.Get().style_overrides.IsEmpty(),
          "reset keeps preset and clears authored recipes");
    Check(theme.Undo() && theme.Serialize(false) == saved_theme, "reset is fully undoable");
    UiDesignerThemeDocument imported;
    const String before_import = imported.Serialize(false);
    Check(imported.ImportTheme(saved_theme, error) && imported.Serialize(false) == saved_theme,
          "theme-only import preserves preset and recipes");
    Check(imported.IsDirty() && imported.Undo() && imported.Serialize(false) == before_import,
          "theme import is dirty and undoable");
    Check(!imported.ImportTheme("invalid JSON", error) && imported.Serialize(false) == before_import,
          "invalid theme import leaves current theme intact");
}
}

GUI_APP_MAIN
{
    CheckAccentShapeAvailability();
    CheckFrameAccents();
    CheckRoleInheritance();
    Run();
    Cout() << "THEME_STUDIO_ROLE checks=" << checks << " failed=" << failed << '\n';
    SetExitCode(failed ? 1 : 0);
}
