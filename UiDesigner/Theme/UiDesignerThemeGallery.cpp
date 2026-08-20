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

static String ThemeRoleName(UiRole role)
{
    switch(role) {
    case UiRole::Subtle: return "Subtle";
    case UiRole::Accent: return "Accent";
    case UiRole::Alert:  return "Alert";
    case UiRole::Standard:
    default:             return "Standard";
    }
}

// -----------------------------------------------------------------------------
// Compact working swatch

UiDesignerThemeSwatch::UiDesignerThemeSwatch()
{
    WantFocus();
    Tip("Theme palette colour — click to edit the six-colour palette, drag to a colour property");
}

UiDesignerThemeSwatch& UiDesignerThemeSwatch::SetColor(Color color)
{
    if(color_ == color)
        return *this;
    color_ = color;
    Tip(HexText() + " — click to edit palette; drag to a colour property; Ctrl+C copies hex");
    Refresh();
    return *this;
}

UiDesignerThemeSwatch& UiDesignerThemeSwatch::SetActive(bool active)
{
    if(active_ == active)
        return *this;
    active_ = active;
    Refresh();
    return *this;
}

String UiDesignerThemeSwatch::HexText() const
{
    return Format("#%02X%02X%02X", color_.GetR(), color_.GetG(), color_.GetB());
}

void UiDesignerThemeSwatch::Paint(Draw& w)
{
    Rect r(Point(0, 0), GetSize());
    w.DrawRect(r, SColorPaper());
    Rect face = r.Deflated(DPI(2));
    if(!face.IsEmpty())
        w.DrawRect(face, color_);
    DrawFrame(w, r, active_ ? SColorHighlight() : SColorShadow(),
              active_ ? DPI(2) : DPI(1));
    if(HasFocus())
        DrawFrame(w, r.Deflated(DPI(1)), SColorHighlight(), DPI(1));
}

void UiDesignerThemeSwatch::LeftDown(Point, dword)
{
    dragging_ = false;
    SetFocus();
}

void UiDesignerThemeSwatch::LeftUp(Point, dword)
{
    if(!dragging_)
        WhenAction();
    dragging_ = false;
}

void UiDesignerThemeSwatch::LeftDrag(Point, dword)
{
    dragging_ = true;
    VectorMap<String, ClipData> payload;
    Append(payload, HexText());
    DoDragAndDrop(payload, Image(), DND_COPY);
}

bool UiDesignerThemeSwatch::Key(dword key, int count)
{
    if(key == K_CTRL_C) {
        WriteClipboardText(HexText());
        return true;
    }
    if(key == K_ENTER || key == K_SPACE) {
        WhenAction();
        return true;
    }
    return Ctrl::Key(key, count);
}

// -----------------------------------------------------------------------------
// Palette popup. The full UiColorPicker belongs here temporarily, not in the
// Theme Builder canvas. It edits one six-colour appearance palette atomically.

class UiDesignerThemePaletteDialog : public TopWindow {
public:
    typedef UiDesignerThemePaletteDialog CLASSNAME;

    UiDesignerThemePaletteDialog(bool dark, const UiDesignerThemePalette& palette,
                                 int active_slot)
    {
        Title(dark ? "Dark theme palette" : "Light theme palette");
        Sizeable().Zoomable();
        SetRect(0, 0, DPI(760), DPI(600));
        Add(picker_.SizePos());

        picker_.EnableSessionPersistence(false)
               .SetSlotCount(UI_DESIGNER_THEME_PALETTE_SIZE)
               .SetGeneratorCount(UI_DESIGNER_THEME_PALETTE_SIZE)
               .SetPageMode(UiColorPicker::PAGE_GENERATOR)
               .SetMediumMode(UiColorPicker::MEDIUM_UI);
        for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
            picker_.SetSlotLabel(i, AsString(i + 1));
            picker_.SetSlotColor(i, palette.Get(i), false);
        }
        picker_.SetActiveSlot(minmax(active_slot, 0,
                                     UI_DESIGNER_THEME_PALETTE_SIZE - 1));
        picker_.WhenAccept = [=] { AcceptBreak(IDOK); };
        picker_.WhenCancel = [=] { RejectBreak(IDCANCEL); };
    }

    UiDesignerThemePalette GetPalette() const
    {
        UiDesignerThemePalette out;
        for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
            out.Set(i, picker_.GetSlotColor(i));
        return out;
    }

