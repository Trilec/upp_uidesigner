#include "UiDesignerThemeGallery.h"
#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Preview/UiDesignerPreview.h>
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

static String PanelRoleName(UiPanelRole role)
{
    switch(role) {
    case UiPanelRole::Subtle: return "Subtle";
    case UiPanelRole::Strong: return "Strong";
    case UiPanelRole::Surface:
    default:                  return "Surface";
    }
}

static UiRole PanelRoleAsControlRole(UiPanelRole role)
{
    switch(role) {
    case UiPanelRole::Subtle: return UiRole::Subtle;
    case UiPanelRole::Strong: return UiRole::Accent;
    case UiPanelRole::Surface:
    default:                  return UiRole::Standard;
    }
}

static void PopulateSampleNode(UiDesignerNode& node,
                               const UiDesignerControlSpec& spec,
                               const String& role)
{
    node.type = spec.type_id;
    for(int i = 0; i < spec.defaults.GetCount(); i++)
        node.SetProperty(AsString(spec.defaults.GetKey(i)),
                         spec.defaults.GetValue(i));
    node.SetProperty("role", role);
}

static bool IsThemeStudioPreviewProperty(const String& id)
{
    static const char *ids[] = {
        "icon", "icon_render_mode", "icon_mode", "icon_side",
        "icon_width", "icon_height", "icon_size", "scale_icon_to_content",
        "content_gap", "align_h", "align_v"
    };
    for(const char *candidate : ids)
        if(id == candidate)
            return true;
    return false;
}

static String ThemeStudioPreviewGroup(const String& id)
{
    return id == "icon" || id == "icon_render_mode" || id == "icon_mode"
        ? "Preview / Content" : "Preview / Layout";
}

static String ThemeStudioAppearanceGroup(const String& group)
{
    if(group.IsEmpty() || group == "General")
        return "Appearance";
    if(group == "Appearance")
        return "Appearance / Control";
    if(group.StartsWith("Appearance / "))
        return group;
    return "Appearance / " + group;
}

static String ThemeStudioPreviewTarget(const String& type, bool panel_sample)
{
    return String(panel_sample ? "panel|" : "control|") + type;
}

static Value ThemeStudioPreviewDefault(const UiDesignerPropertySpec& property)
{
    if(property.id == "icon" &&
       (IsNull(property.default_value) ||
        AsString(property.default_value).IsEmpty() ||
        AsString(property.default_value) == "None" ||
        AsString(property.default_value) == "Default"))
        return String("ICON_DESIGN_WIDGETS_48");
    if(property.id == "icon_render_mode" || property.id == "icon_mode")
        return String("MonoTint");
    if(property.id == "icon_width" || property.id == "icon_height" ||
       property.id == "icon_size") {
        const int authored = IsNumber(property.default_value)
            ? (int)property.default_value : 0;
        return max(24, authored);
    }
    return property.default_value;
}

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
    DrawFatFrame(w, r, active_ ? SColorHighlight() : SColorShadow(),
                 active_ ? DPI(2) : DPI(1));
    if(HasFocus())
        DrawFatFrame(w, r.Deflated(DPI(1)), SColorHighlight(), DPI(1));
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
                    .Add("Surface", (int)UiPanelRole::Surface)
                    .Add("Subtle", (int)UiPanelRole::Subtle)
                    .Add("Strong", (int)UiPanelRole::Strong);
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

    light_label_.SetText("Light").SetAlign(UiAlign::RIGHT, UiAlign::CENTER);
    dark_label_.SetText("Dark").SetAlign(UiAlign::RIGHT, UiAlign::CENTER);

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
            SetPanelRole((UiPanelRole)(int)panel_role_drop_.GetData());
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

