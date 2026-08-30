#include "UiDesignerThemeBuilderV2.h"
#include "UiDesignerThemeAdapter.h"
#include <UiDesigner/Preview/UiDesignerPreview.h>

namespace Upp {
namespace {

static String UniversalRoleName(UiRole role)
{
    switch(role) {
    case UiRole::Subtle: return "Subtle";
    case UiRole::Accent: return "Accent";
    case UiRole::Alert:  return "Alert";
    case UiRole::Standard:
    default:             return "Standard";
    }
}

static String StudioPreviewTargetV2(const String& type, bool panel_sample)
{
    return String(panel_sample ? "panel|" : "control|") + type;
}

static void PopulateSampleNodeV2(UiDesignerNode& node,
                                 const UiDesignerControlSpec& spec,
                                 UiRole role)
{
    node.type = spec.type_id;
    for(int i = 0; i < spec.defaults.GetCount(); ++i)
        node.SetProperty(AsString(spec.defaults.GetKey(i)),
                         spec.defaults.GetValue(i));
    node.SetProperty("role", UniversalRoleName(role));
}

static bool IsStudioPreviewPropertyV2(const String& id)
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

static String StudioPreviewGroupV2(const String& id)
{
    return id == "icon" || id == "icon_render_mode" || id == "icon_mode"
        ? "Preview / Content" : "Preview / Layout";
}

static String StudioAppearanceGroupV2(const String& group)
{
    if(group.IsEmpty() || group == "General")
        return "Appearance";
    if(group == "Appearance")
        return "Appearance / Control";
    if(group.StartsWith("Appearance / "))
        return group;
    return "Appearance / " + group;
}

static Value StudioPreviewDefaultV2(const UiDesignerPropertySpec& property)
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

static void PaintThemeSurface(Draw& w, Size size)
{
    UiPanel::Style style = UiTheme::ResolvePanel(UiRole::Standard);
    style.metrics.shadow.enabled = false;
    UiPaintFaceFrameDash(w, Rect(Point(0, 0), size),
                         style.palette, style.metrics, ST_NORMAL);
}

}

UiDesignerThemeGalleryV2::UiDesignerThemeGalleryV2()
{
    // Feedback duplicated the progress sample already shown in Numeric &
    // Sliders. Reuse that existing group shell for a full-width Table sample,
    // leaving DATA to present List and Tree at readable widths.
    table_.Remove();
    feedback_label_.Remove();
    feedback_progress_.Remove();
    data_group_.SetSubTitle("List and tree");
    feedback_group_.SetTitle("TABLE")
                   .SetSubTitle("Tabular selection and scrolling");
    feedback_group_.Add(table_);

    RebuildColumnPlacement();
    RebindPanelSamples();
    ApplyThemeStylesV2();
}

void UiDesignerThemeGalleryV2::RebuildColumnPlacement()
{
    for(int i = 0; i < 3; ++i)
        control_columns_[i].ClearItems();

    control_columns_[0].Add(controls_reference_panel_).Fixed(DPI(108));
    control_columns_[0].Add(buttons_group_).Fixed(DPI(164));
    control_columns_[0].Add(data_group_).Fixed(DPI(252));

    control_columns_[1].Add(numbers_group_).Fixed(DPI(212));
    control_columns_[1].Add(inputs_group_).Fixed(DPI(226));
    control_columns_[1].Add(choices_group_).Fixed(DPI(158));

    // Navigation now starts the third column; the former Feedback group is the
    // dedicated Table sample below it.
    control_columns_[2].Add(navigation_group_).Fixed(DPI(292));
    control_columns_[2].Add(feedback_group_).Fixed(DPI(252));
}

void UiDesignerThemeGalleryV2::RebindPanelSamples()
{
    const auto bind = [=](UiDesignerThemeSelectableBase& sample,
                          const char *type) {
        UiDesignerThemeSelectableBase *ptr = &sample;
        const String type_id = type;
        sample.WhenThemeSelect = [=] { SelectPanelSample(type_id, ptr); };
    };

    bind(controls_reference_panel_, "UiPanel");
    bind(buttons_group_, "UiGroupPanel");
    bind(choices_group_, "UiGroupPanel");
    bind(numbers_group_, "UiGroupPanel");
    bind(inputs_group_, "UiGroupPanel");
    bind(data_group_, "UiGroupPanel");
    bind(navigation_group_, "UiGroupPanel");
    bind(feedback_group_, "UiGroupPanel");
    bind(container_plain_panel_, "UiPanel");
    bind(container_controls_panel_, "UiPanel");
    bind(container_plain_group_, "UiGroupPanel");
    bind(container_edit_group_, "UiGroupPanel");
    bind(container_numeric_panel_, "UiPanel");
    bind(container_choice_group_, "UiGroupPanel");
    bind(container_scroll_panel_, "UiScrollPanel");
}

void UiDesignerThemeGalleryV2::SetThemeDocument(
    UiDesignerThemeDocument *theme)
{
    UiDesignerThemeGallery::SetThemeDocument(theme);
    if(theme_) {
        theme_->SetPropertyModelProvider(
            [=](PropertyEditorModel& model,
                const UiDesignerThemeSnapshot& value) {
                BuildSelectedPropertyModelV2(model, value);
            });
    }
    SyncSelectedTargetV2();
    ApplyThemeStylesV2();
}

void UiDesignerThemeGalleryV2::SetPanelRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    if(panel_role_v2_ == role)
        return;
    panel_role_v2_ = role;
    if(selected_panel_sample_)
        SyncSelectedTargetV2();
    ApplyThemeStylesV2();
}