private:
    UiColorPicker picker_;
};

// -----------------------------------------------------------------------------
// Theme Builder toolbar

UiDesignerThemeToolbar::UiDesignerThemeToolbar()
{
    BackPaint();

    controls_mode_.SetIcon(ICON_DESIGN_WIDGETS_48())
                  .SetIconSize(DPI(16), DPI(16))
                  .SetCheckable().SetChecked(true)
                  .Tip("Controls preview");
    containers_mode_.SetIcon(ICON_DESIGN_TAB_GROUP_48())
                    .SetIconSize(DPI(16), DPI(16))
                    .SetCheckable()
                    .Tip("Containers preview");

    panel_role_label_.SetText("Panel Role")
                     .SetAlign(UiAlign::RIGHT, UiAlign::CENTER);
    control_role_label_.SetText("Ctrl Role")
                       .SetAlign(UiAlign::RIGHT, UiAlign::CENTER);

    panel_role_drop_.UseInternalModel().Clear()
                    .Add("Standard", (int)UiRole::Standard)
                    .Add("Subtle", (int)UiRole::Subtle)
                    .Add("Accent", (int)UiRole::Accent)
                    .Add("Alert", (int)UiRole::Alert);
    control_role_drop_.UseInternalModel().Clear()
                      .Add("Standard", (int)UiRole::Standard)
                      .Add("Subtle", (int)UiRole::Subtle)
                      .Add("Accent", (int)UiRole::Accent)
                      .Add("Alert", (int)UiRole::Alert);
    panel_role_drop_.Select((int)panel_role_);
    control_role_drop_.Select((int)control_role_);

    appearance_.SetIcon(ICON_ACTION_DARK_MODE_48())
               .SetIconSize(DPI(16), DPI(16))
               .SetCheckable()
               .Tip("Switch Light / Dark preview");

    light_label_.SetText("Light").SetAlign(UiAlign::LEFT, UiAlign::BOTTOM);
    dark_label_.SetText("Dark").SetAlign(UiAlign::LEFT, UiAlign::BOTTOM);

    Add(controls_mode_);
    Add(containers_mode_);
    Add(panel_role_label_);
    Add(panel_role_drop_);
    Add(control_role_label_);
    Add(control_role_drop_);
    Add(appearance_);
    Add(light_label_);
    Add(dark_label_);
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
        Add(light_[i]);
        Add(dark_[i]);
        const int slot = i;
        light_[i].WhenAction = [=] { EditPalette(false, slot); };
        dark_[i].WhenAction = [=] { EditPalette(true, slot); };
    }

    controls_mode_.WhenAction = [=] { SetPreviewMode("controls"); };
    containers_mode_.WhenAction = [=] { SetPreviewMode("containers"); };
    panel_role_drop_.WhenAction = [=] {
        if(!syncing_)
            SetPanelRole((UiRole)(int)panel_role_drop_.GetData());
    };
    control_role_drop_.WhenAction = [=] {
        if(!syncing_)
            SetControlRole((UiRole)(int)control_role_drop_.GetData());
    };
    appearance_.WhenAction = [=] { ToggleAppearance(); };
}

void UiDesignerThemeToolbar::SetThemeDocument(UiDesignerThemeDocument *theme)
{
    theme_ = theme;
    SyncFromTheme();
}

void UiDesignerThemeToolbar::SetGallery(UiDesignerThemeGallery *gallery)
{
    gallery_ = gallery;
    if(gallery_) {
        gallery_->SetPreviewMode(preview_mode_);
        gallery_->SetPanelRole(panel_role_);
        gallery_->SetControlRole(control_role_);
    }
}

void UiDesignerThemeToolbar::SetPreviewMode(const String& mode)
{
    preview_mode_ = ToLower(mode) == "containers" ? "containers" : "controls";
    UpdateModeButtons();
    if(gallery_)
        gallery_->SetPreviewMode(preview_mode_);
    WhenStatus(preview_mode_ == "containers" ? "Theme Builder: Containers"
                                               : "Theme Builder: Controls");
}

