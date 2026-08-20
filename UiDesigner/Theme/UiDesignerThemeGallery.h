#ifndef _Utilities_UiDesigner_Theme_UiDesignerThemeGallery_h_
#define _Utilities_UiDesigner_Theme_UiDesignerThemeGallery_h_

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>
#include <Ui/UiColorPicker/UiColorPicker.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/ThemeCore/UiDesignerTheme.h>

namespace Upp {

UiThemeContext UiDesignerResolveThemeContext(const UiDesignerThemeSnapshot& theme);
void UiDesignerApplyGlobalTheme(const UiDesignerThemeSnapshot& theme);

class UiDesignerThemeSwatch : public Ctrl {
public:
    typedef UiDesignerThemeSwatch CLASSNAME;

    UiDesignerThemeSwatch();

    UiDesignerThemeSwatch& SetColor(Color color);
    Color GetColor() const { return color_; }
    UiDesignerThemeSwatch& SetActive(bool active = true);

    virtual Size GetMinSize() const override { return Size(DPI(18), DPI(18)); }
    virtual void Paint(Draw& w) override;
    virtual void LeftDown(Point p, dword keyflags) override;
    virtual void LeftUp(Point p, dword keyflags) override;
    virtual void LeftDrag(Point p, dword keyflags) override;
    virtual bool Key(dword key, int count) override;

    Event<> WhenAction;

private:
    String HexText() const;

    Color color_ = Color(128, 128, 128);
    bool active_ = false;
    bool dragging_ = false;
};

class UiDesignerThemeGallery;

class UiDesignerThemeToolbar : public ParentCtrl {
public:
    typedef UiDesignerThemeToolbar CLASSNAME;

    UiDesignerThemeToolbar();

    void SetThemeDocument(UiDesignerThemeDocument *theme);
    void SetGallery(UiDesignerThemeGallery *gallery);
    void ApplyTheme(const UiDesignerThemeSnapshot& theme);
    void SyncFromTheme();

    virtual Size GetMinSize() const override { return Size(DPI(760), DPI(50)); }
    virtual void Layout() override;
    virtual void Paint(Draw& w) override;

    Event<String> WhenStatus;

private:
    void SetPreviewMode(const String& mode);
    void SetPanelRole(UiRole role);
    void SetControlRole(UiRole role);
    void ToggleAppearance();
    void EditPalette(bool dark, int active_slot);
    void UpdateModeButtons();

    UiDesignerThemeDocument *theme_ = nullptr;
    UiDesignerThemeGallery *gallery_ = nullptr;
    bool syncing_ = false;
    String preview_mode_ = "controls";
    UiRole panel_role_ = UiRole::Accent;
    UiRole control_role_ = UiRole::Accent;

    UiToolButton controls_mode_;
    UiToolButton containers_mode_;
    UiLabel panel_role_label_;
    UiDropdown panel_role_drop_;
    UiLabel control_role_label_;
    UiDropdown control_role_drop_;
    UiToolButton appearance_;
    UiLabel light_label_;
    UiLabel dark_label_;
    UiDesignerThemeSwatch light_[UI_DESIGNER_THEME_PALETTE_SIZE];
    UiDesignerThemeSwatch dark_[UI_DESIGNER_THEME_PALETTE_SIZE];
};

class UiDesignerThemeGallery : public ParentCtrl {
public:
    typedef UiDesignerThemeGallery CLASSNAME;

    UiDesignerThemeGallery();

    void SetCatalog(const UiDesignerCatalog *catalog);
    void SetThemeDocument(UiDesignerThemeDocument *theme);
    void SetPreviewMode(const String& mode);
    void SetPanelRole(UiRole role);
    void SetControlRole(UiRole role);
    void RefreshTheme();
    int GetContentHeight() const { return content_height_; }

    // Compatibility with the previous gallery filter API. Theme Builder now
    // has only the two purposeful preview modes.
    void SetFilter(const String& filter)
    {
        SetPreviewMode(ToLower(filter) == "containers" ? "containers" : "controls");
    }

    virtual void Layout() override;
    virtual void Paint(Draw& w) override;

private:
    void BuildPreviewMatrices();
    void BuildControlSamples();
    void BuildContainerSamples();
    void ApplyThemeStyles();
    void LayoutControlSamples();
    void LayoutContainerSamples();

    const UiDesignerCatalog *catalog_ = nullptr;
    UiDesignerThemeDocument *theme_ = nullptr;
    String preview_mode_ = "controls";
    UiRole panel_role_ = UiRole::Accent;
    UiRole control_role_ = UiRole::Accent;
    int content_height_ = DPI(730);

    UiStack preview_stack_;
    UiGridLayout controls_matrix_;
    UiGridLayout containers_matrix_;
    UiBoxLayout control_columns_[3];
    UiBoxLayout container_columns_[3];

    // Controls view: representative families hosted on independently themed
    // panels/group panels.
    UiPanel controls_reference_panel_;
    UiLabel controls_reference_label_;
    UiButton controls_reference_button_;

    UiGroupPanel buttons_group_;
    UiButton button_;
    UiToolButton tool_button_;
    UiSplitButton split_button_;
    UiBreadcrumbs breadcrumbs_;

    UiGroupPanel choices_group_;
    UiCheckBox check_;
    UiRadioButton radio_;
    UiToggle toggle_;
    UiDropdown dropdown_;

    UiGroupPanel numbers_group_;
    UiIntEdit int_edit_;
    UiFloatEdit float_edit_;
    UiSlider slider_;
    UiProgressBar progress_;

    UiGroupPanel inputs_group_;
    UiLineEdit line_edit_;
    UiMultiEdit multi_edit_;
    UiSliderEdit slider_edit_;

    UiGroupPanel data_group_;
    UiList list_;
    UiTree tree_;
    UiTable table_;

    UiGroupPanel navigation_group_;
    UiTab tab_;
    ParentCtrl tab_page_a_;
    ParentCtrl tab_page_b_;
    UiAccordion accordion_;
    ParentCtrl accordion_a_;
    ParentCtrl accordion_b_;

    UiGroupPanel feedback_group_;
    UiLabel feedback_label_;
    UiProgressBar feedback_progress_;

    // Containers view: paired plain/populated surfaces make it possible to
    // judge container chrome both alone and with representative content.
    UiPanel container_plain_panel_;
    UiLabel container_plain_label_;

    UiPanel container_controls_panel_;
    UiLabel container_controls_label_;
    UiButton container_button_;
    UiCheckBox container_check_;

    UiGroupPanel container_plain_group_;
    UiLabel container_group_label_;

    UiGroupPanel container_edit_group_;
    UiLineEdit container_edit_;
    UiButton container_edit_button_;

    UiPanel container_numeric_panel_;
    UiLabel container_numeric_label_;
    UiSlider container_slider_;
    UiIntEdit container_int_;

    UiGroupPanel container_choice_group_;
    UiDropdown container_dropdown_;
    UiToggle container_toggle_;
};

}

#endif