void UiDesignerThemeGalleryV2::SetControlRole(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    if(control_role_ == role)
        return;
    control_role_ = role;
    if(!selected_panel_sample_)
        SyncSelectedTargetV2();
    ApplyThemeStylesV2();
}

void UiDesignerThemeGalleryV2::RefreshTheme()
{
    SyncSelectedTargetV2();
    ApplyThemeStylesV2();
}

void UiDesignerThemeGalleryV2::SelectPanelSample(
    const String& type, UiDesignerThemeSelectableBase *sample)
{
    if(selected_sample_ && selected_sample_ != sample)
        selected_sample_->SetThemeSelected(false);
    selected_sample_ = sample;
    selected_type_ = type;
    selected_panel_sample_ = true;
    if(selected_sample_)
        selected_sample_->SetThemeSelected(true);
    SyncSelectedTargetV2();
    Refresh();
}

String UiDesignerThemeGalleryV2::CurrentStyleTargetV2(
    const UiDesignerThemeSnapshot& theme, const String& type,
    bool panel_sample) const
{
    const String appearance = theme.mode == "Dark" ? "Dark" : "Light";
    const UiRole role = panel_sample ? panel_role_v2_ : control_role_;
    return appearance + "|" + (panel_sample ? "panel" : "control") +
           "|" + type + "|" + UniversalRoleName(role);
}

void UiDesignerThemeGalleryV2::SyncSelectedTargetV2()
{
    if(!theme_)
        return;
    const String preview_target = selected_type_.IsEmpty()
        ? String() : StudioPreviewTargetV2(selected_type_, selected_panel_sample_);
    theme_->SetActivePreviewTarget(preview_target);
    theme_->SetActiveStyleTarget(selected_type_.IsEmpty()
        ? String()
        : CurrentStyleTargetV2(theme_->GetEffective(), selected_type_,
                               selected_panel_sample_));
}