void UiDesignerThemeToolbar::SetPanelRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    panel_role_ = role;
    if(gallery_)
        gallery_->SetPanelRole(role);
    WhenStatus("Panel preview role: " + ThemeRoleName(role));
}

void UiDesignerThemeToolbar::SetControlRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    control_role_ = role;
    if(gallery_)
        gallery_->SetControlRole(role);
    WhenStatus("Control preview role: " + ThemeRoleName(role));
}

void UiDesignerThemeToolbar::ToggleAppearance()
{
    if(!theme_ || syncing_)
        return;
    const String next = theme_->GetEffective().mode == "Dark" ? "Light" : "Dark";
    String error;
    if(!theme_->Commit("mode", next, "Theme Builder " + next + " preview", error)) {
        WhenStatus(error);
        return;
    }
    SyncFromTheme();
    WhenStatus(next + " theme preview");
}

void UiDesignerThemeToolbar::EditPalette(bool dark, int active_slot)
{
    if(!theme_)
        return;

    UiDesignerThemePaletteDialog dialog(
        dark, theme_->GetEffective().GetPalette(dark), active_slot);
    dialog.CenterOwner();
    if(dialog.Run() != IDOK)
        return;

    String error;
    if(!theme_->CommitPalette(dark, dialog.GetPalette(),
                              String("Edit ") + (dark ? "Dark" : "Light") +
                                  " six-colour palette",
                              error)) {
        WhenStatus(error);
        return;
    }
    SyncFromTheme();
    WhenStatus(String(dark ? "Dark" : "Light") + " palette updated");
}

void UiDesignerThemeToolbar::UpdateModeButtons()
{
    controls_mode_.SetChecked(preview_mode_ == "controls");
    containers_mode_.SetChecked(preview_mode_ == "containers");
    controls_mode_.SetCustomStyle(UiTheme::ResolveToolButton(
        preview_mode_ == "controls" ? UiRole::Accent : UiRole::Subtle));
    containers_mode_.SetCustomStyle(UiTheme::ResolveToolButton(
        preview_mode_ == "containers" ? UiRole::Accent : UiRole::Subtle));
}

void UiDesignerThemeToolbar::SyncFromTheme()
{
    if(!theme_)
        return;
    syncing_ = true;
    const UiDesignerThemeSnapshot& value = theme_->GetEffective();
    appearance_.SetChecked(value.mode == "Dark");
    appearance_.Tip(value.mode == "Dark" ? "Switch to Light preview"
                                          : "Switch to Dark preview");
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++) {
        light_[i].SetColor(value.light_palette.Get(i));
        dark_[i].SetColor(value.dark_palette.Get(i));
        light_[i].SetActive(false);
        dark_[i].SetActive(false);
    }
    syncing_ = false;
    Refresh();
}

void UiDesignerThemeToolbar::ApplyTheme(const UiDesignerThemeSnapshot& theme)
{
    UiDesignerApplyGlobalTheme(theme);
    UpdateModeButtons();
    appearance_.SetCustomStyle(UiTheme::ResolveToolButton(UiRole::Accent));
    panel_role_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
    control_role_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
    light_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
    dark_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Subtle));
    panel_role_drop_.SetCustomStyle(UiTheme::ResolveDropdown(UiRole::Standard));
    control_role_drop_.SetCustomStyle(UiTheme::ResolveDropdown(UiRole::Standard));
    SyncFromTheme();
    Refresh();
}

