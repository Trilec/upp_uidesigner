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

class UiDesignerThemeSelectableBase {
public:
    virtual ~UiDesignerThemeSelectableBase() {}
    virtual void SetThemeSelected(bool selected) = 0;
    Event<> WhenThemeSelect;
};

template <class T>
class UiDesignerThemeSelectable : public T, public UiDesignerThemeSelectableBase {
public:
    UiDesignerThemeSelectable()
    {
        this->WantFocus();
    }

    void SetThemeSelected(bool selected) override
    {
        if(selected_ == selected)
            return;
        selected_ = selected;
        this->Refresh();
    }

    virtual void LeftDown(Point p, dword keyflags) override
    {
        WhenThemeSelect();
        T::LeftDown(p, keyflags);
    }

    virtual void Paint(Draw& w) override
    {
        T::Paint(w);
        if(selected_)
            DrawFatFrame(w, Rect(Point(0, 0), this->GetSize()),
                         SColorHighlight(), DPI(2));
    }

private:
    bool selected_ = false;
};

class UiDesignerThemeGallery;

class UiDesignerThemeToolbar : public ParentCtrl {
public:
    typedef UiDesignerThemeToolbar CLASSNAME;

    UiDesignerThemeToolbar();
    UiDesignerThemeToolbar(UiDesignerThemeDocument& theme,
                           UiDesignerThemeGallery& gallery)
        : UiDesignerThemeToolbar()
    {
        SetThemeDocument(&theme);
        SetGallery(&gallery);
    }

    void SetThemeDocument(UiDesignerThemeDocument *theme);
    void SetGallery(UiDesignerThemeGallery *gallery);
    void ApplyTheme(const UiDesignerThemeSnapshot& theme);
    void SyncFromTheme();

    // Compatibility while UiDesignerWindow still treats the old toolbar slot
    // like a pill bar. The Theme Builder owns its toolbar children itself.
    UiDesignerThemeToolbar& SetInset(int) { return *this; }
    UiDesignerThemeToolbar& AddControl(Ctrl&, int) { return *this; }

    String GetPreviewMode() const { return preview_mode_; }
    UiPanelRole GetPanelRole() const { return panel_role_; }
    UiRole GetControlRole() const { return control_role_; }

    virtual Size GetMinSize() const override { return Size(DPI(760), DPI(50)); }
    virtual void Layout() override;
    virtual void Paint(Draw& w) override;

    Event<String> WhenStatus;

private:
    void SetPreviewMode(const String& mode);
    void SetPanelRole(UiPanelRole role);
    void SetControlRole(UiRole role);
    void ToggleAppearance();
    void EditPalette(bool dark, int active_slot);
    void UpdateModeButtons();

    UiDesignerThemeDocument *theme_ = nullptr;
    UiDesignerThemeGallery *gallery_ = nullptr;
    bool syncing_ = false;
    String preview_mode_ = "controls";
    UiPanelRole panel_role_ = UiPanelRole::Surface;
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
    virtual ~UiDesignerThemeGallery();

    void SetCatalog(const UiDesignerCatalog *catalog);
    void SetThemeDocument(UiDesignerThemeDocument *theme);
    void SetPreviewMode(const String& mode);
    void SetPanelRole(UiPanelRole role);
    void SetControlRole(UiRole role);
    void RefreshTheme();
    int GetContentHeight() const { return content_height_; }
    String GetPreviewMode() const { return preview_mode_; }
    UiPanelRole GetPanelRole() const { return panel_role_; }
    UiRole GetControlRole() const { return control_role_; }
    String GetSelectedType() const { return selected_type_; }

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
    void BindSelectableSamples();
    void ApplyThemeStyles();
    void ApplySampleTheme(Ctrl& ctrl, const String& type, bool panel_sample);
    void LayoutControlSamples();
    void LayoutContainerSamples();
    void SelectSample(const String& type, UiDesignerThemeSelectableBase *sample,
                      bool panel_sample);
    void SyncSelectedTarget();
    String CurrentStyleTarget(const UiDesignerThemeSnapshot& theme,
                              const String& type, bool panel_sample) const;
    void BuildSelectedPropertyModel(PropertyEditorModel& model,
                                    const UiDesignerThemeSnapshot& theme) const;

    const UiDesignerCatalog *catalog_ = nullptr;
    UiDesignerThemeDocument *theme_ = nullptr;
    String preview_mode_ = "controls";
    UiPanelRole panel_role_ = UiPanelRole::Surface;
    UiRole control_role_ = UiRole::Accent;
    int content_height_ = DPI(730);

