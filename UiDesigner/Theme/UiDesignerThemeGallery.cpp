#include "UiDesignerThemeGallery.h"
#include <Ui/UiIcons.h>

namespace Upp {

UiThemeContext UiDesignerResolveThemeContext(
    const UiDesignerThemeSnapshot& theme)
{
    UiThemeContext context;
    if(theme.preset == "Pill") context.preset = UiThemePreset::Pill;
    else if(theme.preset == "Linear") context.preset = UiThemePreset::Linear;
    else if(theme.preset == "Solid") context.preset = UiThemePreset::Solid;
    else if(theme.preset == "Outline") context.preset = UiThemePreset::Outline;
    else if(theme.preset == "Compact") context.preset = UiThemePreset::Compact;
    else if(theme.preset == "Layered") context.preset = UiThemePreset::Layered;
    else context.preset = UiThemePreset::Minimal;

    if(theme.mode == "Dark") context.mode = UiThemeMode::Dark;
    else if(theme.mode == "System") context.mode = UiThemeMode::System;
    else context.mode = UiThemeMode::Light;
    return context;
}

void UiDesignerApplyGlobalTheme(const UiDesignerThemeSnapshot& theme)
{
    UiTheme::Set(UiDesignerResolveThemeContext(theme));
}

static Color ThemeBuilderReadableInk(Color face)
{
    const int luminance = face.GetR() * 299 + face.GetG() * 587 +
                          face.GetB() * 114;
    return luminance >= 150000 ? Color(15, 23, 42) : White();
}

static UiButton::Style ThemeBuilderRoleButtonStyle(UiRole role, Color face)
{
    UiButton::Style style = UiTheme::ResolveButton(role);
    const Color ink = ThemeBuilderReadableInk(face);
    style.metrics.face_enabled = true;
    style.metrics.frame_enabled = true;
    for(int state = 0; state < 4; state++) {
        style.palette.face[state] = UiFill::Solid(face);
        style.palette.frame[state] = face;
        style.palette.ink[state] = ink;
        style.palette.icon[state] = ink;
    }
    return style;
}

static UiPanel::Style ThemeBuilderPanelStyle(UiPanelRole role, Color face)
{
    UiPanel::Style style = UiTheme::ResolvePanel(role);
    style.metrics.face_enabled = true;
    style.metrics.frame_enabled = true;
    for(int state = 0; state < 4; state++) {
        style.palette.face[state] = UiFill::Solid(face);
        style.palette.frame[state] = face;
    }
    return style;
}

static UiLabel::Style ThemeBuilderPanelLabelStyle(Color face)
{
    UiLabel::Style style = UiTheme::ResolveLabel(UiRole::Standard);
    const Color ink = ThemeBuilderReadableInk(face);
    for(int state = 0; state < 4; state++) {
        style.palette.ink[state] = ink;
        style.palette.icon[state] = ink;
    }
    return style;
}

UiDesignerThemeGallery::UiDesignerThemeGallery()
{
    BackPaint();
    BuildAuthoredControls();
}

void UiDesignerThemeGallery::Put(Ctrl& ctrl, int x, int y, int cx, int cy)
{
    ctrl.SetRect(x, y, max(0, cx), max(0, cy));
}

void UiDesignerThemeGallery::BuildAuthoredControls()
{
    title_card_.SetTitle("Theme Builder")
               .SetSubTitle("Engineer a Light/Dark palette, assign semantic roles, then inspect representative controls below.")
               .SetContentInset(DPI(7))
               .SetMediaGap(DPI(10))
               .SetMediaReserve(DPI(24))
               .SetMediaMin(DPI(16))
               .ShowTitleLine(true)
               .ShowCardLine(false);
    title_card_.SetMedia(ICON_BRAND_NEWLOGO_V5_48(),
                         Size(DPI(29), DPI(29)));

    palette_group_.SetTitle("THEME PALETTE")
                  .SetSubTitle("Six authored colours per appearance mode")
                  .SetLine(false).SetHeaderBand(false);
    palette_mode_.UseInternalModel().Clear()
                 .Add("Light", "Light")
                 .Add("Dark", "Dark");
    palette_mode_.Select(0);
    palette_help_.SetText(
        "Use Generator, Palettes or the User Stash, then drag colours into the six primary theme slots. "
        "The Light and Dark palettes are stored independently in the theme document.")
        .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    palette_editor_.SetSlotCount(UI_DESIGNER_THEME_PALETTE_SIZE)
                   .SetGeneratorCount(UI_DESIGNER_THEME_PALETTE_SIZE)
                   .SetPageMode(UiColorPicker::PAGE_GENERATOR)
                   .SetMediumMode(UiColorPicker::MEDIUM_UI);
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        palette_editor_.SetSlotLabel(i, AsString(i + 1));
    palette_mode_.WhenAction = [=] {
        edit_dark_palette_ = AsString(palette_mode_.GetData()) == "Dark";
        SyncPaletteEditor();
    };
    palette_editor_.WhenSlotChanged = [=](int slot) {
        CommitPaletteSlot(slot);
    };

    control_roles_group_.SetTitle("CONTROL ROLES")
                        .SetSubTitle("Semantic control roles point to palette slots")
                        .SetLine(false).SetHeaderBand(false);
    role_standard_.SetText("Standard").Tip("Standard control role");
    role_subtle_.SetText("Subtle").Tip("Subtle control role");
    role_accent_.SetText("Accent").Tip("Accent control role");
    role_alert_.SetText("Alert").Tip("Alert control role");

    panel_roles_group_.SetTitle("PANEL ROLES")
                      .SetSubTitle("Surface roles are independent from control roles")
                      .SetLine(false).SetHeaderBand(false);
    panel_surface_label_.SetAlign(UiAlign::CENTER, UiAlign::CENTER);
    panel_subtle_label_.SetAlign(UiAlign::CENTER, UiAlign::CENTER);
    panel_strong_label_.SetAlign(UiAlign::CENTER, UiAlign::CENTER);
    panel_surface_.Add(panel_surface_label_);
    panel_subtle_.Add(panel_subtle_label_);
    panel_strong_.Add(panel_strong_label_);

    toggle_group_.SetTitle("TOGGLE THEMES").SetLine(false).SetHeaderBand(false);
    toggle_label_a_.SetText("First toggle theme")
                   .SetIcon(ICON_DESIGN_CIRCLE_CIRCLE_48(),
                            UiIconRenderMode::MonoTint)
                   .SetIconSize(DPI(12), DPI(12));
    toggle_label_b_.SetText("Second toggle theme")
                   .SetIcon(ICON_DESIGN_CIRCLE_CIRCLE_48(),
                            UiIconRenderMode::MonoTint)
                   .SetIconSize(DPI(12), DPI(12));
    toggle_a_.SetOn(true);
    toggle_b_.SetOn(false);

    buttons_group_.SetTitle("BUTTON THEMES").SetSubTitle("Representative button family")
                  .SetLine(false).SetHeaderBand(false);
    button_.SetText("Button");
    tool_button_.SetIcon(ICON_DESIGN_SETTINGS_48()).SetIconSize(DPI(20), DPI(20));
    split_button_.SetText("Save").SetSplitWidth(DPI(30))
                 .Add("Recent A", "a").Add("Recent B", "b").Add("Recent C", "c");
    breadcrumbs_.AddCrumb("Home", "0");
    breadcrumbs_.AddCrumb("Library", "1");
    breadcrumbs_.AddCrumb("Current", "2");
    breadcrumbs_.SetCurrentIndex(2);
    breadcrumbs_.SetDivider("/");
    breadcrumbs_.SetPathIcon(ICON_DESIGN_HOME_48(), UiAlign::LEFT,
                             Size(DPI(16), DPI(16)));

    numbers_group_.SetTitle("NUMERIC AND SLIDERS").SetLine(false).SetHeaderBand(false);
    int_edit_.MinMax(0, 100).Step(1).ShowSpin(true);
    int_edit_.SetValue(42);
    float_edit_.MinMax(0, 100).Step(0.1).Precision(2).ShowSpin(true);
    float_edit_.SetValue(3.14);
    slider_.SetRange(0, 100).SetValue(50);
    progress_.Percent(true).Set(60, 100);

    security_group_.SetTitle("MASK AND PASSWORD").SetLine(false).SetHeaderBand(false);
    mask_edit_.SetMask("##/##/####", '_');
    mask_edit_.SetPlaceholder("Masked value");
    password_edit_.SetTextUtf8("Password");
    password_edit_.SetPlaceholder("Password");
    password_edit_.SetPasswordChar(8226);
    password_edit_.EnableVisibilityIcon(true);

    document_group_.SetTitle("DOCUMENT").SetLine(false).SetHeaderBand(false);
    document_.SetText("UiDoc sample");

    data_group_.SetTitle("TAB, TREE, TABLE, LIST AND ACCORDION")
               .SetLine(false).SetHeaderBand(false);
    tab_.SetVisual(UITAB_UNDERLINE).SetPlacement(UiAlign::TOP)
        .EnableCloseButtons(false).EnableDragHandles(false);
    tab_.Add(tab_page_a_, "First");
    tab_.Add(tab_page_b_, "Second");
    tab_.SetActiveTab(0);
    tree_.Model().AddChild(
        tree_.Model().Root(), UiModelItem("Workspace", "workspace"));
    tree_.ShowConnectorLines(true);
    table_.UseInternalModel();
    table_.Model().SetSize(4, 2);
    list_.Model().Add(UiModelItem("First", 1));
    list_.Model().Add(UiModelItem("Second", 2));
    accordion_.SetSingleOpen(false).SetEnforceOne(false)
              .ShowChevron(true).SetAnimation(true, 120, 0);
    const int accordion_first = accordion_.AddSection("First", true);
    const int accordion_second = accordion_.AddSection("Second", false);
    accordion_.GetSectionContent(accordion_first).Add(accordion_a_.SizePos());
    accordion_.GetSectionContent(accordion_second).Add(accordion_b_.SizePos());

    input_group_.SetTitle("INPUTS AND ADVANCED EDITORS")
                .SetLine(false).SetHeaderBand(false);
    line_edit_.SetData("Line edit");
    multi_edit_.SetData("Multiline\ncontent");
    dropdown_.UseInternalModel().Clear()
             .Add("First", 1).Add("Second", 2).Add("Third", 3);
    dropdown_.Select(0);
    check_.SetText("Check box").SetData(true);
    radio_.SetText("Radio button").SetData(false);
    slider_edit_.SetRange(0, 100).SetValue(50);

    Add(title_card_);

    Add(palette_group_);
    palette_group_.Add(palette_mode_);
    palette_group_.Add(palette_help_);
    palette_group_.Add(palette_editor_);

    Add(control_roles_group_);
    control_roles_group_.Add(role_standard_);
    control_roles_group_.Add(role_subtle_);
    control_roles_group_.Add(role_accent_);
    control_roles_group_.Add(role_alert_);

    Add(panel_roles_group_);
    panel_roles_group_.Add(panel_surface_);
    panel_roles_group_.Add(panel_subtle_);
    panel_roles_group_.Add(panel_strong_);

    Add(toggle_group_);
    toggle_group_.Add(toggle_label_a_);
    toggle_group_.Add(toggle_label_b_);
    toggle_group_.Add(toggle_a_);
    toggle_group_.Add(toggle_b_);

    Add(buttons_group_);
    buttons_group_.Add(button_);
    buttons_group_.Add(tool_button_);
    buttons_group_.Add(split_button_);
    buttons_group_.Add(breadcrumbs_);

    Add(numbers_group_);
    numbers_group_.Add(int_edit_);
    numbers_group_.Add(float_edit_);
    numbers_group_.Add(slider_);
    numbers_group_.Add(progress_);

    Add(security_group_);
    security_group_.Add(mask_edit_);
    security_group_.Add(password_edit_);

    Add(document_group_);
    document_group_.Add(document_);

    Add(data_group_);
    data_group_.Add(tab_);
    data_group_.Add(tree_);
    data_group_.Add(table_);
    data_group_.Add(list_);
    data_group_.Add(accordion_);

    Add(input_group_);
    input_group_.Add(line_edit_);
    input_group_.Add(multi_edit_);
    input_group_.Add(dropdown_);
    input_group_.Add(check_);
    input_group_.Add(radio_);
    input_group_.Add(color_);
    input_group_.Add(slider_edit_);
    input_group_.Add(curve_);
}

void UiDesignerThemeGallery::SetCatalog(const UiDesignerCatalog *catalog)
{
    catalog_ = catalog;
    RebuildInventory();
}

void UiDesignerThemeGallery::SetThemeDocument(
    UiDesignerThemeDocument *theme)
{
    const bool changed = theme_ != theme;
    theme_ = theme;
    if(changed && theme_) {
        edit_dark_palette_ = theme_->GetEffective().UsesDarkPalette();
        palette_mode_.Select(edit_dark_palette_ ? 1 : 0);
    }
    SyncPaletteEditor();
    ApplyThemeStyles();
    Refresh();
}

void UiDesignerThemeGallery::SetFilter(const String& filter)
{
    filter_ = ToLower(filter);
    RebuildInventory();
}

void UiDesignerThemeGallery::SyncPaletteEditor()
{
    if(!theme_ || palette_syncing_)
        return;
    palette_syncing_ = true;
    const UiDesignerThemePalette& palette =
        theme_->GetEffective().GetPalette(edit_dark_palette_);
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        palette_editor_.SetSlotColor(i, palette.Get(i), false);
    palette_syncing_ = false;
}

void UiDesignerThemeGallery::CommitPaletteSlot(int slot)
{
    if(!theme_ || palette_syncing_ || slot < 0 ||
       slot >= UI_DESIGNER_THEME_PALETTE_SIZE)
        return;
    const String property = String("palette.") +
        (edit_dark_palette_ ? "dark." : "light.") + AsString(slot);
    String error;
    if(!theme_->Commit(property, palette_editor_.GetSlotColor(slot),
                       String(edit_dark_palette_ ? "Set Dark palette "
                                                : "Set Light palette ") +
                           AsString(slot + 1), error)) {
        SyncPaletteEditor();
        return;
    }
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::RebuildInventory()
{
    for(Ctrl& ctrl : inventory_controls_)
        ctrl.Remove();
    for(UiLabel& label : inventory_titles_)
        label.Remove();
    for(UiGroupPanel& tile : inventory_tiles_)
        tile.Remove();

    inventory_controls_.Clear();
    inventory_titles_.Clear();
    inventory_tiles_.Clear();

    if(!catalog_)
        return;

    for(const UiDesignerControlSpec& spec : catalog_->GetControls()) {
        if(spec.stock_upp)
            continue;
        if(filter_ == "inputs" && spec.category != "Ui Controls")
            continue;
        if(filter_ == "containers" &&
           spec.category != "Containers" &&
           spec.category != "Layouts")
            continue;

        UiGroupPanel& tile = inventory_tiles_.Add();
        tile.SetTitle(spec.display_name).SetLine(false).SetHeaderBand(false);
        Add(tile);

        UiLabel& title = inventory_titles_.Add();
        title.SetText(spec.type_id);
        tile.Add(title);

        One<Ctrl> created = UiDesignerPreviewFactory::Create(spec);
        if(created) {
            UiDesignerPreviewFactory::Initialize(*created, spec);
            Ctrl& control = inventory_controls_.Add(created.Detach());
            tile.Add(control);
        }
    }
    Layout();
    Refresh();
}

void UiDesignerThemeGallery::ApplyThemeStyles()
{
    const UiDesignerThemeSnapshot effective = theme_
        ? theme_->GetEffective() : UiDesignerThemeSnapshot();
    UiDesignerApplyGlobalTheme(effective);
    SyncPaletteEditor();

    const UiDesignerThemePalette& palette =
        effective.GetPalette(effective.UsesDarkPalette());

    role_standard_.SetText("Standard · P" +
                           AsString(effective.roles.control_standard + 1));
    role_subtle_.SetText("Subtle · P" +
                         AsString(effective.roles.control_subtle + 1));
    role_accent_.SetText("Accent · P" +
                         AsString(effective.roles.control_accent + 1));
    role_alert_.SetText("Alert · P" +
                        AsString(effective.roles.control_alert + 1));

    role_standard_.SetCustomStyle(ThemeBuilderRoleButtonStyle(
        UiRole::Standard, palette.Get(effective.roles.control_standard)));
    role_subtle_.SetCustomStyle(ThemeBuilderRoleButtonStyle(
        UiRole::Subtle, palette.Get(effective.roles.control_subtle)));
    role_accent_.SetCustomStyle(ThemeBuilderRoleButtonStyle(
        UiRole::Accent, palette.Get(effective.roles.control_accent)));
    role_alert_.SetCustomStyle(ThemeBuilderRoleButtonStyle(
        UiRole::Alert, palette.Get(effective.roles.control_alert)));

    const Color surface = palette.Get(effective.roles.panel_surface);
    const Color subtle = palette.Get(effective.roles.panel_subtle);
    const Color strong = palette.Get(effective.roles.panel_strong);
    panel_surface_.SetCustomStyle(
        ThemeBuilderPanelStyle(UiPanelRole::Surface, surface));
    panel_subtle_.SetCustomStyle(
        ThemeBuilderPanelStyle(UiPanelRole::Subtle, subtle));
    panel_strong_.SetCustomStyle(
        ThemeBuilderPanelStyle(UiPanelRole::Strong, strong));
    panel_surface_label_.SetCustomStyle(ThemeBuilderPanelLabelStyle(surface));
    panel_subtle_label_.SetCustomStyle(ThemeBuilderPanelLabelStyle(subtle));
    panel_strong_label_.SetCustomStyle(ThemeBuilderPanelLabelStyle(strong));
    panel_surface_label_.SetText("Surface · P" +
                                 AsString(effective.roles.panel_surface + 1));
    panel_subtle_label_.SetText("Subtle · P" +
                                AsString(effective.roles.panel_subtle + 1));
    panel_strong_label_.SetText("Strong · P" +
                                AsString(effective.roles.panel_strong + 1));

    progress_.SetCustomStyle(UiTheme::ResolveProgressBar(UiRole::Accent));
    Refresh();
}

void UiDesignerThemeGallery::Layout()
{
    const int gap = DPI(10);
    const int inset = DPI(12);
    const int w = max(DPI(760), GetSize().cx);
    const int column = max(DPI(250), (w - inset * 2 - gap * 2) / 3);
    int y = inset;

    Put(title_card_, inset, y, w - inset * 2, DPI(72));
    y += DPI(72) + gap;

    const int palette_h = DPI(445);
    Put(palette_group_, inset, y, w - inset * 2, palette_h);
    Put(palette_mode_, DPI(12), DPI(42), DPI(130), DPI(30));
    Put(palette_help_, DPI(154), DPI(40),
        w - inset * 2 - DPI(178), DPI(34));
    Put(palette_editor_, DPI(12), DPI(82),
        w - inset * 2 - DPI(24), palette_h - DPI(94));
    y += palette_h + gap;

    Put(control_roles_group_, inset, y, w - inset * 2, DPI(84));
    const int role_gap = DPI(8);
    const int role_inner = w - inset * 2 - DPI(24);
    const int role_w = max(DPI(110), (role_inner - role_gap * 3) / 4);
    Put(role_standard_, DPI(12), DPI(42), role_w, DPI(32));
    Put(role_subtle_, DPI(12) + role_w + role_gap, DPI(42), role_w, DPI(32));
    Put(role_accent_, DPI(12) + (role_w + role_gap) * 2, DPI(42), role_w, DPI(32));
    Put(role_alert_, DPI(12) + (role_w + role_gap) * 3, DPI(42), role_w, DPI(32));
    y += DPI(84) + gap;

    Put(panel_roles_group_, inset, y, w - inset * 2, DPI(118));
    const int panel_w = max(DPI(180), (role_inner - role_gap * 2) / 3);
    Put(panel_surface_, DPI(12), DPI(44), panel_w, DPI(60));
    Put(panel_subtle_, DPI(12) + panel_w + role_gap, DPI(44), panel_w, DPI(60));
    Put(panel_strong_, DPI(12) + (panel_w + role_gap) * 2, DPI(44), panel_w, DPI(60));
    panel_surface_label_.SetRect(0, 0, panel_w, DPI(60));
    panel_subtle_label_.SetRect(0, 0, panel_w, DPI(60));
    panel_strong_label_.SetRect(0, 0, panel_w, DPI(60));
    y += DPI(118) + gap;

    Put(toggle_group_, inset, y, column, DPI(150));
    Put(toggle_label_a_, DPI(12), DPI(42), DPI(160), DPI(30));
    Put(toggle_a_, column - DPI(70), DPI(42), DPI(52), DPI(30));
    Put(toggle_label_b_, DPI(12), DPI(84), DPI(160), DPI(30));
    Put(toggle_b_, column - DPI(70), DPI(84), DPI(52), DPI(30));

    Put(buttons_group_, inset + column + gap, y, column, DPI(150));
    Put(button_, DPI(12), DPI(42), DPI(110), DPI(32));
    Put(tool_button_, DPI(132), DPI(42), DPI(44), DPI(32));
    Put(split_button_, DPI(186), DPI(42), column - DPI(198), DPI(32));
    Put(breadcrumbs_, DPI(12), DPI(88), column - DPI(24), DPI(34));

    Put(numbers_group_, inset + (column + gap) * 2, y, column, DPI(150));
    Put(int_edit_, DPI(12), DPI(42), DPI(100), DPI(32));
    Put(float_edit_, DPI(122), DPI(42), DPI(110), DPI(32));
    Put(slider_, DPI(12), DPI(88), column - DPI(24), DPI(28));
    Put(progress_, DPI(12), DPI(120), column - DPI(24), DPI(20));
    y += DPI(150) + gap;

    Put(security_group_, inset, y, column, DPI(126));
    Put(mask_edit_, DPI(12), DPI(44), column - DPI(24), DPI(32));
    Put(password_edit_, DPI(12), DPI(82), column - DPI(24), DPI(32));

    Put(document_group_, inset + column + gap, y, column, DPI(126));
    Put(document_, DPI(12), DPI(42), column - DPI(24), DPI(72));

    Put(input_group_, inset + (column + gap) * 2, y, column, DPI(250));
    Put(line_edit_, DPI(12), DPI(42), column - DPI(24), DPI(32));
    Put(multi_edit_, DPI(12), DPI(80), column - DPI(24), DPI(58));
    Put(dropdown_, DPI(12), DPI(144), column - DPI(24), DPI(32));
    Put(check_, DPI(12), DPI(182), DPI(120), DPI(30));
    Put(radio_, DPI(142), DPI(182), DPI(120), DPI(30));
    Put(color_, DPI(12), DPI(216), DPI(120), DPI(28));
    Put(slider_edit_, DPI(142), DPI(216), column - DPI(154), DPI(28));
    y += DPI(260);

    Put(data_group_, inset, y, w - inset * 2, DPI(280));
    const int inner_w = w - inset * 2 - DPI(24);
    const int data_col = (inner_w - gap * 4) / 5;
    Put(tab_, DPI(12), DPI(42), data_col, DPI(220));
    Put(tree_, DPI(12) + (data_col + gap), DPI(42), data_col, DPI(220));
    Put(table_, DPI(12) + (data_col + gap) * 2, DPI(42), data_col, DPI(220));
    Put(list_, DPI(12) + (data_col + gap) * 3, DPI(42), data_col, DPI(220));
    Put(accordion_, DPI(12) + (data_col + gap) * 4, DPI(42), data_col, DPI(220));
    y += DPI(290);

    const int tile_w = max(DPI(220), (w - inset * 2 - gap * 2) / 3);
    for(int i = 0; i < inventory_tiles_.GetCount(); i++) {
        const int col = i % 3;
        const int row = i / 3;
        const int tx = inset + col * (tile_w + gap);
        const int ty = y + row * DPI(126);
        Put(inventory_tiles_[i], tx, ty, tile_w, DPI(116));
        Put(inventory_titles_[i], DPI(10), DPI(38), tile_w - DPI(20), DPI(22));
        if(i < inventory_controls_.GetCount())
            Put(inventory_controls_[i], DPI(10), DPI(64),
                tile_w - DPI(20), DPI(38));
    }
    content_height_ = y +
        ((inventory_tiles_.GetCount() + 2) / 3) * DPI(126) + inset;
}

void UiDesignerThemeGallery::Paint(Draw& w)
{
    w.DrawRect(GetSize(), SColorPaper());
}

}