void UiDesignerThemeToolbar::Layout()
{
    const int h = GetSize().cy;
    const int gap = DPI(6);
    const int small_gap = DPI(3);
    const int control_h = min(DPI(30), max(0, h - DPI(8)));
    const int y = max(0, (h - control_h) / 2);
    int x = DPI(4);

    controls_mode_.SetRect(x, y, DPI(30), control_h); x += DPI(30) + small_gap;
    containers_mode_.SetRect(x, y, DPI(30), control_h); x += DPI(30) + gap;

    panel_role_label_.SetRect(x, y, DPI(62), control_h); x += DPI(62) + DPI(4);
    panel_role_drop_.SetRect(x, y, DPI(118), control_h); x += DPI(118) + gap;

    control_role_label_.SetRect(x, y, DPI(54), control_h); x += DPI(54) + DPI(4);
    control_role_drop_.SetRect(x, y, DPI(118), control_h); x += DPI(118) + gap;

    appearance_.SetRect(x, y, DPI(34), control_h); x += DPI(34) + DPI(10);

    const int available = max(0, GetSize().cx - x - DPI(4));
    const int palette_gap = DPI(10);
    const int group_width = max(0, (available - palette_gap) / 2);
    const int swatch = min(DPI(24), max(DPI(14),
        (group_width - small_gap * (UI_DESIGNER_THEME_PALETTE_SIZE - 1)) /
            UI_DESIGNER_THEME_PALETTE_SIZE));
    const int swatch_y = max(DPI(17), h - swatch - DPI(3));
    const int label_h = min(DPI(16), swatch_y);

    light_label_.SetRect(x, 0, group_width, label_h);
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        light_[i].SetRect(x + i * (swatch + small_gap), swatch_y, swatch, swatch);

    x += group_width + palette_gap;
    dark_label_.SetRect(x, 0, group_width, label_h);
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        dark_[i].SetRect(x + i * (swatch + small_gap), swatch_y, swatch, swatch);
}

void UiDesignerThemeToolbar::Paint(Draw& w)
{
    w.DrawRect(GetSize(), SColorPaper());
    w.DrawLine(0, GetSize().cy - 1, GetSize().cx, GetSize().cy - 1,
               DPI(1), SColorShadow());
}

// -----------------------------------------------------------------------------
// Matrix-driven preview