void UiDesignerThemeToolbar::SetPanelRole(UiPanelRole role)
{
    if(!UiIsValid(role))
        role = UiPanelRole::Surface;
    panel_role_ = role;
    if(gallery_)
        gallery_->SetPanelRole(role);
    WhenStatus("Panel preview role: " + PanelRoleName(role));
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
    panel_role_drop_.SetRect(x, y, DPI(112), control_h); x += DPI(112) + gap;

    control_role_label_.SetRect(x, y, DPI(54), control_h); x += DPI(54) + DPI(4);
    control_role_drop_.SetRect(x, y, DPI(112), control_h); x += DPI(112) + gap;

    appearance_.SetRect(x, y, DPI(34), control_h); x += DPI(34) + DPI(8);

    const int label_w = DPI(32);
    const int label_gap = DPI(4);
    const int palette_gap = DPI(10);
    const int available = max(0, GetSize().cx - x - DPI(4));
    const int group_w = max(0, (available - palette_gap) / 2);
    const int swatch_space = max(0, group_w - label_w - label_gap);
    const int swatch = min(DPI(24), max(DPI(12),
        (swatch_space - small_gap * (UI_DESIGNER_THEME_PALETTE_SIZE - 1)) /
            UI_DESIGNER_THEME_PALETTE_SIZE));
    const int swatch_y = max(0, (h - swatch) / 2);

    light_label_.SetRect(x, y, label_w, control_h);
    int sx = x + label_w + label_gap;
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        light_[i].SetRect(sx + i * (swatch + small_gap), swatch_y, swatch, swatch);

    x += group_w + palette_gap;
    dark_label_.SetRect(x, y, label_w, control_h);
    sx = x + label_w + label_gap;
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; i++)
        dark_[i].SetRect(sx + i * (swatch + small_gap), swatch_y, swatch, swatch);
}

void UiDesignerThemeToolbar::Paint(Draw& w)
{
    w.DrawRect(GetSize(), SColorPaper());
    w.DrawLine(0, GetSize().cy - 1, GetSize().cx, GetSize().cy - 1,
               DPI(1), SColorShadow());
}

UiDesignerThemeGallery::UiDesignerThemeGallery()
{
    BackPaint();
    BuildPreviewMatrices();
    BuildControlSamples();
    BuildContainerSamples();
    BindSelectableSamples();
    ApplyThemeStyles();
}

UiDesignerThemeGallery::~UiDesignerThemeGallery()
{
    if(theme_)
        theme_->ClearPropertyModelProvider();
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
                  .SetSubTitle("Numeric, range and scrolling surfaces");
    int_edit_.MinMax(0, 100).Step(1).ShowSpin(true).SetValue(42);
    float_edit_.MinMax(0, 100).Step(0.1).Precision(2).ShowSpin(true).SetValue(3.14);
    slider_.SetRange(0, 100).SetValue(56);
    progress_.Percent(true).Set(68, 100);
    scroll_bar_.SetDirection(UiDirection::H).SetRange(0, 100, 24).SetPos(46);
    numbers_group_.Add(int_edit_);
    numbers_group_.Add(float_edit_);
    numbers_group_.Add(slider_);
    numbers_group_.Add(progress_);
    numbers_group_.Add(scroll_bar_);

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
    tab_.SetVisual(UITAB_CLASSIC).SetPlacement(UiAlign::TOP)
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

    control_columns_[1].Add(numbers_group_).Fixed(DPI(212));
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

    container_scroll_label_.SetText("Scroll Panel · viewport and content surface")
                           .SetAlign(UiAlign::LEFT, UiAlign::TOP);
    container_scroll_panel_.SetScrollMode(UIPANELSCROLL_AUTO);
    container_scroll_panel_.Content().Add(container_scroll_label_);

    container_columns_[0].Add(container_plain_panel_).Fixed(DPI(154));
    container_columns_[0].Add(container_controls_panel_).Fixed(DPI(196));
    container_columns_[0].Add(container_scroll_panel_).Fixed(DPI(160));
    container_columns_[1].Add(container_plain_group_).Fixed(DPI(176));
    container_columns_[1].Add(container_edit_group_).Fixed(DPI(196));
    container_columns_[2].Add(container_numeric_panel_).Fixed(DPI(196));
    container_columns_[2].Add(container_choice_group_).Fixed(DPI(196));
}