    String selected_type_;
    bool selected_panel_sample_ = false;
    UiDesignerThemeSelectableBase *selected_sample_ = nullptr;

    UiStack preview_stack_;
    UiGridLayout controls_matrix_;
    UiGridLayout containers_matrix_;
    UiBoxLayout control_columns_[3];
    UiBoxLayout container_columns_[3];

    // Controls view: representative families hosted on independently themed
    // panels/group panels. Only controls backed by a real Theme adapter are
    // selectable; passive reference controls remain visual context.
    UiDesignerThemeSelectable<UiPanel> controls_reference_panel_;
    UiDesignerThemeSelectable<UiLabel> controls_reference_label_;
    UiDesignerThemeSelectable<UiButton> controls_reference_button_;

    UiDesignerThemeSelectable<UiGroupPanel> buttons_group_;
    UiDesignerThemeSelectable<UiButton> button_;
    UiDesignerThemeSelectable<UiToolButton> tool_button_;
    UiDesignerThemeSelectable<UiSplitButton> split_button_;
    UiBreadcrumbs breadcrumbs_;

    UiDesignerThemeSelectable<UiGroupPanel> choices_group_;
    UiDesignerThemeSelectable<UiCheckBox> check_;
    UiDesignerThemeSelectable<UiRadioButton> radio_;
    UiDesignerThemeSelectable<UiToggle> toggle_;
    UiDesignerThemeSelectable<UiDropdown> dropdown_;

    UiDesignerThemeSelectable<UiGroupPanel> numbers_group_;
    UiDesignerThemeSelectable<UiIntEdit> int_edit_;
    UiDesignerThemeSelectable<UiFloatEdit> float_edit_;
    UiDesignerThemeSelectable<UiSlider> slider_;
    UiDesignerThemeSelectable<UiProgressBar> progress_;

    UiDesignerThemeSelectable<UiGroupPanel> inputs_group_;
    UiDesignerThemeSelectable<UiLineEdit> line_edit_;
    UiDesignerThemeSelectable<UiMultiEdit> multi_edit_;
    UiSliderEdit slider_edit_;

    UiDesignerThemeSelectable<UiGroupPanel> data_group_;
    UiDesignerThemeSelectable<UiList> list_;
    UiDesignerThemeSelectable<UiTree> tree_;
    UiTable table_;

    UiDesignerThemeSelectable<UiGroupPanel> navigation_group_;
    UiDesignerThemeSelectable<UiTab> tab_;
    ParentCtrl tab_page_a_;
    ParentCtrl tab_page_b_;
    UiDesignerThemeSelectable<UiAccordion> accordion_;
    ParentCtrl accordion_a_;
    ParentCtrl accordion_b_;

    UiDesignerThemeSelectable<UiGroupPanel> feedback_group_;
    UiDesignerThemeSelectable<UiLabel> feedback_label_;
    UiDesignerThemeSelectable<UiProgressBar> feedback_progress_;

    // Containers view: paired plain/populated surfaces make it possible to
    // judge container chrome both alone and with representative content.
    UiDesignerThemeSelectable<UiPanel> container_plain_panel_;
    UiDesignerThemeSelectable<UiLabel> container_plain_label_;

    UiDesignerThemeSelectable<UiPanel> container_controls_panel_;
    UiDesignerThemeSelectable<UiLabel> container_controls_label_;
    UiDesignerThemeSelectable<UiButton> container_button_;
    UiDesignerThemeSelectable<UiCheckBox> container_check_;

    UiDesignerThemeSelectable<UiGroupPanel> container_plain_group_;
    UiDesignerThemeSelectable<UiLabel> container_group_label_;

    UiDesignerThemeSelectable<UiGroupPanel> container_edit_group_;
    UiDesignerThemeSelectable<UiLineEdit> container_edit_;
    UiDesignerThemeSelectable<UiButton> container_edit_button_;

    UiDesignerThemeSelectable<UiPanel> container_numeric_panel_;
    UiDesignerThemeSelectable<UiLabel> container_numeric_label_;
    UiDesignerThemeSelectable<UiSlider> container_slider_;
    UiDesignerThemeSelectable<UiIntEdit> container_int_;

    UiDesignerThemeSelectable<UiGroupPanel> container_choice_group_;
    UiDesignerThemeSelectable<UiDropdown> container_dropdown_;
    UiDesignerThemeSelectable<UiToggle> container_toggle_;
};

}

#endif