UiDesignerThemeGallery::UiDesignerThemeGallery()
{
    BackPaint();
    BuildPreviewMatrices();
    BuildControlSamples();
    BuildContainerSamples();
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::BuildPreviewMatrices()
{
    Add(preview_stack_);
    preview_stack_.Add(controls_matrix_, "controls");
    preview_stack_.Add(containers_matrix_, "containers");
    preview_stack_.SetActiveKey("controls");

    controls_matrix_.SetGridSize(3, 1).SetGap(DPI(10)).SetInset(DPI(0));
    containers_matrix_.SetGridSize(3, 1).SetGap(DPI(10)).SetInset(DPI(0));

    for(int i = 0; i < 3; i++) {
        control_columns_[i].SetDirection(UiDirection::V)
                           .SetGap(DPI(10))
                           .SetInset(DPI(0))
                           .SetAlignItems(UiCrossAlign::Stretch);
        container_columns_[i].SetDirection(UiDirection::V)
                             .SetGap(DPI(10))
                             .SetInset(DPI(0))
                             .SetAlignItems(UiCrossAlign::Stretch);
        controls_matrix_.AddGrid(control_columns_[i], 0, i, true, true);
        containers_matrix_.AddGrid(container_columns_[i], 0, i, true, true);
    }
}

void UiDesignerThemeGallery::BuildControlSamples()
{
    controls_reference_label_.SetText("Plain panel hosting the selected control role")
                             .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    controls_reference_button_.SetText("Reference button");
    controls_reference_panel_.Add(controls_reference_label_);
    controls_reference_panel_.Add(controls_reference_button_);

    buttons_group_.SetTitle("BUTTONS")
                  .SetSubTitle("Button family on the selected panel role");
    button_.SetText("Button");
    tool_button_.SetIcon(ICON_DESIGN_SETTINGS_48()).SetIconSize(DPI(18), DPI(18));
    split_button_.SetText("Save").SetSplitWidth(DPI(28))
                 .Add("Save", "save").Add("Save As", "save_as");
    breadcrumbs_.AddCrumb("Home", "0");
    breadcrumbs_.AddCrumb("Theme", "1");
    breadcrumbs_.AddCrumb("Control", "2");
    breadcrumbs_.SetCurrentIndex(2);
    buttons_group_.Add(button_);
    buttons_group_.Add(tool_button_);
    buttons_group_.Add(split_button_);
    buttons_group_.Add(breadcrumbs_);

    choices_group_.SetTitle("CHOICES")
                  .SetSubTitle("Selection and toggle controls");
    check_.SetText("Check box").SetData(true);
    radio_.SetText("Radio button").SetData(true);
    toggle_.SetOn(true);
    dropdown_.UseInternalModel().Clear()
             .Add("First choice", 1).Add("Second choice", 2).Add("Third choice", 3);
    dropdown_.Select(0);
    choices_group_.Add(check_);
    choices_group_.Add(radio_);
    choices_group_.Add(toggle_);
    choices_group_.Add(dropdown_);

    numbers_group_.SetTitle("NUMERIC & SLIDERS")
                  .SetSubTitle("Dense numeric controls");
    int_edit_.MinMax(0, 100).Step(1).ShowSpin(true).SetValue(42);
    float_edit_.MinMax(0, 100).Step(0.1).Precision(2).ShowSpin(true).SetValue(3.14);
    slider_.SetRange(0, 100).SetValue(56);
    progress_.Percent(true).Set(68, 100);
    numbers_group_.Add(int_edit_);
    numbers_group_.Add(float_edit_);
    numbers_group_.Add(slider_);
    numbers_group_.Add(progress_);

    inputs_group_.SetTitle("INPUTS")
                 .SetSubTitle("Text and compound editors");
    line_edit_.SetData("Line edit");
    multi_edit_.SetData("Multiline\ncontent");
    slider_edit_.SetRange(0, 100).SetValue(38);
    inputs_group_.Add(line_edit_);
    inputs_group_.Add(multi_edit_);
    inputs_group_.Add(slider_edit_);

    data_group_.SetTitle("DATA")
               .SetSubTitle("List, tree and table");
    list_.Model().Add(UiModelItem("First", 1));
    list_.Model().Add(UiModelItem("Second", 2));
    tree_.Model().AddChild(tree_.Model().Root(), UiModelItem("Workspace", "workspace"));
    tree_.Model().AddChild(tree_.Model().Root(), UiModelItem("Assets", "assets"));
    table_.UseInternalModel();
    table_.Model().SetSize(4, 2);
    data_group_.Add(list_);
    data_group_.Add(tree_);
    data_group_.Add(table_);

    navigation_group_.SetTitle("NAVIGATION")
                     .SetSubTitle("Tabs and accordion");
    tab_.SetVisual(UITAB_UNDERLINE).SetPlacement(UiAlign::TOP)
        .EnableCloseButtons(false).EnableDragHandles(false);
    tab_.Add(tab_page_a_, "First");
    tab_.Add(tab_page_b_, "Second");
    tab_.SetActiveTab(0);
    accordion_.SetSingleOpen(false).SetEnforceOne(false)
              .ShowChevron(true).SetAnimation(true, 120, 0);
    const int first = accordion_.AddSection("First", true);
    const int second = accordion_.AddSection("Second", false);
    accordion_.GetSectionContent(first).Add(accordion_a_.SizePos());
    accordion_.GetSectionContent(second).Add(accordion_b_.SizePos());
    navigation_group_.Add(tab_);
    navigation_group_.Add(accordion_);

    feedback_group_.SetTitle("FEEDBACK")
                   .SetSubTitle("Status and progress");
    feedback_label_.SetText("Ready to preview the selected role")
                   .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    feedback_progress_.Percent(true).Set(72, 100);
    feedback_group_.Add(feedback_label_);
    feedback_group_.Add(feedback_progress_);

    control_columns_[0].Add(controls_reference_panel_).Fixed(DPI(108));
    control_columns_[0].Add(buttons_group_).Fixed(DPI(164));
    control_columns_[0].Add(choices_group_).Fixed(DPI(178));

    control_columns_[1].Add(numbers_group_).Fixed(DPI(178));
    control_columns_[1].Add(inputs_group_).Fixed(DPI(226));
    control_columns_[1].Add(feedback_group_).Fixed(DPI(126));

    control_columns_[2].Add(data_group_).Fixed(DPI(252));
    control_columns_[2].Add(navigation_group_).Fixed(DPI(292));
}

void UiDesignerThemeGallery::BuildContainerSamples()
{
    container_plain_label_.SetText("Plain panel · no representative controls")
                          .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    container_plain_panel_.Add(container_plain_label_);

    container_controls_label_.SetText("Plain panel · with controls")
                             .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    container_button_.SetText("Button");
    container_check_.SetText("Check box").SetData(true);
    container_controls_panel_.Add(container_controls_label_);
    container_controls_panel_.Add(container_button_);
    container_controls_panel_.Add(container_check_);

    container_plain_group_.SetTitle("GROUP PANEL · QUIET")
                          .SetSubTitle("Container chrome without working controls");
    container_group_label_.SetText("The group title remains part of the container theme")
                          .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    container_plain_group_.Add(container_group_label_);

    container_edit_group_.SetTitle("GROUP PANEL · FORM")
                         .SetSubTitle("Edit + action on the selected panel role");
    container_edit_.SetData("Editable value");
    container_edit_button_.SetText("Apply");
    container_edit_group_.Add(container_edit_);
    container_edit_group_.Add(container_edit_button_);

    container_numeric_label_.SetText("Plain panel · numeric content")
                            .SetAlign(UiAlign::LEFT, UiAlign::CENTER);
    container_slider_.SetRange(0, 100).SetValue(61);
    container_int_.MinMax(0, 100).SetValue(61);
    container_numeric_panel_.Add(container_numeric_label_);
    container_numeric_panel_.Add(container_slider_);
    container_numeric_panel_.Add(container_int_);

    container_choice_group_.SetTitle("GROUP PANEL · CHOICES")
                           .SetSubTitle("Dropdown + toggle for contrast checking");
    container_dropdown_.UseInternalModel().Clear()
                       .Add("Primary", 1).Add("Secondary", 2);
    container_dropdown_.Select(0);
    container_toggle_.SetOn(true);
    container_choice_group_.Add(container_dropdown_);
    container_choice_group_.Add(container_toggle_);

    container_columns_[0].Add(container_plain_panel_).Fixed(DPI(154));
    container_columns_[0].Add(container_controls_panel_).Fixed(DPI(196));
    container_columns_[1].Add(container_plain_group_).Fixed(DPI(176));
    container_columns_[1].Add(container_edit_group_).Fixed(DPI(196));
    container_columns_[2].Add(container_numeric_panel_).Fixed(DPI(196));
    container_columns_[2].Add(container_choice_group_).Fixed(DPI(196));
}

void UiDesignerThemeGallery::SetCatalog(const UiDesignerCatalog *catalog)
{
    catalog_ = catalog;
}

void UiDesignerThemeGallery::SetThemeDocument(UiDesignerThemeDocument *theme)
{
    theme_ = theme;
    ApplyThemeStyles();
    Refresh();
}

void UiDesignerThemeGallery::SetPreviewMode(const String& mode)
{
    const String next = ToLower(mode) == "containers" ? "containers" : "controls";
    if(preview_mode_ == next)
        return;
    preview_mode_ = next;
    preview_stack_.SetActiveKey(preview_mode_);
    Layout();
    Refresh();
}

void UiDesignerThemeGallery::SetPanelRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    if(panel_role_ == role)
        return;
    panel_role_ = role;
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::SetControlRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    if(control_role_ == role)
        return;
    control_role_ = role;
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::RefreshTheme()
{
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::ApplyThemeStyles()
{
    const UiDesignerThemeSnapshot effective = theme_
        ? theme_->GetEffective() : UiDesignerThemeSnapshot();
    UiDesignerApplyGlobalTheme(effective);

    const UiPanel::Style panel_style = UiTheme::ResolvePanel(panel_role_);
    controls_reference_panel_.SetCustomStyle(panel_style);
    container_plain_panel_.SetCustomStyle(panel_style);
    container_controls_panel_.SetCustomStyle(panel_style);
    container_numeric_panel_.SetCustomStyle(panel_style);

    const UiGroupPanel::Style group_style = UiTheme::ResolveGroupPanel(panel_role_);
    buttons_group_.SetCustomStyle(group_style);
    choices_group_.SetCustomStyle(group_style);
    numbers_group_.SetCustomStyle(group_style);
    inputs_group_.SetCustomStyle(group_style);
    data_group_.SetCustomStyle(group_style);
    navigation_group_.SetCustomStyle(group_style);
    feedback_group_.SetCustomStyle(group_style);
    container_plain_group_.SetCustomStyle(group_style);
    container_edit_group_.SetCustomStyle(group_style);
    container_choice_group_.SetCustomStyle(group_style);

    controls_reference_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Standard));
    controls_reference_button_.SetCustomStyle(UiTheme::ResolveButton(control_role_));
    button_.SetCustomStyle(UiTheme::ResolveButton(control_role_));
    tool_button_.SetCustomStyle(UiTheme::ResolveToolButton(control_role_));
    split_button_.SetCustomStyle(UiTheme::ResolveButton(control_role_));
    check_.SetCustomStyle(UiTheme::ResolveCheckBox(control_role_));
    radio_.SetCustomStyle(UiTheme::ResolveRadioButton(control_role_));
    toggle_.SetCustomStyle(UiTheme::ResolveToggle(control_role_));
    dropdown_.SetCustomStyle(UiTheme::ResolveDropdown(control_role_));
    int_edit_.SetCustomStyle(UiTheme::ResolveEdit(control_role_));
    float_edit_.SetCustomStyle(UiTheme::ResolveEdit(control_role_));
    slider_.SetCustomStyle(UiTheme::ResolveSlider(control_role_));
    progress_.SetCustomStyle(UiTheme::ResolveProgressBar(control_role_));
    line_edit_.SetCustomStyle(UiTheme::ResolveEdit(control_role_));
    multi_edit_.SetCustomStyle(UiTheme::ResolveEdit(control_role_));
    tab_.SetCustomStyle(UiTheme::ResolveTab(control_role_, UITAB_UNDERLINE));
    feedback_label_.SetCustomStyle(UiTheme::ResolveLabel(control_role_));
    feedback_progress_.SetCustomStyle(UiTheme::ResolveProgressBar(control_role_));

    container_plain_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Standard));
    container_controls_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Standard));
    container_numeric_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Standard));
    container_group_label_.SetCustomStyle(UiTheme::ResolveLabel(UiRole::Standard));
    container_button_.SetCustomStyle(UiTheme::ResolveButton(control_role_));
    container_check_.SetCustomStyle(UiTheme::ResolveCheckBox(control_role_));
    container_edit_.SetCustomStyle(UiTheme::ResolveEdit(control_role_));
    container_edit_button_.SetCustomStyle(UiTheme::ResolveButton(control_role_));
    container_slider_.SetCustomStyle(UiTheme::ResolveSlider(control_role_));
    container_int_.SetCustomStyle(UiTheme::ResolveEdit(control_role_));
    container_dropdown_.SetCustomStyle(UiTheme::ResolveDropdown(control_role_));
    container_toggle_.SetCustomStyle(UiTheme::ResolveToggle(control_role_));

    Layout();
    Refresh();
}