void UiDesignerThemeGallery::BindSelectableSamples()
{
    const auto bind = [=](UiDesignerThemeSelectableBase& sample,
                          const char *type, bool panel_sample) {
        UiDesignerThemeSelectableBase *ptr = &sample;
        const String type_id = type;
        sample.WhenThemeSelect = [=] { SelectSample(type_id, ptr, panel_sample); };
    };

    bind(controls_reference_panel_, "UiPanel", true);
    bind(controls_reference_label_, "UiLabel", false);
    bind(controls_reference_button_, "UiButton", false);
    bind(buttons_group_, "UiGroupPanel", true);
    bind(button_, "UiButton", false);
    bind(tool_button_, "UiToolButton", false);
    bind(split_button_, "UiSplitButton", false);
    bind(choices_group_, "UiGroupPanel", true);
    bind(check_, "UiCheckBox", false);
    bind(radio_, "UiRadioButton", false);
    bind(toggle_, "UiToggle", false);
    bind(dropdown_, "UiDropdown", false);
    bind(numbers_group_, "UiGroupPanel", true);
    bind(int_edit_, "UiIntEdit", false);
    bind(float_edit_, "UiFloatEdit", false);
    bind(slider_, "UiSlider", false);
    bind(progress_, "UiProgressBar", false);
    bind(scroll_bar_, "UiScrollBar", false);
    bind(inputs_group_, "UiGroupPanel", true);
    bind(line_edit_, "UiLineEdit", false);
    bind(multi_edit_, "UiMultiEdit", false);
    bind(data_group_, "UiGroupPanel", true);
    bind(list_, "UiList", false);
    bind(tree_, "UiTree", false);
    bind(navigation_group_, "UiGroupPanel", true);
    bind(tab_, "UiTab", false);
    bind(accordion_, "UiAccordion", false);
    bind(feedback_group_, "UiGroupPanel", true);
    bind(feedback_label_, "UiLabel", false);
    bind(feedback_progress_, "UiProgressBar", false);

    bind(container_plain_panel_, "UiPanel", true);
    bind(container_plain_label_, "UiLabel", false);
    bind(container_controls_panel_, "UiPanel", true);
    bind(container_controls_label_, "UiLabel", false);
    bind(container_button_, "UiButton", false);
    bind(container_check_, "UiCheckBox", false);
    bind(container_plain_group_, "UiGroupPanel", true);
    bind(container_group_label_, "UiLabel", false);
    bind(container_edit_group_, "UiGroupPanel", true);
    bind(container_edit_, "UiLineEdit", false);
    bind(container_edit_button_, "UiButton", false);
    bind(container_numeric_panel_, "UiPanel", true);
    bind(container_numeric_label_, "UiLabel", false);
    bind(container_slider_, "UiSlider", false);
    bind(container_int_, "UiIntEdit", false);
    bind(container_choice_group_, "UiGroupPanel", true);
    bind(container_dropdown_, "UiDropdown", false);
    bind(container_toggle_, "UiToggle", false);
    bind(container_scroll_panel_, "UiScrollPanel", true);
    bind(container_scroll_label_, "UiLabel", false);
}

