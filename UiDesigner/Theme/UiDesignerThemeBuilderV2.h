#ifndef _Utilities_UiDesigner_Theme_UiDesignerThemeBuilderV2_h_
#define _Utilities_UiDesigner_Theme_UiDesignerThemeBuilderV2_h_

#include "UiDesignerThemeGallery.h"

namespace Upp {

struct UiDesignerThemeSurfacePalette {
    Color paper;
    Color alternate;
    Color accent;
    Color ink;
    Color disabled;
    Color divider;
};

// Resolve Designer-owned painting colours from the active UiTheme rather than
// U++ SColor globals. This is shared by Theme Studio and catalog painting so a
// Dark Theme preview cannot leave light-only application chrome behind.
UiDesignerThemeSurfacePalette UiDesignerResolveThemeSurfacePalette();

class UiDesignerThemeGalleryV2 : public UiDesignerThemeGallery {
public:
    typedef UiDesignerThemeGalleryV2 CLASSNAME;

    UiDesignerThemeGalleryV2();

    void SetThemeDocument(UiDesignerThemeDocument *theme);
    void SetPanelRole(UiRole role);
    void SetControlRole(UiRole role);
    void RefreshTheme();

    UiRole GetUniversalPanelRole() const { return panel_role_v2_; }
    UiRole GetUniversalControlRole() const { return control_role_; }
    int GetDataSampleColumn() const;
    int GetChoicesSampleColumn() const;
    bool IsSaveSampleContained() const;

    void Layout() override;
    void Paint(Draw& w) override;

private:
    friend class UiDesignerThemeToolbarV2;

    void RebuildColumnPlacement();
    void RebindPanelSamples();
    void SelectPanelSample(const String& type,
                           UiDesignerThemeSelectableBase *sample);
    void SyncSelectedTargetV2();
    String CurrentStyleTargetV2(const UiDesignerThemeSnapshot& theme,
                                const String& type, bool panel_sample) const;
    void BuildSelectedPropertyModelV2(PropertyEditorModel& model,
                                      const UiDesignerThemeSnapshot& theme) const;
    void ApplySampleThemeV2(Ctrl& ctrl, const String& type, bool panel_sample);
    void ApplyThemeStylesV2();

    UiRole panel_role_v2_ = UiRole::Standard;
};

class UiDesignerThemeToolbarV2 : public UiDesignerThemeToolbar {
public:
    typedef UiDesignerThemeToolbarV2 CLASSNAME;

    UiDesignerThemeToolbarV2();
    UiDesignerThemeToolbarV2(UiDesignerThemeDocument& theme,
                             UiDesignerThemeGalleryV2& gallery)
        : UiDesignerThemeToolbarV2()
    {
        SetThemeDocument(&theme);
        SetGallery(&gallery);
    }

    void SetGallery(UiDesignerThemeGalleryV2 *gallery);
    void SetPanelRole(UiRole role) { SetPanelRoleV2(role); }
    void SetControlRole(UiRole role) { SetControlRoleV2(role); }
    UiRole GetUniversalPanelRole() const { return panel_role_v2_; }
    UiRole GetUniversalControlRole() const { return control_role_v2_; }

    void Layout() override;
    void Paint(Draw& w) override;

private:
    void SetPanelRoleV2(UiRole role);
    void SetControlRoleV2(UiRole role);

    UiDesignerThemeGalleryV2 *gallery_v2_ = nullptr;
    UiRole panel_role_v2_ = UiRole::Standard;
    UiRole control_role_v2_ = UiRole::Accent;
};

}

#endif