void UiDesignerThemeGallery::LayoutControlSamples()
{
    const int inset = DPI(12);

    int w = controls_reference_panel_.GetSize().cx;
    controls_reference_label_.SetRect(inset, DPI(10), max(0, w - inset * 2), DPI(28));
    controls_reference_button_.SetRect(inset, DPI(50), max(DPI(100), w - inset * 2), DPI(32));

    w = buttons_group_.GetSize().cx;
    button_.SetRect(inset, DPI(48), DPI(92), DPI(32));
    tool_button_.SetRect(inset + DPI(100), DPI(48), DPI(42), DPI(32));
    split_button_.SetRect(inset + DPI(150), DPI(48), max(DPI(94), w - DPI(162)), DPI(32));
    breadcrumbs_.SetRect(inset, DPI(94), max(0, w - inset * 2), DPI(34));

    w = choices_group_.GetSize().cx;
    check_.SetRect(inset, DPI(48), max(0, w / 2 - inset), DPI(30));
    radio_.SetRect(max(inset, w / 2), DPI(48), max(0, w / 2 - inset), DPI(30));
    toggle_.SetRect(inset, DPI(88), DPI(58), DPI(30));
    dropdown_.SetRect(inset + DPI(70), DPI(88), max(DPI(100), w - inset * 2 - DPI(70)), DPI(32));

    w = numbers_group_.GetSize().cx;
    int_edit_.SetRect(inset, DPI(48), max(DPI(72), (w - inset * 2 - DPI(8)) / 2), DPI(32));
    float_edit_.SetRect(inset + (w - inset * 2 - DPI(8)) / 2 + DPI(8), DPI(48),
                       max(DPI(72), (w - inset * 2 - DPI(8)) / 2), DPI(32));
    slider_.SetRect(inset, DPI(92), max(0, w - inset * 2), DPI(28));
    progress_.SetRect(inset, DPI(130), max(0, w - inset * 2), DPI(20));

    w = inputs_group_.GetSize().cx;
    line_edit_.SetRect(inset, DPI(48), max(0, w - inset * 2), DPI(32));
    multi_edit_.SetRect(inset, DPI(90), max(0, w - inset * 2), DPI(70));
    slider_edit_.SetRect(inset, DPI(174), max(0, w - inset * 2), DPI(30));

    w = feedback_group_.GetSize().cx;
    feedback_label_.SetRect(inset, DPI(46), max(0, w - inset * 2), DPI(26));
    feedback_progress_.SetRect(inset, DPI(82), max(0, w - inset * 2), DPI(20));

    w = data_group_.GetSize().cx;
    const int data_gap = DPI(6);
    const int data_w = max(DPI(60), (w - inset * 2 - data_gap * 2) / 3);
    list_.SetRect(inset, DPI(48), data_w, DPI(178));
    tree_.SetRect(inset + data_w + data_gap, DPI(48), data_w, DPI(178));
    table_.SetRect(inset + (data_w + data_gap) * 2, DPI(48), data_w, DPI(178));

    w = navigation_group_.GetSize().cx;
    tab_.SetRect(inset, DPI(48), max(0, w - inset * 2), DPI(102));
    accordion_.SetRect(inset, DPI(160), max(0, w - inset * 2), DPI(108));
}