void UiDesignerThemeGallery::SetCatalog(const UiDesignerCatalog *catalog)
{
    catalog_ = catalog;
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::SetThemeDocument(UiDesignerThemeDocument *theme)
{
    if(theme_ != theme) {
        if(theme_)
            theme_->ClearPropertyModelProvider();
        theme_ = theme;
        if(theme_) {
            theme_->SetPropertyModelProvider(
                [=](PropertyEditorModel& model,
                    const UiDesignerThemeSnapshot& value) {
                    BuildSelectedPropertyModel(model, value);
                });
        }
    }
    SyncSelectedTarget();
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::SetPreviewMode(const String& mode)
{
    const String next = ToLower(mode) == "containers" ? "containers" : "controls";
    if(preview_mode_ == next)
        return;
    preview_mode_ = next;
    preview_stack_.SetActiveKey(preview_mode_);
    if(selected_sample_)
        selected_sample_->SetThemeSelected(false);
    selected_sample_ = nullptr;
    selected_type_.Clear();
    selected_panel_sample_ = false;
    SyncSelectedTarget();
    Layout();
    Refresh();
}

void UiDesignerThemeGallery::SetPanelRole(UiPanelRole role)
{
    if(!UiIsValid(role))
        role = UiPanelRole::Surface;
    if(panel_role_ == role)
        return;
    panel_role_ = role;
    SyncSelectedTarget();
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::SetControlRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    if(control_role_ == role)
        return;
    control_role_ = role;
    SyncSelectedTarget();
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::RefreshTheme()
{
    SyncSelectedTarget();
    ApplyThemeStyles();
}

void UiDesignerThemeGallery::SelectSample(
    const String& type, UiDesignerThemeSelectableBase *sample, bool panel_sample)
{
    if(selected_sample_ && selected_sample_ != sample)
        selected_sample_->SetThemeSelected(false);
    selected_sample_ = sample;
    selected_type_ = type;
    selected_panel_sample_ = panel_sample;
    if(selected_sample_)
        selected_sample_->SetThemeSelected(true);
    SyncSelectedTarget();
    Refresh();
}

String UiDesignerThemeGallery::CurrentStyleTarget(
    const UiDesignerThemeSnapshot& theme, const String& type,
    bool panel_sample) const
{
    const String appearance = theme.mode == "Dark" ? "Dark" : "Light";
    const String role = panel_sample ? PanelRoleName(panel_role_)
                                     : ThemeRoleName(control_role_);
    return appearance + "|" + (panel_sample ? "panel" : "control") +
           "|" + type + "|" + role;
}

void UiDesignerThemeGallery::SyncSelectedTarget()
{
    if(!theme_)
        return;
    const String preview_target = selected_type_.IsEmpty()
        ? String() : ThemeStudioPreviewTarget(selected_type_, selected_panel_sample_);
    theme_->SetActivePreviewTarget(preview_target);
    theme_->SetActiveStyleTarget(selected_type_.IsEmpty()
        ? String()
        : CurrentStyleTarget(theme_->GetEffective(), selected_type_,
                             selected_panel_sample_));
}

void UiDesignerThemeGallery::BuildSelectedPropertyModel(
    PropertyEditorModel& model, const UiDesignerThemeSnapshot& theme) const
{
    model.Clear(false);
    if(selected_type_.IsEmpty()) {
        model.AddReadOnly("theme.studio.status", "Status",
                          "Select a Theme Studio sample to edit its theme.",
                          "Theme Studio");
        model.StructureChanged();
        return;
    }

    const UiDesignerControlSpec *spec = catalog_ ? catalog_->Find(selected_type_)
                                                 : nullptr;
    const UiDesignerThemeAdapter *adapter = spec ? UiDesignerGetThemeAdapter(*spec)
                                                 : nullptr;
    if(!spec || !adapter || !adapter->Supports(spec->runtime_kind) ||
       spec->theme_overrides.IsEmpty()) {
        model.AddReadOnly("theme.studio.status", "Status",
                          selected_type_ + " has no editable Theme adapter yet.",
                          "Theme Studio");
        model.StructureChanged();
        return;
    }

    const String control_role = ThemeRoleName(control_role_);
    const String panel_role = PanelRoleName(panel_role_);
    const String role = selected_panel_sample_
        ? ThemeRoleName(PanelRoleAsControlRole(panel_role_))
        : control_role;
    const String appearance = theme.mode == "Dark" ? "Dark" : "Light";

    model.AddReadOnly("theme.studio.identity.control", "Control",
                      spec->display_name, "Identity");
    model.AddReadOnly("theme.studio.identity.type", "Type",
                      selected_type_, "Identity");
    model.AddReadOnly("theme.studio.identity.appearance", "Appearance",
                      appearance, "Identity");
    model.AddReadOnly("theme.studio.identity.role", "Role",
                      selected_panel_sample_ ? panel_role : control_role,
                      "Identity");
    model.AddReadOnly("theme.studio.identity.scope", "Scope",
                      selected_panel_sample_ ? "Panel role recipe" : "Control role recipe",
                      "Identity");

    UiDesignerNode base;
    PopulateSampleNode(base, *spec, role);
    const String target = CurrentStyleTarget(theme, selected_type_,
                                             selected_panel_sample_);
    const ValueMap authored = theme.GetStyleOverrides(target);

    const auto add_field = [&](const UiDesignerThemeOverrideSpec& property) {
        const Value inherited = adapter->ResolveFieldValue(
            base, *spec, property.adapter_field_id, nullptr);
        const int q = authored.Find(property.id);
        const Value value = q >= 0 ? authored.GetValue(q) : inherited;
        UiDesignerThemeOverrideSpec projected = property;
        projected.group = ThemeStudioAppearanceGroup(property.group);
        projected.AddTo(model, value, false);
        PropertyEditorItem *item = model.Find(property.id);
        if(!item)
            return;
        item->id = "studio." + property.id;
        item->default_value = inherited;
        item->resettable = true;
        item->overrideable = false;
        item->override_active = q >= 0;
        item->value_editable = !item->read_only;
        item->SetInherited(q < 0);
    };

    for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides)
        if(property.group == "General")
            add_field(property);
    for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides)
        if(property.group != "General")
            add_field(property);

    for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides) {
        if(property.visible_when_id.IsEmpty())
            continue;
        PropertyEditorItem *item = model.Find("studio." + property.id);
        const PropertyEditorItem *condition =
            model.Find("studio." + property.visible_when_id);
        if(item)
            item->visible = condition &&
                            condition->value == property.visible_when_value;
    }

    const String preview_target = ThemeStudioPreviewTarget(
        selected_type_, selected_panel_sample_);
    const ValueMap preview_values = theme.GetStudioPreview(preview_target);
    for(const UiDesignerPropertySpec& source : spec->properties) {
        if(!IsThemeStudioPreviewProperty(source.id))
            continue;
        UiDesignerPropertySpec property = source;
        property.group = ThemeStudioPreviewGroup(property.id);
        property.domain = PropertyEditorDomain::DesignerOnly;
        const Value fallback = ThemeStudioPreviewDefault(property);
        const int q = preview_values.Find(property.id);
        const Value value = q >= 0 ? preview_values.GetValue(q) : fallback;
        property.AddTo(model, value, false);
        PropertyEditorItem *item = model.Find(property.id);
        if(!item)
            continue;
        item->id = "preview." + property.id;
        item->default_value = fallback;
        item->resettable = true;
        item->overrideable = false;
        item->override_active = false;
        item->value_editable = !item->read_only;
        item->SetInherited(q < 0);
        if(property.id == "icon_render_mode" || property.id == "icon_mode")
            item->SetHelp("Preview-only icon rendering. Monochrome tint uses the selected control's themed Icon Ink.");
    }

    model.SetGroupSubtitle("Identity", spec->display_name + " · " +
        (selected_panel_sample_ ? panel_role : control_role) + " · " + appearance);
    model.SetGroupSubtitle("Appearance",
                           "reusable Theme Studio recipe; reset returns this field to the selected role");
    if(model.Find("preview.icon"))
        model.SetGroupSubtitle("Preview / Content",
                               "sample-only content for judging the authored theme");
    if(model.Find("preview.icon_side") || model.Find("preview.icon_width") ||
       model.Find("preview.icon_height") || model.Find("preview.icon_size") ||
       model.Find("preview.content_gap"))
        model.SetGroupSubtitle("Preview / Layout",
                               "sample-only layout; not part of the runtime theme recipe");
    model.StructureChanged();
}