void UiDesignerThemeGalleryV2::BuildSelectedPropertyModelV2(
    PropertyEditorModel& model, const UiDesignerThemeSnapshot& theme) const
{
    if(!selected_panel_sample_) {
        BuildSelectedPropertyModel(model, theme);
        return;
    }

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

    const String role_name = UniversalRoleName(panel_role_v2_);
    const String appearance = theme.mode == "Dark" ? "Dark" : "Light";
    model.AddReadOnly("theme.studio.identity.control", "Control",
                      spec->display_name, "Identity");
    model.AddReadOnly("theme.studio.identity.type", "Type",
                      selected_type_, "Identity");
    model.AddReadOnly("theme.studio.identity.appearance", "Appearance",
                      appearance, "Identity");
    model.AddReadOnly("theme.studio.identity.role", "Panel role",
                      role_name, "Identity");
    model.AddReadOnly("theme.studio.identity.scope", "Scope",
                      "Panel role recipe — independent from Control Role",
                      "Identity");

    UiDesignerNode base;
    PopulateSampleNodeV2(base, *spec, panel_role_v2_);
    const String target = CurrentStyleTargetV2(theme, selected_type_, true);
    const ValueMap authored = theme.GetStyleOverrides(target);

    const auto add_field = [&](const UiDesignerThemeOverrideSpec& property) {
        const Value inherited = adapter->ResolveFieldValue(
            base, *spec, property.adapter_field_id, nullptr);
        const int q = authored.Find(property.id);
        const Value value = q >= 0 ? authored.GetValue(q) : inherited;
        UiDesignerThemeOverrideSpec projected = property;
        projected.group = StudioAppearanceGroupV2(property.group);
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

    const String preview_target = StudioPreviewTargetV2(selected_type_, true);
    const ValueMap preview_values = theme.GetStudioPreview(preview_target);
    for(const UiDesignerPropertySpec& source : spec->properties) {
        if(!IsStudioPreviewPropertyV2(source.id))
            continue;
        UiDesignerPropertySpec property = source;
        property.group = StudioPreviewGroupV2(property.id);
        property.domain = PropertyEditorDomain::DesignerOnly;
        const Value fallback = StudioPreviewDefaultV2(property);
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
    }

    model.SetGroupSubtitle("Identity", spec->display_name + " · Panel " +
                           role_name + " · " + appearance);
    model.SetGroupSubtitle("Appearance",
        "panel recipe only; the Control Role selection remains independent");
    if(model.Find("preview.icon"))
        model.SetGroupSubtitle("Preview / Content",
                               "sample-only content for judging the panel recipe");
    if(model.Find("preview.icon_side") || model.Find("preview.icon_width") ||
       model.Find("preview.icon_height") || model.Find("preview.icon_size") ||
       model.Find("preview.content_gap"))
        model.SetGroupSubtitle("Preview / Layout",
                               "sample-only layout; not part of the runtime theme recipe");
    model.StructureChanged();
}

void UiDesignerThemeGalleryV2::ApplySampleThemeV2(
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
    const UiRole role = panel_sample ? panel_role_v2_ : control_role_;
    UiDesignerNode base;
    PopulateSampleNodeV2(base, *spec, role);
    UiDesignerNode styled = base;
    const ValueMap authored = effective.GetStyleOverrides(
        CurrentStyleTargetV2(effective, type, panel_sample));

    // Inherited fields must stay inherited. Materializing every resolver value
    // as a local override freezes the sample to the palette that happened to be
    // active when it was built and suppresses later Light/Dark resolver changes.
    for(const UiDesignerThemeOverrideSpec& property : spec->theme_overrides) {
        const int q = authored.Find(property.id);
        if(q >= 0)
            styled.theme_overrides.Set(property.id, authored.GetValue(q));
    }
    adapter->ApplyPreviewStyle(ctrl, styled, *spec, nullptr);

    const String preview_target = StudioPreviewTargetV2(type, panel_sample);
    for(const UiDesignerPropertySpec& property : spec->properties) {
        if(!IsStudioPreviewPropertyV2(property.id))
            continue;
        const Value value = effective.GetStudioPreviewValue(
            preview_target, property.id, StudioPreviewDefaultV2(property));
        UiDesignerPreviewFactory::Apply(ctrl, *spec, property.id, value);
    }
}

void UiDesignerThemeGalleryV2::ApplyThemeStylesV2()
{
    const UiDesignerThemeSnapshot effective = theme_
        ? theme_->GetEffective() : UiDesignerThemeSnapshot();
    UiDesignerApplyGlobalTheme(effective);

    ApplySampleThemeV2(controls_reference_panel_, "UiPanel", true);
    ApplySampleThemeV2(controls_reference_label_, "UiLabel", false);
    ApplySampleThemeV2(controls_reference_button_, "UiButton", false);
    ApplySampleThemeV2(buttons_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(button_, "UiButton", false);
    ApplySampleThemeV2(tool_button_, "UiToolButton", false);
    ApplySampleThemeV2(split_button_, "UiSplitButton", false);
    ApplySampleThemeV2(choices_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(check_, "UiCheckBox", false);
    ApplySampleThemeV2(radio_, "UiRadioButton", false);
    ApplySampleThemeV2(toggle_, "UiToggle", false);
    ApplySampleThemeV2(dropdown_, "UiDropdown", false);
    ApplySampleThemeV2(numbers_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(int_edit_, "UiIntEdit", false);
    ApplySampleThemeV2(float_edit_, "UiFloatEdit", false);
    ApplySampleThemeV2(slider_, "UiSlider", false);
    ApplySampleThemeV2(progress_, "UiProgressBar", false);
    ApplySampleThemeV2(scroll_bar_, "UiScrollBar", false);
    ApplySampleThemeV2(inputs_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(line_edit_, "UiLineEdit", false);
    ApplySampleThemeV2(multi_edit_, "UiMultiEdit", false);
    ApplySampleThemeV2(data_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(list_, "UiList", false);
    ApplySampleThemeV2(tree_, "UiTree", false);
    ApplySampleThemeV2(navigation_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(tab_, "UiTab", false);
    ApplySampleThemeV2(accordion_, "UiAccordion", false);
    ApplySampleThemeV2(feedback_group_, "UiGroupPanel", true);

    ApplySampleThemeV2(container_plain_panel_, "UiPanel", true);
    ApplySampleThemeV2(container_plain_label_, "UiLabel", false);
    ApplySampleThemeV2(container_controls_panel_, "UiPanel", true);
    ApplySampleThemeV2(container_controls_label_, "UiLabel", false);
    ApplySampleThemeV2(container_button_, "UiButton", false);
    ApplySampleThemeV2(container_check_, "UiCheckBox", false);
    ApplySampleThemeV2(container_plain_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(container_group_label_, "UiLabel", false);
    ApplySampleThemeV2(container_edit_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(container_edit_, "UiLineEdit", false);
    ApplySampleThemeV2(container_edit_button_, "UiButton", false);
    ApplySampleThemeV2(container_numeric_panel_, "UiPanel", true);
    ApplySampleThemeV2(container_numeric_label_, "UiLabel", false);
    ApplySampleThemeV2(container_slider_, "UiSlider", false);
    ApplySampleThemeV2(container_int_, "UiIntEdit", false);
    ApplySampleThemeV2(container_choice_group_, "UiGroupPanel", true);
    ApplySampleThemeV2(container_dropdown_, "UiDropdown", false);
    ApplySampleThemeV2(container_toggle_, "UiToggle", false);
    ApplySampleThemeV2(container_scroll_panel_, "UiScrollPanel", true);
    ApplySampleThemeV2(container_scroll_label_, "UiLabel", false);

    // The table has no Theme Studio catalog recipe of its own yet, so it must
    // remain on the reusable UiTheme path rather than retaining a stale
    // custom/default palette across Light/Dark switches.
    table_.ClearCustomStyle();

    Layout();
    Refresh();
}

void UiDesignerThemeGalleryV2::Layout()
{
    UiDesignerThemeGallery::Layout();
    const int inset = DPI(12);

    int w = buttons_group_.GetSize().cx;
    const int split_x = inset + DPI(150);
    split_button_.SetRect(split_x, DPI(48),
                          max(0, w - split_x - inset), DPI(32));

    // DATA now gives List and Tree half the available width each.
    w = data_group_.GetSize().cx;
    const int data_gap = DPI(8);
    const int data_w = max(DPI(80), (w - inset * 2 - data_gap) / 2);
    list_.SetRect(inset, DPI(48), data_w, DPI(178));
    tree_.SetRect(inset + data_w + data_gap, DPI(48), data_w, DPI(178));

    // TABLE uses the former Feedback shell as an independent full-width sample.
    w = feedback_group_.GetSize().cx;
    table_.SetRect(inset, DPI(48), max(0, w - inset * 2),
                   max(DPI(120), feedback_group_.GetSize().cy - DPI(62)));
}

void UiDesignerThemeGalleryV2::Paint(Draw& w)
{
    PaintThemeSurface(w, GetSize());
}

UiDesignerThemeToolbarV2::UiDesignerThemeToolbarV2()
{
    // Theme mode is owned by the single application-level toggle in the
    // window header. Keep the palette row focused on role selection and
    // swatches instead of exposing a second, duplicate mode control.
    appearance_.Hide();
    panel_role_v2_ = UiRole::Standard;
    control_role_v2_ = UiRole::Accent;
    control_role_ = control_role_v2_;

    panel_role_label_.SetText("Panel Role")
                     .Tip("Themes container surfaces only; independent from Control Role");
    control_role_label_.SetText("Control Role")
                       .Tip("Themes controls only; independent from Panel Role");

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
    panel_role_drop_.Select((int)panel_role_v2_);
    control_role_drop_.Select((int)control_role_v2_);
    panel_role_drop_.Tip("Panel Role: Standard / Subtle / Accent / Alert — does not change Control Role");
    control_role_drop_.Tip("Control Role: Standard / Subtle / Accent / Alert — does not change Panel Role");

    panel_role_drop_.WhenAction = [=] {
        if(!syncing_)
            SetPanelRoleV2((UiRole)(int)panel_role_drop_.GetData());
    };
    control_role_drop_.WhenAction = [=] {
        if(!syncing_)
            SetControlRoleV2((UiRole)(int)control_role_drop_.GetData());
    };
}

void UiDesignerThemeToolbarV2::SetGallery(UiDesignerThemeGalleryV2 *gallery)
{
    gallery_v2_ = gallery;
    gallery_ = gallery;
    if(gallery_v2_) {
        gallery_v2_->SetPreviewMode(preview_mode_);
        gallery_v2_->SetPanelRole(panel_role_v2_);
        gallery_v2_->SetControlRole(control_role_v2_);
    }
}

void UiDesignerThemeToolbarV2::SetPanelRoleV2(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    panel_role_v2_ = role;
    syncing_ = true;
    panel_role_drop_.Select((int)role);
    syncing_ = false;
    if(gallery_v2_)
        gallery_v2_->SetPanelRole(role);
    WhenStatus("Panel Role: " + UniversalRoleName(role) +
               " (Control Role unchanged)");
}

void UiDesignerThemeToolbarV2::SetControlRoleV2(UiRole role)
{
    if(!UiIsValid(role))
        role = UiRole::Standard;
    control_role_v2_ = role;
    control_role_ = role;
    syncing_ = true;
    control_role_drop_.Select((int)role);
    syncing_ = false;
    if(gallery_v2_)
        gallery_v2_->SetControlRole(role);
    WhenStatus("Control Role: " + UniversalRoleName(role) +
               " (Panel Role unchanged)");
}

void UiDesignerThemeToolbarV2::Layout()
{
    const int h = GetSize().cy;
    const int gap = DPI(6);
    const int small_gap = DPI(3);
    const int control_h = min(DPI(30), max(0, h - DPI(8)));
    const int y = max(0, (h - control_h) / 2);
    int x = DPI(4);

    controls_mode_.SetRect(x, y, DPI(30), control_h); x += DPI(30) + small_gap;
    containers_mode_.SetRect(x, y, DPI(30), control_h); x += DPI(30) + gap;

    panel_role_label_.SetRect(x, y, DPI(68), control_h); x += DPI(68) + DPI(4);
    panel_role_drop_.SetRect(x, y, DPI(108), control_h); x += DPI(108) + gap;

    control_role_label_.SetRect(x, y, DPI(76), control_h); x += DPI(76) + DPI(4);
    control_role_drop_.SetRect(x, y, DPI(108), control_h); x += DPI(108) + gap;

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
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; ++i)
        light_[i].SetRect(sx + i * (swatch + small_gap), swatch_y,
                          swatch, swatch);

    x += group_w + palette_gap;
    dark_label_.SetRect(x, y, label_w, control_h);
    sx = x + label_w + label_gap;
    for(int i = 0; i < UI_DESIGNER_THEME_PALETTE_SIZE; ++i)
        dark_[i].SetRect(sx + i * (swatch + small_gap), swatch_y,
                         swatch, swatch);
}

void UiDesignerThemeToolbarV2::Paint(Draw& w)
{
    PaintThemeSurface(w, GetSize());
    const UiLabel::Style label = UiTheme::ResolveLabel(UiRole::Subtle);
    w.DrawLine(0, GetSize().cy - 1, GetSize().cx, GetSize().cy - 1,
               DPI(1), label.palette.ink[ST_DISABLED]);
}

}