void UiDesignerThemeGallery::LayoutContainerSamples()
{
    const int inset = DPI(12);
    int w = container_plain_panel_.GetSize().cx;
    container_plain_label_.SetRect(inset, DPI(14), max(0, w - inset * 2), DPI(30));

    w = container_controls_panel_.GetSize().cx;
    container_controls_label_.SetRect(inset, DPI(12), max(0, w - inset * 2), DPI(28));
    container_button_.SetRect(inset, DPI(58), DPI(110), DPI(32));
    container_check_.SetRect(inset, DPI(104), max(0, w - inset * 2), DPI(30));

    w = container_plain_group_.GetSize().cx;
    container_group_label_.SetRect(inset, DPI(58), max(0, w - inset * 2), DPI(44));

    w = container_edit_group_.GetSize().cx;
    container_edit_.SetRect(inset, DPI(58), max(0, w - inset * 2), DPI(32));
    container_edit_button_.SetRect(inset, DPI(106), DPI(110), DPI(32));

    w = container_numeric_panel_.GetSize().cx;
    container_numeric_label_.SetRect(inset, DPI(12), max(0, w - inset * 2), DPI(28));
    container_slider_.SetRect(inset, DPI(62), max(0, w - inset * 2), DPI(28));
    container_int_.SetRect(inset, DPI(108), DPI(100), DPI(32));

    w = container_choice_group_.GetSize().cx;
    container_dropdown_.SetRect(inset, DPI(58), max(0, w - inset * 2), DPI(32));
    container_toggle_.SetRect(inset, DPI(108), DPI(58), DPI(30));
}

void UiDesignerThemeGallery::Layout()
{
    preview_stack_.SetRect(0, 0, GetSize().cx, GetSize().cy);
    controls_matrix_.SetRect(0, 0, GetSize().cx, GetSize().cy);
    containers_matrix_.SetRect(0, 0, GetSize().cx, GetSize().cy);
    controls_matrix_.Layout();
    containers_matrix_.Layout();
    for(int i = 0; i < 3; i++) {
        control_columns_[i].Layout();
        container_columns_[i].Layout();
    }
    LayoutControlSamples();
    LayoutContainerSamples();
}

void UiDesignerThemeGallery::Paint(Draw& w)
{
    w.DrawRect(GetSize(), SColorPaper());
}

}