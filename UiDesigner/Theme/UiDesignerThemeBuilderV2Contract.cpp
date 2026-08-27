#include "UiDesignerThemeBuilderV2.h"

namespace Upp {

static Color ThemeSurfaceFace(const UiFill& fill, Color fallback)
{
    return fill.IsSolid() && !IsNull(fill.color) ? fill.color : fallback;
}

UiDesignerThemeSurfacePalette UiDesignerResolveThemeSurfacePalette()
{
    const UiPanel::Style surface = UiTheme::ResolvePanel(UiRole::Standard);
    const UiPanel::Style subtle_surface = UiTheme::ResolvePanel(UiRole::Subtle);
    const UiLabel::Style text_style = UiTheme::ResolveLabel(UiRole::Standard);
    const UiLabel::Style subtle_text = UiTheme::ResolveLabel(UiRole::Subtle);
    const UiLabel::Style accent_text = UiTheme::ResolveLabel(UiRole::Accent);

    UiDesignerThemeSurfacePalette out;
    out.paper = ThemeSurfaceFace(surface.palette.face[ST_NORMAL], SColorPaper());
    out.alternate = ThemeSurfaceFace(subtle_surface.palette.face[ST_NORMAL], out.paper);
    out.accent = !IsNull(accent_text.palette.ink[ST_NORMAL])
        ? accent_text.palette.ink[ST_NORMAL] : SColorHighlight();
    out.ink = !IsNull(text_style.palette.ink[ST_NORMAL])
        ? text_style.palette.ink[ST_NORMAL] : SColorText();
    out.disabled = !IsNull(subtle_text.palette.ink[ST_DISABLED])
        ? subtle_text.palette.ink[ST_DISABLED]
        : (!IsNull(subtle_text.palette.ink[ST_NORMAL])
            ? subtle_text.palette.ink[ST_NORMAL] : SColorDisabled());
    out.divider = !IsNull(surface.palette.frame[ST_NORMAL])
        ? surface.palette.frame[ST_NORMAL] : SColorShadow();
    return out;
}

static int ThemeControlColumn(const Ctrl& ctrl,
                              const UiBoxLayout (&columns)[3])
{
    const Ctrl *parent = ctrl.GetParent();
    for(int i = 0; i < 3; ++i)
        if(parent == &columns[i])
            return i;
    return -1;
}

int UiDesignerThemeGalleryV2::GetDataSampleColumn() const
{
    return ThemeControlColumn(data_group_, control_columns_);
}

int UiDesignerThemeGalleryV2::GetChoicesSampleColumn() const
{
    return ThemeControlColumn(choices_group_, control_columns_);
}

bool UiDesignerThemeGalleryV2::IsSaveSampleContained() const
{
    const Rect r = split_button_.GetRect();
    const int width = buttons_group_.GetSize().cx;
    return r.left >= 0 && r.right <= width;
}

}