void UiDesignerThemeGallery::ApplySampleTheme(
    Ctrl& ctrl, const String& type, bool panel_sample)
{
    if(!catalog_ || !theme_)
        return;
    const UiDesignerControlSpec *spec = catalog_->Find(type);
    const UiDesignerThemeAdapter *adapter = spec ? UiDesignerGetThemeAdapter(*spec)
                                                 : nullptr;
    if(!spec || !adapter || !adapter->Supports(spec->runtime_kind) ||
       spec->theme_overrides.IsEmpty())
        return;

    const UiDesignerThemeSnapshot& effective = theme_->GetEffective();
    const String role = panel_sample
        ? ThemeRoleName(PanelRoleAsControlRole(panel_role_))
        : ThemeRoleName(control_role_);
    UiDesignerNode base;
    PopulateSampleNode(base, *spec, role);
    UiDesignerNode styled = base;
    const ValueMap authored = effective.GetStyleOverrides(
        CurrentStyleTarget(effective, type, panel_sample));

    for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides) {
        const int q = authored.Find(property.id);
        const Value value = q >= 0
            ? authored.GetValue(q)
            : adapter->ResolveFieldValue(base, *spec,
                                         property.adapter_field_id, nullptr);
        styled.theme_overrides.Set(property.id, value);
    }
    adapter->ApplyPreviewStyle(ctrl, styled, *spec, nullptr);

    const String preview_target = ThemeStudioPreviewTarget(type, panel_sample);
    for(const UiDesignerPropertySpec& property : spec->properties) {
        if(!IsThemeStudioPreviewProperty(property.id))
            continue;
        const Value value = effective.GetStudioPreviewValue(
            preview_target, property.id, ThemeStudioPreviewDefault(property));
        UiDesignerPreviewFactory::Apply(ctrl, *spec, property.id, value);
    }
}

