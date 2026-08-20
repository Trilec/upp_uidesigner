#ifndef _Utilities_UiDesigner_Theme_UiDesignerThemeGallery_h_
#define _Utilities_UiDesigner_Theme_UiDesignerThemeGallery_h_

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>
#include <Ui/UiColorPicker/UiColorPicker.h>
#include <UiDesigner/Catalog/UiDesignerCatalog.h>
#include <UiDesigner/Preview/UiDesignerPreview.h>
#include <UiDesigner/ThemeCore/UiDesignerTheme.h>

namespace Upp {

UiThemeContext UiDesignerResolveThemeContext(const UiDesignerThemeSnapshot& theme);
void UiDesignerApplyGlobalTheme(const UiDesignerThemeSnapshot& theme);

class UiDesignerThemeGallery : public ParentCtrl {
public:
    typedef UiDesignerThemeGallery CLASSNAME;

    UiDesignerThemeGallery();

    void SetCatalog(const UiDesignerCatalog *catalog);
    void SetThemeDocument(UiDesignerThemeDocument *theme);
    void SetFilter(const String& filter);
    void RebuildInventory();
    int GetContentHeight() const { return content_height_; }

    virtual void Layout() override;
    virtual void Paint(Draw& w) override;

private:
    void BuildAuthoredControls();
    void ApplyThemeStyles();
    void SyncPaletteEditor();
    void CommitPaletteSlot(int slot);
    void Put(Ctrl& ctrl, int x, int y, int cx, int cy);

    const UiDesignerCatalog *catalog_ = nullptr;
    UiDesignerThemeDocument *theme_ = nullptr;
    String filter_ = "all";
    int content_height_ = 1200;
    bool palette_syncing_ = false;
    bool edit_dark_palette_ = false;

    UiTitleCard title_card_;

    UiGroupPanel palette_group_;
    UiDropdown palette_mode_;
    UiLabel palette_help_;
    UiColorPicker palette_editor_;

    UiGroupPanel control_roles_group_;
    UiButton role_standard_;
    UiButton role_subtle_;
    UiButton role_accent_;
    UiButton role_alert_;

    UiGroupPanel panel_roles_group_;
    UiPanel panel_surface_;
    UiPanel panel_subtle_;
    UiPanel panel_strong_;
    UiLabel panel_surface_label_;
    UiLabel panel_subtle_label_;
    UiLabel panel_strong_label_;

    UiGroupPanel toggle_group_;
    UiLabel toggle_label_a_;
    UiLabel toggle_label_b_;
    UiToggle toggle_a_;
    UiToggle toggle_b_;

    UiGroupPanel buttons_group_;
    UiButton button_;
    UiToolButton tool_button_;
    UiSplitButton split_button_;
    UiBreadcrumbs breadcrumbs_;

    UiGroupPanel numbers_group_;
    UiIntEdit int_edit_;
    UiFloatEdit float_edit_;
    UiSlider slider_;
    UiProgressBar progress_;

    UiGroupPanel security_group_;
    UiMaskEdit mask_edit_;
    UiPasswordEdit password_edit_;

    UiGroupPanel document_group_;
    UiDoc document_;

    UiGroupPanel data_group_;
    UiTab tab_;
    ParentCtrl tab_page_a_;
    ParentCtrl tab_page_b_;
    UiTree tree_;
    UiTable table_;
    UiList list_;
    UiAccordion accordion_;
    ParentCtrl accordion_a_;
    ParentCtrl accordion_b_;

    UiGroupPanel input_group_;
    UiLineEdit line_edit_;
    UiMultiEdit multi_edit_;
    UiDropdown dropdown_;
    UiCheckBox check_;
    UiRadioButton radio_;
    UiColorPicker color_;
    UiSliderEdit slider_edit_;
    UiBezierCurveField curve_;

    Array<UiGroupPanel> inventory_tiles_;
    Array<UiLabel> inventory_titles_;
    Array<Ctrl> inventory_controls_;
};

}

#endif