void UiDesignerThemeGallery::ApplyThemeStyles()
{
    const UiDesignerThemeSnapshot effective = theme_
        ? theme_->GetEffective() : UiDesignerThemeSnapshot();
    UiDesignerApplyGlobalTheme(effective);

    ApplySampleTheme(controls_reference_panel_, "UiPanel", true);
    ApplySampleTheme(controls_reference_label_, "UiLabel", false);
    ApplySampleTheme(controls_reference_button_, "UiButton", false);
    ApplySampleTheme(buttons_group_, "UiGroupPanel", true);
    ApplySampleTheme(button_, "UiButton", false);
    ApplySampleTheme(tool_button_, "UiToolButton", false);
    ApplySampleTheme(split_button_, "UiSplitButton", false);
    ApplySampleTheme(choices_group_, "UiGroupPanel", true);
    ApplySampleTheme(check_, "UiCheckBox", false);
    ApplySampleTheme(radio_, "UiRadioButton", false);
    ApplySampleTheme(toggle_, "UiToggle", false);
    ApplySampleTheme(dropdown_, "UiDropdown", false);
    ApplySampleTheme(numbers_group_, "UiGroupPanel", true);
    ApplySampleTheme(int_edit_, "UiIntEdit", false);
    ApplySampleTheme(float_edit_, "UiFloatEdit", false);
    ApplySampleTheme(slider_, "UiSlider", false);
    ApplySampleTheme(progress_, "UiProgressBar", false);
    ApplySampleTheme(scroll_bar_, "UiScrollBar", false);
    ApplySampleTheme(inputs_group_, "UiGroupPanel", true);
    ApplySampleTheme(line_edit_, "UiLineEdit", false);
    ApplySampleTheme(multi_edit_, "UiMultiEdit", false);
    ApplySampleTheme(data_group_, "UiGroupPanel", true);
    ApplySampleTheme(list_, "UiList", false);
    ApplySampleTheme(tree_, "UiTree", false);
    ApplySampleTheme(navigation_group_, "UiGroupPanel", true);
    ApplySampleTheme(tab_, "UiTab", false);
    ApplySampleTheme(accordion_, "UiAccordion", false);
    ApplySampleTheme(feedback_group_, "UiGroupPanel", true);
    ApplySampleTheme(feedback_label_, "UiLabel", false);
    ApplySampleTheme(feedback_progress_, "UiProgressBar", false);

    ApplySampleTheme(container_plain_panel_, "UiPanel", true);
    ApplySampleTheme(container_plain_label_, "UiLabel", false);
    ApplySampleTheme(container_controls_panel_, "UiPanel", true);
    ApplySampleTheme(container_controls_label_, "UiLabel", false);
    ApplySampleTheme(container_button_, "UiButton", false);
    ApplySampleTheme(container_check_, "UiCheckBox", false);
    ApplySampleTheme(container_plain_group_, "UiGroupPanel", true);
    ApplySampleTheme(container_group_label_, "UiLabel", false);
    ApplySampleTheme(container_edit_group_, "UiGroupPanel", true);
    ApplySampleTheme(container_edit_, "UiLineEdit", false);
    ApplySampleTheme(container_edit_button_, "UiButton", false);
    ApplySampleTheme(container_numeric_panel_, "UiPanel", true);
    ApplySampleTheme(container_numeric_label_, "UiLabel", false);
    ApplySampleTheme(container_slider_, "UiSlider", false);
    ApplySampleTheme(container_int_, "UiIntEdit", false);
    ApplySampleTheme(container_choice_group_, "UiGroupPanel", true);
    ApplySampleTheme(container_dropdown_, "UiDropdown", false);
    ApplySampleTheme(container_toggle_, "UiToggle", false);
    ApplySampleTheme(container_scroll_panel_, "UiScrollPanel", true);
    ApplySampleTheme(container_scroll_label_, "UiLabel", false);

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
    scroll_bar_.SetRect(inset, DPI(166), max(0, w - inset * 2), DPI(18));

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

    w = container_scroll_panel_.GetSize().cx;
    container_scroll_label_.SetRect(inset, DPI(14), max(0, w - inset * 2), DPI(112));
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
